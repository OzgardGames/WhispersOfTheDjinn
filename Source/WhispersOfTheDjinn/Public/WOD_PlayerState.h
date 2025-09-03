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


/**
 * 
 */

UCLASS()
class WHISPERSOFTHEDJINN_API AWOD_PlayerState : public APlayerState
{
	GENERATED_BODY()

public:
	AWOD_PlayerState();

	void SetCoopRole(ECoopRole newRole);
	ECoopRole GetCoopRole() const;

	void SetPlayerRole(EPlayerRole newPlayerRole);
	EPlayerRole GetPlayerRole() const;	

protected:

	UPROPERTY(Replicated, EditAnywhere, BlueprintReadOnly, Category = "Role")
	ECoopRole CoopRole;

	UPROPERTY(Replicated, EditAnywhere, BlueprintReadOnly, Category = "PlayerRole")
	EPlayerRole PlayerRole = EPlayerRole::Sister;

	void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const;

	
};
