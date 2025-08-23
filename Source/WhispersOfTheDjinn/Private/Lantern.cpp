// Fill out your copyright notice in the Description page of Project Settings.


#include "Lantern.h"

// Sets default values
ALantern::ALantern()
{
	LanternMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Lantern"));
	LanternMesh->SetRelativeScale3D(FVector(0.2f, 0.2f, 0.2f));
	LanternMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	RootComponent = LanternMesh;

	// Find the lantern asset
	static ConstructorHelpers::FObjectFinder<UStaticMesh> MeshAsset(TEXT("/Game/WOD/Meshes/Lantern_Mesh.Lantern_Mesh"));
	if (MeshAsset.Succeeded())
	{
		LanternMesh->SetStaticMesh(MeshAsset.Object);
	}

	OriginLight = CreateDefaultSubobject<UPointLightComponent>(TEXT("OriginLight"));
	OriginLight->SetupAttachment(LanternMesh);
	OriginLight->SetIntensity(200.0f); // Set the intensity of the light

	ShineLight = CreateDefaultSubobject<USpotLightComponent>(TEXT("ShineLight"));
	ShineLight->SetupAttachment(LanternMesh);
	ShineLight->SetIntensity(1000.0f); // Set the intensity of the spotlight
	ShineLight->AttenuationRadius = 1500.0f; // Set the radius of the spotlight
	ShineLight->InnerConeAngle = 10.0f;
	ShineLight->OuterConeAngle = 15.0f;



 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void ALantern::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ALantern::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (LanternMPC)
	{
		UMaterialParameterCollectionInstance* MPCInstance = GetWorld()->GetParameterCollectionInstance(LanternMPC);
		if (MPCInstance)
		{
			MPCInstance->SetVectorParameterValue(TEXT("LightPos"), GetActorLocation());
			MPCInstance->SetVectorParameterValue(TEXT("LightDir"), GetActorForwardVector());
			MPCInstance->SetScalarParameterValue(TEXT("ConeAngle"), ShineLight->InnerConeAngle);
			MPCInstance->SetScalarParameterValue(TEXT("LightRange"), ShineLight->AttenuationRadius);
		}
	}
	

}

