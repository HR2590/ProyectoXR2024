// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include <EngineFwd.h>
#include <VREditorInteractor.h>
#include <UserSettings/EnhancedInputUserSettings.h>
#include "VRPawn.generated.h"

UCLASS()
class PROYECTOFINAL_API AVRPawn : public APawn
{
	GENERATED_BODY()

public:
	AVRPawn();

protected:
	virtual void BeginPlay() override;
	USceneComponent* VRCore;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "VR Set")
	UCameraComponent* VRCamera;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "VR Controllers")
	UMotionControllerComponent* L_MotionController;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "VR Controllers")
	UMotionControllerComponent* R_MotionController;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "VR Controllers Mesh")
	UStaticMeshComponent* AnchorPointLeft;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "VR Controllers Mesh")
	UStaticMeshComponent* AnchorPointRight;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Input")
	UInputMappingContext* InputMappingContext;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Input")
	UInputAction* TeleportAction;

public:
	virtual void Tick(float DeltaTime) override;

	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	void HandleTeleport();

private:
	const float DISTANCE = 1000;
	APlayerController* PlayerController;

};


