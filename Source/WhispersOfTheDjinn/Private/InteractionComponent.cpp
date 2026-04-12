// Fill out your copyright notice in the Description page of Project Settings.


#include "InteractionComponent.h"
#include "Interactable.h"
#include <WOD_Character.h>

// Sets default values for this component's properties
UInteractionComponent::UInteractionComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}


// Called when the game starts
void UInteractionComponent::BeginPlay()
{
	Super::BeginPlay();
}

void UInteractionComponent::TryInteract()
{
	AWOD_Character* Owner = Cast<AWOD_Character>(GetOwner());
	if (!Owner) return;

	FVector Start = Owner->GetActorLocation();
	FVector End = Start + Owner->GetActorForwardVector() * InteractionDistance;

	FHitResult HitResult;
	FCollisionQueryParams CollisionParams;
	CollisionParams.AddIgnoredActor(Owner);

	bool bHit = GetWorld()->LineTraceSingleByChannel(
		HitResult,
		Start,
		End,
		ECC_Visibility,
		CollisionParams
	);

	if (bHit && HitResult.GetActor() && HitResult.GetActor()->Implements<UInteractable>())
	{
		Owner->Server_TryInteract(HitResult.GetActor());
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("No interactable actor found within range."));
	}

#if WITH_EDITOR
	DrawDebugLine(GetWorld(), Start, End, FColor::Green, false, 2.0f);
#endif
}

