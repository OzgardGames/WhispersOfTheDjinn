// Copyright Epic Games, Inc. All Rights Reserved.

#include "WOD_Character.h"
#include "WOD_PlayerState.h"
#include "Engine/LocalPlayer.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "GameFramework/Controller.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "InputActionValue.h"

AWOD_Character::AWOD_Character()
{
	// Set size for collision capsule
	GetCapsuleComponent()->InitCapsuleSize(42.f, 96.0f);

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
	InteractionComponent = CreateDefaultSubobject<UInteractionComponent>(TEXT("Interaction Component"));

	
	PrimaryActorTick.bCanEverTick = true;
	GetCharacterMovement()->GetNavAgentPropertiesRef().bCanCrouch = true;

	bReplicates = true;
	bAlwaysRelevant = true;
}

void AWOD_Character::BeginPlay()
{
	Super::BeginPlay();

}

void AWOD_Character::Landed(const FHitResult& Hit)
{
	Super::Landed(Hit);

	bIsGrounded = true;
}

void AWOD_Character::OnRep_PlayerState()
{
	Super::OnRep_PlayerState();

	PS = GetPlayerState<AWOD_PlayerState>();

	if (PS)
	{
		AssignPlayerRoles();
	}
}

void AWOD_Character::PossessedBy(AController* NewController)
{
	Super::PossessedBy(NewController);

	PS = GetPlayerState<AWOD_PlayerState>();

	if (PS)
	{
		AssignPlayerRoles();

		AnimInstance = Cast<UBaseAnimInstance>(SkeletalMesh->GetAnimInstance());

		if (AnimInstance)
		{
			PS->OnPlayerStateChanged.AddDynamic(this, &AWOD_Character::HandlePlayerStateChanged);
			AnimInstance->AnimState = AnimInstance->MapPlayerStateToAnim(PS->GetState());
		}
	}

}

void AWOD_Character::HandlePlayerStateChanged(EPlayerState NewState)
{
	if (AnimInstance)
	{
		AnimInstance->AnimState = AnimInstance->MapPlayerStateToAnim(NewState);
	}
}

void AWOD_Character::AssignPlayerRoles()
{
	if (!PS) return;
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

void AWOD_Character::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	// Set up action bindings
	if (UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(PlayerInputComponent)) {

		// Moving
		EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &AWOD_Character::Move);

		// Jumping
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Started, this, &AWOD_Character::StartJump);
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Completed, this, &AWOD_Character::StopJump);

		// Crouching
		EnhancedInputComponent->BindAction(CrouchAction, ETriggerEvent::Started, this, &AWOD_Character::ToggleCrouch);
		
		EnhancedInputComponent->BindAction(InteractAction, ETriggerEvent::Started, InteractionComponent, &UInteractionComponent::TryInteract);
	}

}

void AWOD_Character::Move(const FInputActionValue& Value)
{
	if (Controller != nullptr)
	{
		if (!PS) return;
		if (bCanCrouch)
		{
			PS->SetState(EPlayerState::Walking);
		}

		// input is a Vector2D
		FVector2D MovementVector = Value.Get<FVector2D>();

		FVector ForwardDir = FVector::ForwardVector;
		FVector RightDir = FVector::RightVector;

		AddMovementInput(ForwardDir, MovementVector.Y);
		AddMovementInput(RightDir, MovementVector.X);

	}
}

void AWOD_Character::StartJump(const FInputActionValue& Value)
{
	if (!PS) return;
	PS->SetState(EPlayerState::Jumping);

	// make sure the character stop crouching if he was
	if (!bCanCrouch)
	{
		UncrouchStart();
	}

	Jump();
	bIsGrounded = false;
}

void AWOD_Character::StopJump(const FInputActionValue& Value)
{
	StopJumping();
}

void AWOD_Character::ToggleCrouch(const FInputActionValue& Value)
{
	if (!PS) return;

	if (bIsGrounded)
	{
		if (!bCanCrouch)
		{
			UncrouchStart();
		}
		else
		{
			CrouchStart();
		}
	}
}

void AWOD_Character::CrouchStart()
{
	PS->SetState(EPlayerState::Crouching);
	GetCapsuleComponent()->SetCapsuleSize(50.0f, 48.0f, true);
	GetCapsuleComponent()->SetRelativeLocation(GetCapsuleComponent()->GetRelativeLocation() - FVector(0.0f, 0.0f, 48.0f));
}

void AWOD_Character::UncrouchStart()
{
	PS->SetState(EPlayerState::Idle);
	GetCapsuleComponent()->SetCapsuleSize(42.0f, 96.0f, true);
	GetCapsuleComponent()->SetRelativeLocation(GetCapsuleComponent()->GetRelativeLocation() + FVector(0.0f, 0.0f, 48.0f));
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
