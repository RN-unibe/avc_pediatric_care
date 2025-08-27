#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Patient.generated.h"

class USkeletalMeshComponent;
class UBoxComponent;
class UPatientAnimInstance;

UCLASS()
class CPP_VR2_API APatient : public AActor
{
    GENERATED_BODY()

public:
    APatient();

protected:
    virtual void BeginPlay() override;

    UFUNCTION()
    void OnHandOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
        UPrimitiveComponent* OtherComp, int32 OtherBodyIndex,
        bool bFromSweep, const FHitResult& SweepResult);

    UFUNCTION()
    void OnHandOverlapEnd(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
        UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

public:
    // Call these from your grab logic later
    UFUNCTION(BlueprintCallable, Category = "Patient")
    void SetHeld(bool bHeld);

private:
    // Smoothly change TouchAlpha over 'Duration' seconds
    void SetTouchTarget(float NewTarget, float Duration = 0.6f);
    void TickTouch(); // timer callback

private:
    UPROPERTY(VisibleAnywhere)
    USkeletalMeshComponent* Mesh = nullptr;

    UPROPERTY(VisibleAnywhere)
    UBoxComponent* TriggerVolume = nullptr;

    // Runtime anim instance (owned by SkeletalMesh)
    UPROPERTY(Transient)
    UPatientAnimInstance* PatientAnim = nullptr;

    float TouchAlphaCurrent = 0.f;
    float TouchAlphaTarget = 0.f;

    FTimerHandle TouchTimer;

    UPROPERTY(EditAnywhere, Category = "Patient|Timing")
    float TouchBlendDuration = 0.6f; // seconds
};
