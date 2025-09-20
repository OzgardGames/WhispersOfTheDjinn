// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "WOD_Character.h"
#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "Pickable.generated.h"

/**
 * 
 */

UINTERFACE(Blueprintable, MinimalAPI)
class UPickable : public UInterface
{
	GENERATED_BODY()
};

class WHISPERSOFTHEDJINN_API IPickable
{
	GENERATED_BODY()

public:

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Pickable")
	void OnPickedUp();
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Pickable")
	void OnDropped();

	virtual bool CanBePickedUp() const = 0;
	virtual bool IsHeld() const = 0;
	virtual float GetWeight() const = 0;
};
