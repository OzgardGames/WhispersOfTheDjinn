// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "BaseAnimInstance.generated.h"

UENUM(BlueprintType)
enum class ECharacterAnimState : uint8
{
    Idle        UMETA(DisplayName = "Idle"),
    Walking        UMETA(DisplayName = "Walk"),
    Running         UMETA(DisplayName = "Run"),
    Jumping         UMETA(DisplayName = "Jump"),
    Crouching      UMETA(DisplayName = "Crouch"),

    // Brother specific
    Carrying    UMETA(DisplayName = "Carrying"),
    Sliding     UMETA(DisplayName = "Sliding"),
    Drawing     UMETA(DisplayName = "Drawing"),

    // Sister specific
    AimingLantern  UMETA(DisplayName = "Aiming Lantern"),
    CalmingBrother UMETA(DisplayName = "Calming Brother"),
    Reading        UMETA(DisplayName = "Reading"),
};
/**
 * 
 */
UCLASS()
class WHISPERSOFTHEDJINN_API UBaseAnimInstance : public UAnimInstance
{
	GENERATED_BODY()
	
public:

    void SetAnimState(ECharacterAnimState newAnimState);
    ECharacterAnimState GetAnimState();

	UFUNCTION(BlueprintCallable, Category = "Animation")
    ECharacterAnimState MapPlayerStateToAnim(EPlayerState State);

    UPROPERTY(Replicated, EditAnywhere, BlueprintReadOnly, Category = "State")
    ECharacterAnimState AnimState;

    virtual void NativeUpdateAnimation(float DeltaSeconds) override;
};