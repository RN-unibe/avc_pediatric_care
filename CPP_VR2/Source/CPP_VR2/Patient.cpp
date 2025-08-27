#include "Patient.h"
#include "Components/SkeletalMeshComponent.h"
#include "Components/BoxComponent.h"
#include "Animation/AnimationAsset.h"
#include "Animation/AnimTypes.h" 

APatient::APatient() {
    PrimaryActorTick.bCanEverTick = false;

    // Mesh as root
    Mesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("SkeletalMesh"));
    RootComponent = Mesh;

    // Trigger box attached to mesh
    TriggerVolume = CreateDefaultSubobject<UBoxComponent>(TEXT("TriggerVolume"));
    TriggerVolume->SetupAttachment(RootComponent);
    TriggerVolume->SetBoxExtent(FVector(20.f, 20.f, 20.f));
    TriggerVolume->SetCollisionProfileName(TEXT("Trigger"));
    TriggerVolume->SetGenerateOverlapEvents(true);
}

void APatient::BeginPlay() {
    Super::BeginPlay();

    // Ensure the mesh will actually play raw animation assets
    if (Mesh) {
        Mesh->SetAnimationMode(EAnimationMode::Type::AnimationSingleNode);
        PlayIdleAnimation();
    }

    if (TriggerVolume) {
        TriggerVolume->OnComponentBeginOverlap.AddDynamic(this, &APatient::OnHandOverlapBegin);
        TriggerVolume->OnComponentEndOverlap.AddDynamic(this, &APatient::OnHandOverlapEnd);
    }
}

void APatient::OnHandOverlapBegin(
    UPrimitiveComponent* /*OverlappedComp*/, AActor* OtherActor,
    UPrimitiveComponent* OtherComp, int32, bool, const FHitResult&)
{
    const bool IsHandActor = OtherActor && OtherActor->ActorHasTag(TEXT("PlayerHand"));
    const bool IsHandComp = OtherComp && OtherComp->ComponentHasTag(TEXT("PlayerHand"));
    if (IsHandActor || IsHandComp)
    {
        GEngine->AddOnScreenDebugMessage(-1, 1.5f, FColor::Green, TEXT("Hand overlap!"));

	    PlayTouchAnimation();
    }
}

void APatient::OnHandOverlapEnd(
    UPrimitiveComponent* /*OverlappedComp*/, AActor* OtherActor,
    UPrimitiveComponent* OtherComp, int32)
{
    const bool IsHandActor = OtherActor && OtherActor->ActorHasTag(TEXT("PlayerHand"));
    const bool IsHandComp = OtherComp && OtherComp->ComponentHasTag(TEXT("PlayerHand"));
    if (IsHandActor || IsHandComp)
    {
        GEngine->AddOnScreenDebugMessage(-1, 1.5f, FColor::Green, TEXT("Hand left!"));

	    PlayIdleAnimation();
    }
}



void APatient::PlayTouchAnimation() {
    if (Mesh && TouchAnimation) {
        Mesh->PlayAnimation(TouchAnimation, /*bLoop=*/false);
    }
}

void APatient::PlayIdleAnimation() {
    if (Mesh && IdleAnimation) {
        Mesh->PlayAnimation(IdleAnimation, /*bLoop=*/true);
    }
}
