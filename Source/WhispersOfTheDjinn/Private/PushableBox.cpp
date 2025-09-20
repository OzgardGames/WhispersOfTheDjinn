// Fill out your copyright notice in the Description page of Project Settings.


#include "PushableBox.h"
#include "WOD_Character.h"
#include "Components/BoxComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Net/UnrealNetwork.h"

// Sets default values
APushableBox::APushableBox()
{
	BoxMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("BoxMesh"));
	RootComponent = BoxMesh;
	BoxMesh->SetSimulatePhysics(true);
	BoxMesh->SetEnableGravity(true);
	BoxMesh->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	BoxMesh->SetCollisionObjectType(ECC_PhysicsBody);
	BoxMesh->SetCollisionResponseToAllChannels(ECR_Block);
	BoxMesh->SetCollisionResponseToChannel(ECC_Pawn, ECR_Ignore);

	BoxMesh->SetLinearDamping(2.f);   // slows box gradually
	BoxMesh->SetAngularDamping(5.f);  // prevents spinning

	BoxOverlap = CreateDefaultSubobject<UBoxComponent>(TEXT("BoxOverlap"));
	BoxOverlap->SetupAttachment(RootComponent);
	BoxOverlap->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	BoxOverlap->SetGenerateOverlapEvents(true);

	BoxOverlap->OnComponentBeginOverlap.AddDynamic(this, &APushableBox::OnOverlapBegin);
	BoxOverlap->OnComponentEndOverlap.AddDynamic(this, &APushableBox::OnOverlapEnd);

	PushPointFront = CreateDefaultSubobject<USceneComponent>(TEXT("PushPointFront"));
	PushPointFront->SetupAttachment(RootComponent);

 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	UE_LOG(LogTemp, Warning, TEXT("Box mass = %f"), BoxMesh->GetMass());

	bReplicates = true;
	SetReplicateMovement(true);

}

bool APushableBox::CanBePushed() const
{
	return bCanBePushed;
}

bool APushableBox::IsBeingPushed() const
{
	return bIsBeingPushed;
}

float APushableBox::GetWeight() const
{
	return Weight;
}

void APushableBox::OnStartPush_Implementation(AWOD_Character* Pusher)
{
	Pushers.Add(Pusher);

	if (Pushers.Num() >= RequiredPushers)
	{
		bCanBePushed = true;
	}
	BoxMesh->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Ignore);
	BoxOverlap->SetGenerateOverlapEvents(false);
}

void APushableBox::OnStopPush_Implementation()
{
	bCanBePushed = false;
	Pushers.Empty();
	BoxMesh->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Block);
	BoxOverlap->SetGenerateOverlapEvents(true);
}
USceneComponent* APushableBox::GetSnapPoint_Implementation() const
{
	return PushPointFront;
}

// Called when the game starts or when spawned
void APushableBox::BeginPlay()
{
	Super::BeginPlay();
	
}

void APushableBox::OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (OtherActor && (OtherActor != this))
	{
		if (AWOD_Character* Character = Cast<AWOD_Character>(OtherActor))
		{
			Character->Server_SetCurrentPushable(this);
		}
	}
}

void APushableBox::OnOverlapEnd(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	if (AWOD_Character* Character = Cast<AWOD_Character>(OtherActor))
	{
		Character->Server_SetCurrentPushable(nullptr);
	}
}

// Called every frame
void APushableBox::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

