// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Pickable.h"
#include "TimerManager.h"
#include "Components/BoxComponent.h"
#include "PouchActor.generated.h"

UCLASS()
class WHISPERSOFTHEDJINN_API APouchActor : public AActor, public IPickable
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	APouchActor();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UFUNCTION()
	void OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
	UFUNCTION()
	void OnOverlapEnd(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

	virtual void AttachToHand_Implementation(AWOD_Character* PickingCharacter) override;

	virtual void DropToGround_Implementation(AWOD_Character* PickingCharacter) override;

	public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Pouch")
	UStaticMeshComponent* PouchMesh;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Pouch")
	UMaterialInterface* PouchMaterial;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Pouch")
	UBoxComponent* TriggerBox;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Pouch")
	UBoxComponent* CollisionBox;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Pouch")
	bool bIsPickedUp = false;


};
