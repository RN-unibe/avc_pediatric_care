#pragma once

#include "Animation/AnimInstance.h"
#include "PatientAnimInstance.generated.h"

UCLASS(Blueprintable)
class CPP_VR2_API UPatientAnimInstance : public UAnimInstance
{
    GENERATED_BODY()

public:
    // 0.0 = fully Idle, 10.0 = fully React (touched)
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Patient")
    float TouchAlpha = 0.0f;

    // True while the patient is held/picked up
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Patient")
    bool bIsHeld = false;
};
