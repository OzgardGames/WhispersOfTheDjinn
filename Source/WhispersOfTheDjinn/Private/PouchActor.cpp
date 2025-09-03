// Fill out your copyright notice in the Description page of Project Settings.


#include "PouchActor.h"
#include "WOD_Character.h"
#include "Net/UnrealNetwork.h"

// Sets default values
APouchActor::APouchActor()
{
	CollisionBox = CreateDefaultSubobject<UBoxComponent>(TEXT("CollisionBox"));
	RootComponent = CollisionBox;

	TriggerBox = CreateDefaultSubobject<UBoxComponent>(TEXT("TriggerBox"));
	TriggerBox->SetupAttachment(RootComponent);
	TriggerBox->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	TriggerBox->SetGenerateOverlapEvents(true);

	TriggerBox->OnComponentBeginOverlap.AddDynamic(this, &APouchActor::OnOverlapBegin);
	TriggerBox->OnComponentEndOverlap.AddDynamic(this, &APouchActor::OnOverlapEnd);

	PouchMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("PouchMesh"));
	PouchMesh->SetupAttachment(RootComponent);

 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	bReplicates = true;
	SetReplicateMovement(true);
}

// Called when the game starts or when spawned
void APouchActor::BeginPlay()
{
	Super::BeginPlay();
	
}

void APouchActor::OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (OtherActor && (OtherActor != this))
	{
		if (AWOD_Character* Character = Cast<AWOD_Character>(OtherActor))
		{
			Character->ServerSetCurrentInteractable(this);
		}

	}
}

void APouchActor::OnOverlapEnd(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	if (AWOD_Character* Character = Cast<AWOD_Character>(OtherActor))
	{
		Character->ServerSetCurrentInteractable(nullptr);
	}
}

// Called every frame
void APouchActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void APouchActor::AttachToHand_Implementation(AWOD_Character* PickingCharacter)
{
	if (AWOD_Character* Character = PickingCharacter)
	{
		bIsPickedUp = true;

		SetActorEnableCollision(false);
		PouchMesh->SetSimulatePhysics(false);
		PouchMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);

		FAttachmentTransformRules AttachRules(EAttachmentRule::SnapToTarget, true);
		AttachToComponent(
			Character->GetMesh(),
			AttachRules,
			FName("HandGrip_R")
		);

	}

}

void APouchActor::DropToGround_Implementation(AWOD_Character* PickingCharacter)
{
	bIsPickedUp = false;
	DetachFromActor(FDetachmentTransformRules::KeepWorldTransform);

	if (AWOD_Character* Character = PickingCharacter)
	{
		FVector Start = Character->GetActorLocation();
		FVector End = Start - FVector(0, 0, 200.0f);

		FHitResult Hit;
		FCollisionQueryParams Params;
		Params.AddIgnoredActor(Character);

		if (GetWorld()->LineTraceSingleByChannel(Hit, Start, End, ECC_Visibility, Params))
		{
			FVector GroundLocation = Hit.ImpactPoint + FVector(0, 0, 25.0f);
			FVector ToFront = Character->GetActorForwardVector() * 35.0f;
			SetActorLocation(GroundLocation + ToFront);
		}
		else
		{
			SetActorLocation(Character->GetActorLocation() - FVector(0, 0, 50.0f));
		}

		SetActorRotation(FRotator::ZeroRotator);

		SetActorEnableCollision(true);
		PouchMesh->SetSimulatePhysics(false);
		PouchMesh->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	}
}

