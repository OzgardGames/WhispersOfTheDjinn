// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "WOD_PlayerState.h"
#include "BaseAnimInstance.generated.h"

/**
 * 
 */
UCLASS()
class WHISPERSOFTHEDJINN_API UBaseAnimInstance : public UAnimInstance
{
	GENERATED_BODY()
	
public:

    void SetAnimState(EPlayerState newAnimState);
    EPlayerState GetAnimState();

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Animation State")
    EPlayerState AnimState;

    virtual void NativeUpdateAnimation(float DeltaSeconds) override;
};