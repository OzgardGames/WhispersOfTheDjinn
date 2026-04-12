// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Interactable.h"
#include "ButtonActor.generated.h"

UCLASS()
class WHISPERSOFTHEDJINN_API AButtonActor : public AActor, public IInteractable
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AButtonActor();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Implement the Interact method from IInteractable interface
	virtual void Interact_Implementation(AActor* InteractingActor) override;

	// Implement the GetSnapTransform method from IInteractable interface
	virtual FTransform GetSnapTransform_Implementation() override;
	void UpdateButtonMaterial();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Mesh")
	UStaticMeshComponent* ButtonMesh;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Material")
	UMaterialInterface* ButtonMaterial;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Material")
	UMaterialInterface* ButtonPressedMaterial;

	UPROPERTY(ReplicatedUsing = OnRep_IsPressed)
	bool bIsPressed = false;

	UFUNCTION()
	void OnRep_IsPressed();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Button")
	FString InteractionText = "Press Button";

protected:

	void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const;

};
