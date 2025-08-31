#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Patient.generated.h"

class USkeletalMeshComponent;
class UBoxComponent;
class UPatientAnimInstance;
class USceneComponent;

UCLASS()
class CPP_VR2_API APatient : public AActor
{
    GENERATED_BODY()

public:
    APatient();

    UFUNCTION(BlueprintCallable, Category = "Patient")
    void SetHeld(bool bHeld);

    UFUNCTION(BlueprintCallable, Category = "Patient|Return")
    void SnapBackToHome();

    UPROPERTY(EditAnywhere, Category = "Patient|Return")
    bool bSnapOnRelease = true;

    UPROPERTY(EditAnywhere, Category = "Patient|Return")
    bool bEnableGravityWhenNotSnapping = true;


protected:
    virtual void BeginPlay() override;

    UFUNCTION()
    void OnHandOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
        UPrimitiveComponent* OtherComp, int32 OtherBodyIndex,
        bool bFromSweep, const FHitResult& SweepResult);

    UFUNCTION()
    void OnHandOverlapEnd(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
        UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

private:
    void SetTouchTarget(float NewTarget, float Duration = 0.6f);
    void TickTouch();
    void Tick(float DeltaSeconds) override;

    UPROPERTY(VisibleAnywhere)
    USkeletalMeshComponent* Mesh = nullptr;

    UPROPERTY(VisibleAnywhere)
    UBoxComponent* TriggerVolume = nullptr;

    UPROPERTY(Transient)
    UPatientAnimInstance* PatientAnim = nullptr;

    float TouchAlphaCurrent = 0.f;
    float TouchAlphaTarget = 0.f;
    FTimerHandle TouchTimer;

    UPROPERTY(EditAnywhere, Category = "Patient|Timing")
    float TouchBlendDuration = 0.6f;

    FTransform InitialActorTransform;

};
