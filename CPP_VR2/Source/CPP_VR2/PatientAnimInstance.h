#pragma once

#include "Animation/AnimInstance.h"
#include "PatientAnimInstance.generated.h"

UCLASS(Blueprintable)
class CPP_VR2_API UPatientAnimInstance : public UAnimInstance
{
    GENERATED_BODY()

public:
    // Animation Sequence Variables
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Patient")
    float TouchAlpha = 0.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Patient")
    float FadeOutTime = 4.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Patient")
    bool bIsHeldAnim = false;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Patient")
    bool SoundIsPlaying = false;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Patient")
    UAudioComponent* ActiveSound;


    // Head Movement
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "LookAt")
    FVector LookAtLocationWS = FVector::ZeroVector;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "LookAt")
    float HeadLookAlpha = 0.0f;  // blend how much the head follows

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "LookAt")
    float MaxHeadYaw = 60.f;   // degrees

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "LookAt")
    float MaxHeadPitch = 35.f; // degrees

};
