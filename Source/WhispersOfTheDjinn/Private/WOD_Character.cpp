// Copyright Epic Games, Inc. All Rights Reserved.

#include "WOD_Character.h"
#include "WOD_PlayerState.h"
#include "WOD_PlayerController.h"
#include "Pickable.h"
#include "BaseAnimInstance.h"
#include "Engine/LocalPlayer.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "GameFramework/Controller.h"
#include "EnhancedInputComponent.h"
#include "Net/UnrealNetwork.h"

AWOD_Character::AWOD_Character()
{
	// Set size for collision capsule
	GetCapsuleComponent()->InitCapsuleSize(42.f, 96.0f);
	GetCapsuleComponent()->bHiddenInGame = false;
	GetCapsuleComponent()->SetHiddenInGame(false);

	// Don't rotate when the controller rotates. Let that just affect the camera.
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;

	// Configure character movement
	GetCharacterMovement()->bOrientRotationToMovement = true;
	GetCharacterMovement()->RotationRate = FRotator(0.0f, 500.0f, 0.0f);

	// Note: For faster iteration times these variables, and many more, can be tweaked in the Character Blueprint
	// instead of recompiling to adjust them
	GetCharacterMovement()->JumpZVelocity = 500.f;
	GetCharacterMovement()->AirControl = 0.35f;
	GetCharacterMovement()->MinAnalogWalkSpeed = 20.f;
	GetCharacterMovement()->BrakingDecelerationWalking = 2000.f;
	GetCharacterMovement()->BrakingDecelerationFalling = 1500.0f;

	// Create a camera boom (pulls in towards the player if there is a collision)
	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(RootComponent);
	CameraBoom->TargetArmLength = 700.0f;
	CameraBoom->bUsePawnControlRotation = true;

	// Create a follow camera
	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName);
	FollowCamera->bUsePawnControlRotation = false;

	SkeletalMesh = GetMesh();

	PrimaryActorTick.bCanEverTick = true;
	GetCharacterMovement()->NavAgentProps.bCanCrouch = true;

	bReplicates = true;
	bAlwaysRelevant = true;
}

void AWOD_Character::ServerSetAnimState_Implementation(EAnimState NewState)
{
	AnimState = NewState;
}

void AWOD_Character::ToggleCrouch(const FInputActionValue& Value)
{
	if (bIsCrouched)
	{
		UnCrouch();
	}
	else
	{
		Crouch();
	}

	UE_LOG(LogTemp, Warning, TEXT("bIsCrouched after: %d"), bIsCrouched);
}

void AWOD_Character::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AWOD_Character, bIsGrounded);
	DOREPLIFETIME(AWOD_Character, AnimState);
	DOREPLIFETIME(AWOD_Character, HeldItem);
	DOREPLIFETIME(AWOD_Character, CurrentInteractable);
	DOREPLIFETIME(AWOD_Character, bIsCarrying);
	DOREPLIFETIME(AWOD_Character, CanCarry);
}

void AWOD_Character::BeginPlay()
{
	Super::BeginPlay();

}

void AWOD_Character::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AWOD_Character::Landed(const FHitResult& Hit)
{
	Super::Landed(Hit);

	bIsGrounded = true;
}

void AWOD_Character::OnRep_PlayerState()
{
	Super::OnRep_PlayerState();

	if (AWOD_PlayerState* PS = GetPlayerState<AWOD_PlayerState>())
	{
		AssignPlayerRoles();
	}
}

void AWOD_Character::PossessedBy(AController* NewController)
{
	Super::PossessedBy(NewController);

	if (AWOD_PlayerState* PS = GetPlayerState<AWOD_PlayerState>())
	{
		AssignPlayerRoles();
	}

}

void AWOD_Character::AssignPlayerRoles()
{
	if (AWOD_PlayerState* PS = GetPlayerState<AWOD_PlayerState>())
	{
		if (PS->GetPlayerRole() == EPlayerRole::Sister)
		{
			AssignLantern();
			SkeletalMesh->SetSkeletalMesh(SisterSkeletalMesh);
			SkeletalMesh->SetAnimInstanceClass(SisterAnimInstance);
			GetCharacterMovement()->MaxWalkSpeed = 450.0f;
		}
		else
		{
			SkeletalMesh->SetSkeletalMesh(BrotherSkeletalMesh);
			SkeletalMesh->SetAnimInstanceClass(BrotherAnimInstance);
			GetCharacterMovement()->MaxWalkSpeed = 550.0f;
		}

		
	}
}

void AWOD_Character::AssignLantern()
{
	Lantern = GetWorld()->SpawnActor<ALantern>();

	if (Lantern)
	{
		Lantern->AttachToComponent(
			GetMesh(),
			FAttachmentTransformRules::SnapToTargetNotIncludingScale,
			TEXT("LanternSocket")); //Attach Lantern to player hand
	}
}

void AWOD_Character::Interact(const FInputActionValue& Value)
{
	AWOD_PlayerState* PS = GetPlayerState<AWOD_PlayerState>();
	if (PS->GetPlayerRole() == EPlayerRole::Brother)
	{
		RunPickupAction();
	}
}

void AWOD_Character::OnRep_HeldItem()
{
	ApplyHeldItem();
}

void AWOD_Character::ApplyHeldItem()
{
	if (HeldItem && HeldItem->Implements<UPickable>())
	{
		if (bIsCarrying)
		{
			IPickable::Execute_AttachToHand(HeldItem, this);
		}
		else
		{
			IPickable::Execute_DropToGround(HeldItem, this);
		}
	}
}

void AWOD_Character::ServerSetHeldItem_Implementation(AActor* newItem)
{
	HeldItem = newItem;

	bIsCarrying = true;
	CanCarry = !bIsCarrying;
	ApplyHeldItem();
	CurrentInteractable = nullptr;
}

void AWOD_Character::ServerDropHeldItem_Implementation()
{
	bIsCarrying = false;
	CanCarry = !bIsCarrying;
	ApplyHeldItem();
	HeldItem = nullptr;
}

void AWOD_Character::ServerSetCurrentInteractable_Implementation(AActor* newCurrent)
{
	CurrentInteractable = newCurrent;
}

void AWOD_Character::RunPickupAction()
{
	if (CurrentInteractable)
	{
		if (CanCarry)
			ServerSetHeldItem(CurrentInteractable);
		else
			ServerDropHeldItem();
	}
	else if (!CurrentInteractable)
	{
		if (CanCarry)
			UE_LOG(LogTemp, Log, TEXT("No Pickable Item Found At Location !***"))
		else
			ServerDropHeldItem();
	}
}

void AWOD_Character::Move(const FInputActionValue& Value)
{
	if (Controller != nullptr)
	{
		ServerSetAnimState(EAnimState::Walking);

		// input is a Vector2D
		FVector2D MovementVector = Value.Get<FVector2D>();

		FVector ForwardDir = FVector::ForwardVector;
		FVector RightDir = FVector::RightVector;

		AddMovementInput(ForwardDir, MovementVector.Y);
		AddMovementInput(RightDir, MovementVector.X);

	}
}

void AWOD_Character::StopMove(const FInputActionValue& Value)
{
	if (Controller != nullptr)
	{
		ServerSetAnimState(EAnimState::Idle);
	}
}

void AWOD_Character::StartJump(const FInputActionValue& Value)
{
	ServerSetAnimState(EAnimState::Jumping);

	// make sure the character stop crouching if he was
	if (bIsCrouched)
	{
		UnCrouch();
	}

	Jump();
	bIsGrounded = false;
}

void AWOD_Character::StopJump(const FInputActionValue& Value)
{
	StopJumping();
}

