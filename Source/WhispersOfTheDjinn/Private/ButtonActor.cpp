// Fill out your copyright notice in the Description page of Project Settings.


#include "ButtonActor.h"

// Sets default values
AButtonActor::AButtonActor()
{

 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	ButtonMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("ButtonMesh"));
	RootComponent = ButtonMesh;
}

// Called when the game starts or when spawned
void AButtonActor::BeginPlay()
{
	Super::BeginPlay();
	
	if (ButtonMaterial)
	{
		ButtonMesh->SetMaterial(0, ButtonMaterial);
	}

}

// Called every frame
void AButtonActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void AButtonActor::Interact_Implementation(AActor* InteractingActor)
{
	// Toggle the button state
	bIsPressed = !bIsPressed;

	// Log the interaction
	UE_LOG(LogTemp, Warning, TEXT("Button %s pressed by %s"), *GetName(), *InteractingActor->GetName());

	if (bIsPressed)
	{
		ButtonMesh->SetMaterial(0, ButtonPressedMaterial);
	}
	else
	{
		ButtonMesh->SetMaterial(0, ButtonMaterial);
	}

}

FTransform AButtonActor::GetSnapTransform_Implementation()
{
	return FTransform();
}

