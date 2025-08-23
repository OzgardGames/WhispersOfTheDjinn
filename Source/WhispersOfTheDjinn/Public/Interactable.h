// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "Interactable.generated.h"

/**
 * 
 */

UINTERFACE(MinimalAPI,Blueprintable)
class UInteractable : public UInterface
{
	GENERATED_BODY()
};

class WHISPERSOFTHEDJINN_API IInteractable
{
	GENERATED_BODY()

public:
	
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Interaction")
	void Interact(AActor* InteractingActor);

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Interaction")
	FTransform GetSnapTransform();
};
