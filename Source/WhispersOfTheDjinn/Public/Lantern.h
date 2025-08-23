// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Components/PointLightComponent.h"
#include "Components/SpotLightComponent.h"
#include "Materials/MaterialParameterCollection.h"
#include "Materials/MaterialParameterCollectionInstance.h"
#include "Lantern.generated.h"

UCLASS()
class WHISPERSOFTHEDJINN_API ALantern : public AActor
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	ALantern();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

protected:

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	UStaticMeshComponent* LanternMesh;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	UPointLightComponent* OriginLight;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	USpotLightComponent* ShineLight;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Materials")
	UMaterialParameterCollection* LanternMPC;
};
