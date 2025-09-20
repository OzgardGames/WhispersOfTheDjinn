// Copyright Epic Games, Inc. All Rights Reserved.

#include "WOD_Character.h"
#include "WOD_PlayerState.h"
#include "WOD_PlayerController.h"
#include "Pickable.h"
#include "Pushable.h"
#include "PushableBox.h"
#include "BaseAnimInstance.h"
#include "Engine/LocalPlayer.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "GameFramework/Controller.h"
#include "EnhancedInputComponent.h"
#include "Kismet/KismetMathLibrary.h"
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



void AWOD_Character::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AWOD_Character, bIsGrounded);
	DOREPLIFETIME(AWOD_Character, AnimState);
	DOREPLIFETIME(AWOD_Character, MovementVector);
	DOREPLIFETIME(AWOD_Character, CurrentPickable);
	DOREPLIFETIME(AWOD_Character, CurrentPushable);
	DOREPLIFETIME(AWOD_Character, HeldItem);
	DOREPLIFETIME(AWOD_Character, PushedItem);
	DOREPLIFETIME(AWOD_Character, bIsCarrying);
	DOREPLIFETIME(AWOD_Character, bIsPushing);
	DOREPLIFETIME(AWOD_Character, bIsHanging);
	DOREPLIFETIME(AWOD_Character, RepPushableLocation);
}

void AWOD_Character::BeginPlay()
{
	Super::BeginPlay();

}

void AWOD_Character::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (PushedItem && !HasAuthority())
	{
		PushedItem->SetActorLocation(RepPushableLocation);
	}

	SendForwardTrace();

	if (bIsHanging)
	{
		SetActorRotation(UKismetMathLibrary::MakeRotFromX(-WallNormal));
		if (!FMath::IsNearlyZero(MovementVector.X))
			SetActorLocation(GetActorLocation() + (NewRight * MovementVector.X * 50.0f * DeltaTime));
		else
			GetCharacterMovement()->StopMovementImmediately();
	}

	//if (bIsHanging)
	//{
	//	FVector NewLocation = LedgePoint + WallNormal * 70.0f;
	//	float ZRelocate = NewLocation.Z - 75.0f;
	//	NewLocation.Z = ZRelocate;

	//	if (AnimState != EAnimState::Climbing)
	//	{
	//		FVector HangLocation = FMath::VInterpTo(GetActorLocation(), NewLocation, DeltaTime, 30.0f);
	//		SetActorLocation(HangLocation);
	//	}
	//}

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

void AWOD_Character::Server_SetCurrentPickable_Implementation(AActor* newCurrent)
{
	CurrentPickable = newCurrent;
}

void AWOD_Character::Server_SetCurrentPushable_Implementation(AActor* newCurrent)
{
	CurrentPushable = newCurrent;
}

void AWOD_Character::Interact(const FInputActionValue& Value)
{
	AWOD_PlayerState* PS = GetPlayerState<AWOD_PlayerState>();
	if (PS->GetPlayerRole() == EPlayerRole::Brother)
	{
		if (!HeldItem && !PushedItem) // Not Holding anything
		{
			if (CurrentPickable)
			{
				Server_PickupItem(CurrentPickable);
			}
			else if (CurrentPushable)
			{
				Server_StartPushing(CurrentPushable);
			}
		}
		else // Holding something
		{
			if (HeldItem && HeldItem->Implements<UPickable>())
			{
				FVector DropPosition = HandleDropPosition();
				Server_DropItem(DropPosition, FRotator::ZeroRotator);
			}
			else if (PushedItem && PushedItem->Implements<UPushable>())
			{
				Server_StopPushing();
			}
		}
	}
}

void AWOD_Character::Server_PickupItem_Implementation(AActor* newPickable)
{
	HeldItem = newPickable;

	if (HeldItem->Implements<UPickable>())
	{
		bIsCarrying = true;
		CurrentPickable = nullptr;

		if (HeldItem->Implements<UPickable>())
		{
			IPickable::Execute_OnPickedUp(HeldItem);
		}

		FAttachmentTransformRules AttachRules(EAttachmentRule::SnapToTarget, true);
		HeldItem->AttachToComponent(
			GetMesh(),
			AttachRules,
			FName("HandGrip_R")
		);
	}
}

