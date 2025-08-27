#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Patient.generated.h"

class USkeletalMeshComponent;
class UBoxComponent;
class UAnimationAsset; 

UCLASS()
class CPP_VR2_API APatient : public AActor {
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

    void PlayTouchAnimation();
    void PlayIdleAnimation();

private:
    UPROPERTY(VisibleAnywhere)
    USkeletalMeshComponent* Mesh;

    UPROPERTY(VisibleAnywhere)
    UBoxComponent* TriggerVolume;

    UPROPERTY(EditAnywhere, Category = "Animation")
    UAnimationAsset* TouchAnimation = nullptr;

    UPROPERTY(EditAnywhere, Category = "Animation")
    UAnimationAsset* IdleAnimation = nullptr;
};
