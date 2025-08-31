#include "Patient.h"
#include "Components/SkeletalMeshComponent.h"
#include "Components/BoxComponent.h"
#include "TimerManager.h"
#include "PatientAnimInstance.h"
#include "Kismet/GameplayStatics.h"
#include "Camera/CameraComponent.h"

APatient::APatient()
{
    PrimaryActorTick.bCanEverTick = true;

    Mesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("SkeletalMesh"));
    RootComponent = Mesh; 

    TriggerVolume = CreateDefaultSubobject<UBoxComponent>(TEXT("TriggerVolume"));
    TriggerVolume->SetupAttachment(Mesh);
    TriggerVolume->SetBoxExtent(FVector(25.f, 25.f, 25.f));
    TriggerVolume->SetCollisionProfileName(TEXT("Trigger"));
    TriggerVolume->SetGenerateOverlapEvents(true);
}

void APatient::BeginPlay()
{
    Super::BeginPlay();

    Mesh->SetAnimationMode(EAnimationMode::AnimationBlueprint);
    PatientAnim = Cast<UPatientAnimInstance>(Mesh->GetAnimInstance());

    if (!PatientAnim)
    {
        UE_LOG(LogTemp, Warning, TEXT("APatient: AnimInstance is not UPatientAnimInstance. Did you set the ABP?"));
    }

    if (TriggerVolume)
    {
        TriggerVolume->OnComponentBeginOverlap.AddDynamic(this, &APatient::OnHandOverlapBegin);
        TriggerVolume->OnComponentEndOverlap.AddDynamic(this, &APatient::OnHandOverlapEnd);
    }

    TouchAlphaCurrent = 0.f;
    TouchAlphaTarget = 0.f;
    if (PatientAnim) PatientAnim->TouchAlpha = 0.f;

    InitialActorTransform = GetActorTransform();

    Mesh->SetSimulatePhysics(false);
    Mesh->SetEnableGravity(false);
    Mesh->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
    Mesh->SetCollisionResponseToAllChannels(ECR_Block);
}

void APatient::OnHandOverlapBegin(UPrimitiveComponent*, AActor* OtherActor,
    UPrimitiveComponent* OtherComp, int32,
    bool, const FHitResult&)
{
    const bool IsHandActor = (OtherActor && OtherActor->ActorHasTag(TEXT("PlayerHand")));
    const bool IsHandComp = (OtherComp && OtherComp->ComponentHasTag(TEXT("PlayerHand")));
    if (IsHandActor || IsHandComp)
    {
        SetTouchTarget(10.f, TouchBlendDuration);
    }
}

void APatient::OnHandOverlapEnd(UPrimitiveComponent*, AActor* OtherActor,
    UPrimitiveComponent* OtherComp, int32)
{
    const bool IsHandActor = (OtherActor && OtherActor->ActorHasTag(TEXT("PlayerHand")));
    const bool IsHandComp = (OtherComp && OtherComp->ComponentHasTag(TEXT("PlayerHand")));
    if (IsHandActor || IsHandComp)
    {
        SetTouchTarget(0.f, TouchBlendDuration);
    }
}

void APatient::SetHeld(bool bHeld)
{
    if (PatientAnim)
    {
        PatientAnim->bIsHeldAnim = bHeld;
    }
}

void APatient::SetTouchTarget(float NewTarget, float Duration)
{
    TouchAlphaTarget = FMath::Clamp(NewTarget, 0.f, 10.f);

    if (!PatientAnim)
    {
        PatientAnim = Cast<UPatientAnimInstance>(Mesh ? Mesh->GetAnimInstance() : nullptr);
        if (!PatientAnim) return;
    }

    if (Duration <= KINDA_SMALL_NUMBER)
    {
        TouchAlphaCurrent = TouchAlphaTarget;
        PatientAnim->TouchAlpha = TouchAlphaCurrent;
        GetWorldTimerManager().ClearTimer(TouchTimer);
        return;
    }

    GetWorldTimerManager().ClearTimer(TouchTimer);
    const float Step = 0.02f;
    GetWorldTimerManager().SetTimer(TouchTimer, this, &APatient::TickTouch, Step, true);
}


void APatient::Tick(float DeltaSeconds)
{
    Super::Tick(DeltaSeconds);

    if (!PatientAnim) {
        PatientAnim = Cast<UPatientAnimInstance>(Mesh ? Mesh->GetAnimInstance() : nullptr);
        if (!PatientAnim) return;
    }

    APlayerController* PC = UGameplayStatics::GetPlayerController(this, 0);
    if (!PC) return;
    APawn* PlayerPawn = PC->GetPawn();
    if (!PlayerPawn) return;

    UCameraComponent* Cam = PlayerPawn->FindComponentByClass<UCameraComponent>();
    const FVector Target = Cam ? Cam->GetComponentLocation() : PlayerPawn->GetActorLocation();

    const float Dist = FVector::Dist(Target, GetActorLocation());
    float Alpha = 1.0f;
    const float StartFollow = 300.f; // start following at 3 m
    const float FullFollow = 120.f; // full follow within 1.2 m
    if (Dist > StartFollow) {
        Alpha = 0.f;
    }
    else if (Dist > FullFollow) {
        Alpha = 1.f - (Dist - FullFollow) / (StartFollow - FullFollow);
    }

    PatientAnim->LookAtLocationWS = Target;
    PatientAnim->HeadLookAlpha = Alpha; 
}

void APatient::TickTouch()
{
    if (!PatientAnim)
    {
        GetWorldTimerManager().ClearTimer(TouchTimer);
        return;
    }

    if (PatientAnim->bIsHeldAnim)
    {
        TouchAlphaTarget = 0.f;
    }

    const float Step = 0.02f;
    const float Rate = 1.f / FMath::Max(SMALL_NUMBER, TouchBlendDuration);

    TouchAlphaCurrent = FMath::FInterpConstantTo(TouchAlphaCurrent, TouchAlphaTarget, Step, Rate);
    PatientAnim->TouchAlpha = TouchAlphaCurrent;

    if (FMath::IsNearlyEqual(TouchAlphaCurrent, TouchAlphaTarget, 0.005f))
    {
        TouchAlphaCurrent = TouchAlphaTarget;
        PatientAnim->TouchAlpha = TouchAlphaCurrent;
        GetWorldTimerManager().ClearTimer(TouchTimer);
    }
}




void APatient::SnapBackToHome()
{
    DetachFromActor(FDetachmentTransformRules::KeepWorldTransform);
    if (Mesh && Mesh->GetAttachParent())
    {
        Mesh->DetachFromComponent(FDetachmentTransformRules::KeepWorldTransform);
    }

    SetActorTransform(InitialActorTransform, /*bSweep=*/false, /*OutHit=*/nullptr, ETeleportType::TeleportPhysics);
}