void AWOD_Character::Server_DropItem_Implementation(FVector DropLocation, FRotator DropRotation)
{
	if (HeldItem->Implements<UPickable>())
	{
		IPickable::Execute_OnDropped(HeldItem);
	}

	bIsCarrying = false;
	HeldItem->DetachFromActor(FDetachmentTransformRules::KeepWorldTransform);
	HeldItem->SetActorLocationAndRotation(DropLocation, DropRotation);

	HeldItem = nullptr;
}

void AWOD_Character::Server_MovePushable_Implementation()
{
	if (PushedItem)
	{
		FVector Forward = GetActorForwardVector();
		FVector Right = GetActorRightVector();

		FVector MoveDir = (Forward * MovementVector.X - Right * MovementVector.Y).GetSafeNormal();

		if (!MoveDir.IsNearlyZero())
		{
			float Speed = 200.f; // tweak for "weighty" feel
			FVector NewLocation = PushedItem->GetActorLocation() + MoveDir * Speed * GetWorld()->GetDeltaSeconds();
			PushedItem->SetActorLocation(NewLocation);

			RepPushableLocation = NewLocation;
		}
	}
}

void AWOD_Character::OnRep_HeldItem()
{
	if (HeldItem)
	{
		if (HeldItem->Implements<UPickable>())
		{
			IPickable::Execute_OnPickedUp(HeldItem);
		}

		FAttachmentTransformRules AttachRules(EAttachmentRule::SnapToTarget, true);
		HeldItem->AttachToComponent(
			GetMesh(),
			AttachRules,
			FName("HandGrip_R")
		);
	}
}

void AWOD_Character::SendForwardTrace()
{

	FVector Start = GetActorLocation();
	FVector End = Start + (GetActorForwardVector() * 100.0f);

	FHitResult Hit;
	FCollisionQueryParams Params;
	Params.AddIgnoredActor(this);

	bool bHit = GetWorld()->LineTraceSingleByChannel(Hit, Start, End, ECC_Visibility, Params);

	if (bHit)
	{
		DrawDebugLine(GetWorld(), Start, End, FColor::Green, false);
		WallNormal = Hit.ImpactNormal;
		WallPoint = Hit.ImpactPoint;
		SendLedgeTrace(WallPoint);

		if (GetCharacterMovement()->Velocity.Z < 0.0f && CanHang)
		{
			if (FVector::Distance(WallPoint, LedgePoint) <= 80.0f)
			{
				ServerSetAnimState(EAnimState::Hanging);
				bIsHanging = true;

				GetCharacterMovement()->StopMovementImmediately();
				GetCharacterMovement()->SetMovementMode(MOVE_Custom);
				
				
			}
		}
	}
	else
	{
		DrawDebugLine(GetWorld(), Start, End, FColor::Red, false);
	}

}

