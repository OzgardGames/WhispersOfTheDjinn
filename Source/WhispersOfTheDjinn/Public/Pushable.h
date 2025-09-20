// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "WOD_Character.h"
#include "UObject/Interface.h"
#include "Pushable.generated.h"

/**
 * 
 */

UINTERFACE(Blueprintable, MinimalAPI)
class UPushable : public UInterface
{
	GENERATED_BODY()
};

class WHISPERSOFTHEDJINN_API IPushable
{
	GENERATED_BODY()

public :
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Pushable")
	void OnStartPush(AWOD_Character* Pusher);
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Pushable")
	void OnStopPush();

	virtual bool CanBePushed() const = 0;
	virtual bool IsBeingPushed() const = 0;
	virtual float GetWeight() const = 0;

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Pushable")
	USceneComponent* GetSnapPoint() const;
};
