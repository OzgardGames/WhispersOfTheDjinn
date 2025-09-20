// Fill out your copyright notice in the Description page of Project Settings.


#include "PouchActor.h"
#include "WOD_Character.h"
#include "Net/UnrealNetwork.h"

// Sets default values
APouchActor::APouchActor()
{
	PouchMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("PouchMesh"));
	RootComponent = RootComponent;
	PouchMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	TriggerBox = CreateDefaultSubobject<UBoxComponent>(TEXT("TriggerBox"));
	TriggerBox->SetupAttachment(RootComponent);
	TriggerBox->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	TriggerBox->SetGenerateOverlapEvents(true);

	TriggerBox->OnComponentBeginOverlap.AddDynamic(this, &APouchActor::OnOverlapBegin);
	TriggerBox->OnComponentEndOverlap.AddDynamic(this, &APouchActor::OnOverlapEnd);

	CollisionBox = CreateDefaultSubobject<UBoxComponent>(TEXT("CollisionBox"));
	CollisionBox->SetupAttachment(RootComponent);
	CollisionBox->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	CollisionBox->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Block);
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

void APouchActor::OnPickedUp_Implementation()
{
	bIsHeld = true;
	bCanBePickedUp = false;

	SetActorEnableCollision(false);
}

void APouchActor::OnDropped_Implementation()
{
	bIsHeld = false;
	bCanBePickedUp = true;

	SetActorEnableCollision(true);
}

bool APouchActor::CanBePickedUp() const
{
	return bCanBePickedUp;
}

bool APouchActor::IsHeld() const
{
	return bIsHeld;
}

float APouchActor::GetWeight() const
{
	return Weight;
}

void APouchActor::OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (OtherActor && (OtherActor != this))
	{
		if (AWOD_Character* Character = Cast<AWOD_Character>(OtherActor))
		{
			Character->Server_SetCurrentPickable(this);
		}

	}
}

void APouchActor::OnOverlapEnd(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	if (AWOD_Character* Character = Cast<AWOD_Character>(OtherActor))
	{
		Character->Server_SetCurrentPickable(nullptr);
	}
}
// Called every frame
void APouchActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}
