// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Pushable.h"
#include "PushableBox.generated.h"

UCLASS()
class WHISPERSOFTHEDJINN_API APushableBox : public AActor, public IPushable
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	APushableBox();

	virtual void OnStartPush_Implementation(AWOD_Character* Pusher) override;
	virtual void OnStopPush_Implementation() override;

	virtual bool CanBePushed() const override;
	virtual bool IsBeingPushed() const override;
	virtual float GetWeight() const override;
	virtual USceneComponent* GetSnapPoint_Implementation() const override;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UFUNCTION()
	void OnOverlapBegin(class UPrimitiveComponent* OverlappedComp, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
	
	UFUNCTION()
	void OnOverlapEnd(class UPrimitiveComponent* OverlappedComp, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UFUNCTION()
	UStaticMeshComponent* GetBoxMesh() const { return BoxMesh; }

protected:

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	class UStaticMeshComponent* BoxMesh;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	class UBoxComponent* BoxOverlap;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	class USceneComponent* PushPointFront;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	class TArray<AWOD_Character*> Pushers;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Pushable")
	bool bCanBePushed = true;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Pushable")
	bool bIsBeingPushed = false;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Pouch")
	float Weight = 25.0f;

	int RequiredPushers = 1;

};
