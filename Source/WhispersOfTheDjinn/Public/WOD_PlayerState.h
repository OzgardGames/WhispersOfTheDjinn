// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerState.h"
#include "WOD_PlayerState.generated.h"

UENUM(BlueprintType)
enum class ECoopRole : uint8
{
	PlayerOne UMETA(DisplayName = "Player One"),
	PlayerTwo UMETA(DisplayName = "Player Two")
};

UENUM(BlueprintType)
enum class EPlayerRole : uint8
{
	Sister UMETA(DisplayName = "Sister"),
	Brother UMETA(DisplayName = "Brother")
};

UENUM(BlueprintType)
enum class EPlayerState : uint8
{
	Idle UMETA(DisplayName = "Idle"),
	Walking UMETA(DisplayName = "Walk"),
	Running UMETA(DisplayName = "Run"),
	Jumping UMETA(DisplayName = "Jump"),
	Crouching UMETA(DisplayName = "Crouch"),
	Carrying UMETA(DisplayName = "Carry"),
};
/**
 * 
 */
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnPlayerStateChanged, EPlayerState, NewState);

UCLASS()
class WHISPERSOFTHEDJINN_API AWOD_PlayerState : public APlayerState
{
	GENERATED_BODY()

public:
	AWOD_PlayerState();

	void SetRole(ECoopRole newRole);
	ECoopRole GetRole() const;

	void SetPlayerRole(EPlayerRole newPlayerRole);
	EPlayerRole GetPlayerRole() const;	
	
	void SetState(EPlayerState newState);
	EPlayerState GetState() const;

	UPROPERTY(BlueprintAssignable, Category = "PlayerState")
	FOnPlayerStateChanged OnPlayerStateChanged;


protected:


	UPROPERTY(Replicated, EditAnywhere, BlueprintReadOnly, Category = "Role")
	ECoopRole CoopRole;

	UPROPERTY(Replicated, EditAnywhere, BlueprintReadOnly, Category = "PlayerRole")
	EPlayerRole PlayerRole = EPlayerRole::Sister;

	UPROPERTY(Replicated, EditAnywhere, BlueprintReadOnly, Category = "State")
	EPlayerState PlayerState = EPlayerState::Idle;


	
};
