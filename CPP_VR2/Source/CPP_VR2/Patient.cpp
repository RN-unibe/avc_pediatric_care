#include "Patient.h"
#include "Components/SkeletalMeshComponent.h"
#include "Components/BoxComponent.h"
#include "TimerManager.h"
#include "PatientAnimInstance.h"
#include "Animation/AnimInstance.h"
#include "Animation/AnimTypes.h" // EAnimationMode

APatient::APatient() {
    PrimaryActorTick.bCanEverTick = false;

    Mesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("SkeletalMesh"));
    RootComponent = Mesh;

    TriggerVolume = CreateDefaultSubobject<UBoxComponent>(TEXT("TriggerVolume"));
    TriggerVolume->SetupAttachment(RootComponent);
    TriggerVolume->SetBoxExtent(FVector(25.f, 25.f, 25.f));
    TriggerVolume->SetCollisionProfileName(TEXT("Trigger"));
    TriggerVolume->SetGenerateOverlapEvents(true);
}

void APatient::BeginPlay() {
    Super::BeginPlay();

    // Make sure the mesh uses an AnimBlueprint (we’ll assign it in the editor)
    Mesh->SetAnimationMode(EAnimationMode::AnimationBlueprint);

    // Cache our custom anim instance (will be valid after BeginPlay if ABP is set)
    PatientAnim = Cast<UPatientAnimInstance>(Mesh->GetAnimInstance());
    if (!PatientAnim) {
        UE_LOG(LogTemp, Warning, TEXT("APatient: AnimInstance is not UPatientAnimInstance. Did you assign the ABP?"));
    }

    if (TriggerVolume) {
        TriggerVolume->OnComponentBeginOverlap.AddDynamic(this, &APatient::OnHandOverlapBegin);
        TriggerVolume->OnComponentEndOverlap.AddDynamic(this, &APatient::OnHandOverlapEnd);
    }

    // Initialize
    TouchAlphaCurrent = 0.f;
    TouchAlphaTarget = 0.f;
    if (PatientAnim) PatientAnim->TouchAlpha = 0.f;
}

void APatient::OnHandOverlapBegin(UPrimitiveComponent*, AActor* OtherActor,
    UPrimitiveComponent* OtherComp, int32,
    bool, const FHitResult&) {

    const bool IsHandActor = (OtherActor && OtherActor->ActorHasTag(TEXT("PlayerHand")));
    const bool IsHandComp = (OtherComp && OtherComp->ComponentHasTag(TEXT("PlayerHand")));

    if (IsHandActor || IsHandComp) {
        SetTouchTarget(10.f, TouchBlendDuration); // ease in to "react"
    }
}

void APatient::OnHandOverlapEnd(UPrimitiveComponent*, AActor* OtherActor,
    UPrimitiveComponent* OtherComp, int32) {

    const bool IsHandActor = (OtherActor && OtherActor->ActorHasTag(TEXT("PlayerHand")));
    const bool IsHandComp = (OtherComp && OtherComp->ComponentHasTag(TEXT("PlayerHand")));

    if (IsHandActor || IsHandComp) {
        SetTouchTarget(0.f, TouchBlendDuration); // ease out to idle
    }
}

void APatient::SetHeld(bool bHeld) {
    if (PatientAnim) {
        PatientAnim->bIsHeld = bHeld;
    }
}

void APatient::SetTouchTarget(float NewTarget, float Duration) {
    TouchAlphaTarget = FMath::Clamp(NewTarget, 0.f, 10.f);

    if (!PatientAnim) {
        // Try to recache if something changed at runtime
        PatientAnim = Cast<UPatientAnimInstance>(Mesh ? Mesh->GetAnimInstance() : nullptr);
        if (!PatientAnim) return;
    }

    // If duration is tiny, snap
    if (Duration <= KINDA_SMALL_NUMBER) {
        TouchAlphaCurrent = TouchAlphaTarget;
        PatientAnim->TouchAlpha = TouchAlphaCurrent;

        GetWorldTimerManager().ClearTimer(TouchTimer);

        return;
    }

    // Start a small timer to interpolate smoothly
    GetWorldTimerManager().ClearTimer(TouchTimer);
    const float Step = 0.02f; // 50 Hz
    GetWorldTimerManager().SetTimer(TouchTimer, this, &APatient::TickTouch, Step, true);
}

void APatient::TickTouch() {
    if (!PatientAnim) {
        GetWorldTimerManager().ClearTimer(TouchTimer);

        return;
    }

    // Move current toward target with a fixed rate (approximate duration)
    const float Step = 0.02f;        // timer tick
    const float Rate = 1.f / FMath::Max(SMALL_NUMBER, TouchBlendDuration);

    TouchAlphaCurrent = FMath::FInterpConstantTo(TouchAlphaCurrent, TouchAlphaTarget, Step, Rate);

    PatientAnim->TouchAlpha = TouchAlphaCurrent;

    // Stop when close enough
    if (FMath::IsNearlyEqual(TouchAlphaCurrent, TouchAlphaTarget, 0.005f)) {
        TouchAlphaCurrent = TouchAlphaTarget;
        PatientAnim->TouchAlpha = TouchAlphaCurrent;
        GetWorldTimerManager().ClearTimer(TouchTimer);
    }
}

void APatient::Grab(USceneComponent* AttachParent, FName SocketName)
{
    if (!AttachParent || !Mesh) return;

    // Stop any ragdoll/physics while carried
    Mesh->SetSimulatePhysics(false);
    Mesh->SetCollisionEnabled(ECollisionEnabled::QueryOnly); // no blocking while carried
    Mesh->SetCollisionResponseToAllChannels(ECR_Ignore);      // optional: prevent bumps while carried
    Mesh->SetCollisionResponseToChannel(ECC_Visibility, ECR_Block); // keep traces if you like

    // Attach to the player's hand/controller
    FAttachmentTransformRules Rules(FAttachmentTransformRules::KeepWorldTransform);
    Mesh->AttachToComponent(AttachParent, Rules, SocketName);

    // Drive animation pose
    SetHeld(true); // sets AnimInstance->bIsHeld = true
}

void APatient::Release(FVector LinearVelocity, FVector AngularVelocityDeg)
{
    if (!Mesh) return;

    Mesh->DetachFromComponent(FDetachmentTransformRules::KeepWorldTransform);
    Mesh->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
    Mesh->SetCollisionResponseToAllChannels(ECR_Block);

    Mesh->SetSimulatePhysics(false);
    Mesh->SetPhysicsLinearVelocity(LinearVelocity);

    const FVector AngularRad = FMath::DegreesToRadians(AngularVelocityDeg);
    Mesh->SetPhysicsAngularVelocityInRadians(AngularRad);

    SetHeld(false);
}

