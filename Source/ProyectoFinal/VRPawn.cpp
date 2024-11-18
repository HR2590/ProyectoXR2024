#include "VRPawn.h"
#include "Camera/CameraComponent.h"
#include "MotionControllerComponent.h"
#include <EnhancedInputComponent.h>
#include <EnhancedInputSubsystems.h>
#include <DrawerActor.h>

AVRPawn::AVRPawn()
{
	PrimaryActorTick.bCanEverTick = true;

	VRCore = CreateDefaultSubobject<USceneComponent>(TEXT("VR_Body"));
	RootComponent = VRCore;

	VRCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("VR_Camera"));
	VRCamera->SetupAttachment(RootComponent);

	L_MotionController = CreateDefaultSubobject<UMotionControllerComponent>(TEXT("L_MotionController"));
	L_MotionController->SetupAttachment(RootComponent);
	L_MotionController->SetTrackingMotionSource(FName("Left"));

	R_MotionController = CreateDefaultSubobject<UMotionControllerComponent>(TEXT("R_MotionController"));
	R_MotionController->SetupAttachment(RootComponent);
	R_MotionController->SetTrackingMotionSource(FName("Right"));

	AnchorPointLeft = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Anchor_Point_Left"));
	AnchorPointLeft->SetupAttachment(L_MotionController);

	AnchorPointRight = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Anchor_Point_Right"));
	AnchorPointRight->SetupAttachment(R_MotionController);
}

void AVRPawn::BeginPlay()
{
	Super::BeginPlay();

	PlayerController = Cast<APlayerController>(GetController());

	if (PlayerController)
	{
		UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer());
		if (Subsystem)
		{
			Subsystem->AddMappingContext(InputMappingContext, 0);
		}
	}
}

void AVRPawn::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (ObjectGrabbed && CaughtComponent)
	{
		FVector TargetLocation = L_MotionController->GetComponentLocation();
		FRotator TargetRotation = L_MotionController->GetComponentRotation();

		CaughtComponent->SetWorldLocationAndRotation(TargetLocation, TargetRotation);
	}
}

void AVRPawn::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	if (UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(PlayerInputComponent))
	{
		EnhancedInputComponent->BindAction(TeleportAction, ETriggerEvent::Triggered, this, &AVRPawn::HandleTeleport, DISTANCE_TELEPORT);
		EnhancedInputComponent->BindAction(GrabAction, ETriggerEvent::Started, this, &AVRPawn::PickupObject, DISTANCE_GRAB);
	}
}

void AVRPawn::PickupObject(float _distance)
{
	FVector Location = L_MotionController->GetComponentLocation();
	FVector EndLocation = Location + (L_MotionController->GetForwardVector() * _distance);
	FHitResult HitResult;

	bool raycastHit = PerformRaycast(Location, EndLocation, HitResult);

	if (raycastHit)
	{
		UPrimitiveComponent* HitComponent = HitResult.GetComponent();

		if (!ObjectGrabbed && HitComponent)
		{
			HandleObjectPickup(HitComponent);
		}
		else if (ObjectGrabbed && HitComponent)
		{
			ReleaseObject(HitComponent);
		}
	}
}

void AVRPawn::HandleObjectPickup(UPrimitiveComponent* HitComponent)
{
	if (HitComponent->ComponentHasTag(PICKABLE_TAG))
	{
		PickupPhysicsObject(HitComponent);
	}
	else if (HitComponent->ComponentHasTag(DRAWER_TAG))
	{
		PickupDrawerObject(HitComponent);
	}
}

void AVRPawn::PickupPhysicsObject(UPrimitiveComponent* HitComponent)
{
	CaughtComponent = HitComponent;
	if (HitComponent->IsSimulatingPhysics())
	{
		HitComponent->AttachToComponent(L_MotionController, FAttachmentTransformRules::SnapToTargetIncludingScale);
		HitComponent->SetSimulatePhysics(false);
		ObjectGrabbed = true;
	}
}

void AVRPawn::PickupDrawerObject(UPrimitiveComponent* HitComponent)
{
	AActor* OwnerActor = HitComponent->GetOwner();

	if (OwnerActor && OwnerActor->GetClass()->ImplementsInterface(UInteractable::StaticClass()))
	{
		//ADrawerActor* drawerObjectClass = Cast<ADrawerActor>(OwnerActor);
		//drawerObjectClass->CallDrawerAction(HitComponent);
		IInteractable::Execute_InteractionHit(OwnerActor, HitComponent);
	}
}

void AVRPawn::ReleaseObject(UPrimitiveComponent* HitComponent)
{
	if (ObjectGrabbed && HitComponent)
	{
		HitComponent->DetachFromComponent(FDetachmentTransformRules::KeepWorldTransform);
		HitComponent->SetSimulatePhysics(true);
		ObjectGrabbed = false;
	}
}


void AVRPawn::HandleTeleport(float _distance)
{
	if (PlayerController) 
	{
		FVector Location;
		FRotator Rotation;

		PlayerController->GetPlayerViewPoint(Location, Rotation);
		FVector EndLocation = Location + (Rotation.Vector() * _distance);
		FHitResult HitResult;

		bool raycastHit = PerformRaycast(Location, EndLocation, HitResult);

		if (raycastHit) 
		{
			DrawDebugSphere(GetWorld(), HitResult.Location, 10.f, 12, FColor::Red, false, 1000);
			FVector TeleportLocation = FVector(HitResult.Location.X, HitResult.Location.Y, this->GetActorLocation().Z);
			this->SetActorLocation(TeleportLocation);
		}
	}
}

bool AVRPawn::PerformRaycast(FVector _location, FVector _endLocation, FHitResult& _hitResult)
{
	FCollisionQueryParams TraceParams;
	TraceParams.bTraceComplex = false;
	TraceParams.AddIgnoredActor(this);

	bool raycastHit = GetWorld()->LineTraceSingleByChannel(
		_hitResult,
		_location,
		_endLocation,
		ECC_Visibility,
		TraceParams
	);

	return raycastHit;
}