void AWOD_Character::SendLedgeTrace(FVector HitPoint)
{
	float CapsuleRadius = 15.0f;
	float CapsuleHalfSize = 350.0f;
	FVector Offset = -WallNormal;
	
	FVector Start = Offset + FVector(HitPoint.X, HitPoint.Y, HitPoint.Z + CapsuleHalfSize);
	FVector End = Offset + HitPoint;

	FHitResult Hit;
	FCollisionQueryParams Params;
	Params.AddIgnoredActor(this);

	bool bHit = GetWorld()->SweepSingleByChannel(Hit, Start, End, FQuat::Identity, ECC_Visibility, FCollisionShape::MakeCapsule(CapsuleRadius, CapsuleHalfSize), Params);
	
	if (bHit)
	{
		DrawDebugCapsule(GetWorld(), End + FVector(0,0, CapsuleHalfSize / 2), CapsuleHalfSize / 2, CapsuleRadius, FQuat::Identity, FColor::Green, false);
		LedgePoint = Hit.ImpactPoint;
		DrawDebugPoint(GetWorld(), Start, 10.0f, FColor::Blue, false);
		DrawDebugPoint(GetWorld(), End, 10.0f, FColor::Blue, false);
		DrawDebugPoint(GetWorld(), LedgePoint, 10.0f, FColor::Blue, false);
		CreateLedgeNewTransform();
		LocateHandsOnLedge(LedgePoint);
		SendHandTrace(RightHandLocation);
		SendHandTrace(LeftHandLocation);

	}
	else
	{
		DrawDebugCapsule(GetWorld(), Start,CapsuleHalfSize/2,CapsuleRadius,FQuat::Identity, FColor::Red, false);
	}
}
void AWOD_Character::CreateLedgeNewTransform()
{
	float LineLength = 100.0f;
	FVector NewForward = WallNormal * -1;
	NewRight = FVector::CrossProduct(GetActorUpVector(), NewForward);

	FVector NewUp = FVector::CrossProduct(NewForward, NewRight);

	DrawDebugLine(GetWorld(), WallPoint, WallPoint + (NewRight * LineLength), FColor::Yellow, false);
	DrawDebugLine(GetWorld(), WallPoint, WallPoint + (NewUp * LineLength), FColor::Cyan, false);
}
void AWOD_Character::LocateHandsOnLedge(FVector HitPoint)
{
	float HandOffset = 40.0f;
	RightHandLocation = HitPoint + NewRight * HandOffset;
	LeftHandLocation = HitPoint + -NewRight * HandOffset;

	DrawDebugPoint(GetWorld(), RightHandLocation, 10.0f, FColor::Emerald, false);
	DrawDebugPoint(GetWorld(), LeftHandLocation, 10.0f, FColor::Magenta, false);
}

void AWOD_Character::SendHandTrace(FVector HandLocation)
{
	float CapsuleRadius = 15.0f;
	float CapsuleHalfSize = 50.0f;

	FVector Start = HandLocation;
	FVector End = HandLocation + (-WallNormal * CapsuleHalfSize);

	FHitResult Hit;
	FCollisionQueryParams Params;
	Params.AddIgnoredActor(this);

	bool bHit = GetWorld()->SweepSingleByChannel(Hit, Start, End, FQuat::Identity, ECC_Visibility, FCollisionShape::MakeCapsule(CapsuleRadius, CapsuleHalfSize), Params);
	DrawDebugPoint(GetWorld(), End, 10.0f, FColor::Orange, false);
	
	// Pick debug color
	FColor Color = bHit ? FColor::Green : FColor::Red;

	// Draw impact point & normal
	if (bHit)
	{
		DrawDebugLine(GetWorld(), Start, End, Color, false);
		DrawDebugPoint(GetWorld(), Hit.ImpactPoint, 10.0f,FColor::Magenta,false);
	}
	else
	{
		DrawDebugLine(GetWorld(), Start, End, Color, false);
	}
}

FVector AWOD_Character::HandleDropPosition()
{

	FVector Start = GetActorLocation();
	FVector End = Start - FVector(0, 0, 200.0f);
	float offsetFromPlayer = 100.0f;
	float PickupHeight = 25.0f;

	FHitResult Hit;
	FCollisionQueryParams Params;
	Params.AddIgnoredActor(this);

	FVector DropLocation = GetActorLocation();

	if (GetWorld()->LineTraceSingleByChannel(Hit, Start, End, ECC_Visibility, Params))
	{
		FVector ToFront = GetActorForwardVector() * offsetFromPlayer;
		DropLocation = Hit.ImpactPoint + FVector(0, 0, PickupHeight) + ToFront;
	}

	return DropLocation;

}

