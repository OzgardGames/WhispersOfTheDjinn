// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "WOD_Character.h"
#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "Pickable.generated.h"

/**
 * 
 */

UINTERFACE()
class UPickable : public UInterface
{
	GENERATED_BODY()
};

class WHISPERSOFTHEDJINN_API IPickable
{
	GENERATED_BODY()
public:

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Pickup")
	void AttachToHand(AWOD_Character* PickingCharacter);

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Pickup")
	void DropToGround(AWOD_Character* PickingCharacter);
};
