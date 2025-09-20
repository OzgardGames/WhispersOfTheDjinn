// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "WOD_Character.h"
#include "Animation/AnimInstance.h"
#include "BaseAnimInstance.generated.h"

/**
 * 
 */
UCLASS()
class WHISPERSOFTHEDJINN_API UBaseAnimInstance : public UAnimInstance
{
	GENERATED_BODY()
	
public:

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Animation State")
    EAnimState CurrentAnimState;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Animation State")
    bool aIsCrouching = false;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Animation State")
    bool aIsHanging = false;

    virtual void NativeUpdateAnimation(float DeltaSeconds) override;
};