void AWOD_Character::Server_StartPushing_Implementation(AActor* newPushable)
{
	PushedItem = newPushable;

	if (PushedItem && PushedItem->Implements<UPushable>())
	{
		bIsPushing = true;
	
		RepPushableLocation = PushedItem->GetActorLocation();

		USceneComponent* SnapPoint = IPushable::Execute_GetSnapPoint(PushedItem);
		
		AttachToComponent(SnapPoint, FAttachmentTransformRules::SnapToTargetNotIncludingScale);

		GetCharacterMovement()->DisableMovement();
	}
}

void AWOD_Character::Server_StopPushing_Implementation()
{
	bIsPushing = false;
	DetachFromActor(FDetachmentTransformRules::KeepWorldTransform);

	GetCharacterMovement()->SetMovementMode(MOVE_Walking);
	PushedItem = nullptr;
}

void AWOD_Character::Server_SetMovementVector_Implementation(FVector2D NewMovementVector)
{
	MovementVector = NewMovementVector;
}

void AWOD_Character::OnRep_MovementVector()
{
	if (PushedItem)
	{
		FVector Forward = GetActorForwardVector();
		FVector Right = GetActorRightVector();

		FVector MoveDir = (Forward * MovementVector.X - Right * MovementVector.Y).GetSafeNormal();

		if (!MoveDir.IsNearlyZero())
		{
			float Speed = 200.f; // tweak for "weighty" feel
			FVector NewLocation = PushedItem->GetActorLocation() + MoveDir * Speed * GetWorld()->GetDeltaSeconds();
			PushedItem->SetActorLocation(NewLocation);
		}
	}
}

void AWOD_Character::Move(const FInputActionValue& Value)
{
	if (Controller != nullptr)
	{
		
		ServerSetAnimState(EAnimState::Walking);

		// input is a Vector2D
		Server_SetMovementVector(Value.Get<FVector2D>());

		if (bIsPushing && PushedItem)
		{
			Server_MovePushable();
		}
		else if (bIsHanging)
		{
			FVector DirAlongLedge = NewRight;
			AddMovementInput(DirAlongLedge, MovementVector.X);
		}
		else
		{
			// normal character move
			FVector ForwardDir = FVector::ForwardVector;
			FVector RightDir = FVector::RightVector;

			AddMovementInput(ForwardDir, MovementVector.Y);
			AddMovementInput(RightDir, MovementVector.X);
		}
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
	if (bIsHanging)
	{
		CanHang = false;
		ServerSetAnimState(EAnimState::Climbing);
		GetCharacterMovement()->SetMovementMode(MOVE_None);
		GetMesh()->GetAnimInstance()->Montage_Play(ClimbMontage);


		GetWorldTimerManager().ClearTimer(HangTimer);
		GetWorldTimerManager().SetTimer(HangTimer, this, &AWOD_Character::HangTimerSolver, 1.0f, false);
		return;
	}



	// make sure the character stop crouching if he was
	if (bIsCrouched)
	{
		UnCrouch();
	}

	ServerSetAnimState(EAnimState::Jumping);
	Jump();
	bIsGrounded = false;
}

void AWOD_Character::StopJump(const FInputActionValue& Value)
{
	StopJumping();
}

void AWOD_Character::ToggleCrouch(const FInputActionValue& Value)
{
	if (bIsHanging)
	{
		ServerSetAnimState(EAnimState::Idle);
		bIsHanging = false;
		CanHang = false;
		GetCharacterMovement()->SetMovementMode(MOVE_Walking);
		GetWorldTimerManager().ClearTimer(HangTimer);
		GetWorldTimerManager().SetTimer(HangTimer,this,&AWOD_Character::HangTimerSolver, 1.0f,false);
		return;
	}

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

void AWOD_Character::HangTimerSolver()
{
	if (!bIsHanging)
	{
		CanHang = true;

	}
	else
	{
		FVector RootLocation = GetActorLocation();
		RootLocation.Z += 192.0f;
		SetActorLocation(RootLocation + (-WallNormal * 50.0f));
		GetCharacterMovement()->SetMovementMode(MOVE_Walking);
		ServerSetAnimState(EAnimState::Idle);

		bIsHanging = false;
		CanHang = true;

	}
}

