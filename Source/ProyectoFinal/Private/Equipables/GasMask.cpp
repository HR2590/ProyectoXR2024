#include "VRPawn.generated.h"
#include "Equipables/GasMask.h"


AGasMask::AGasMask()
{
    PrimaryActorTick.bCanEverTick = true;

    MaskMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MaskMesh"));
    RootComponent = MaskMesh;

    bIsEquipped = false;

    ProximitySphere = CreateDefaultSubobject<USphereComponent>(TEXT("HeadCollision"));
    ProximitySphere->SetupAttachment(RootComponent);  // Asegúrate de que esté adjunto al RootComponent
    ProximitySphere->SetSphereRadius(10.0f);  // Ajusta el radio según sea necesario
    ProximitySphere->SetCollisionProfileName(TEXT("OverlapAll"));  // Configura el perfil de colisión
}

void AGasMask::BeginPlay()
{
    Super::BeginPlay();
}
