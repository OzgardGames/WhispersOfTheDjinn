// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Animation/AnimInstance.h"
#include "Logging/LogMacros.h"
#include "Lantern.h"
#include "InteractionComponent.h"
#include "WOD_Character.generated.h"

UENUM(BlueprintType)
enum class EAnimState : uint8
{
	Idle UMETA(DisplayName = "Idle"),
	Walking UMETA(DisplayName = "Walk"),
	Running UMETA(DisplayName = "Run"),
	Jumping UMETA(DisplayName = "Jump"),
	Crouching UMETA(DisplayName = "Crouch"),
	Carrying UMETA(DisplayName = "Carry"),
	Hanging UMETA(DisplayName = "Hang"),
	Climbing UMETA(DisplayName = "Climb"),
};

USTRUCT(BlueprintType)
struct FPlayerActionState
{
	GENERATED_BODY(BlueprintReadOnly)



	UPROPERTY(BlueprintReadOnly)
	EAnimState AnimState = EAnimState::Idle;
};

UCLASS()
class WHISPERSOFTHEDJINN_API AWOD_Character : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	AWOD_Character();

	UFUNCTION(Server, Reliable)
	void ServerSetAnimState(EAnimState NewState);

	UPROPERTY(Replicated, VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	EAnimState AnimState = EAnimState::Idle;
	
	UPROPERTY(Replicated, EditAnywhere, BlueprintReadOnly)
	bool bIsGrounded = true;

	UPROPERTY(Replicated, EditAnywhere, BlueprintReadOnly)
	bool bIsCarrying = false;

	UPROPERTY(Replicated, EditAnywhere, BlueprintReadOnly)
	bool bIsPushing = false;

	UPROPERTY(Replicated, EditAnywhere, BlueprintReadOnly)
	bool bIsSprinting = false;

	UPROPERTY(Replicated, EditAnywhere, BlueprintReadOnly)
	bool bIsHanging = false;


protected:

	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;
	virtual void Landed(const FHitResult& Hit) override;
	virtual void OnRep_PlayerState() override;
	virtual void PossessedBy(AController* NewController) override;

private:

	/** Camera boom positioning the camera behind the character */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class USpringArmComponent* CameraBoom;

	/** Follow camera */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class UCameraComponent* FollowCamera;

	/** Follow camera */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Asset", meta = (AllowPrivateAccess = "true"))

	USkeletalMeshComponent* SkeletalMesh;

	// Sister Character
	ALantern* Lantern;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Player Meshes", meta = (AllowPrivateAccess = "true"))
	USkeletalMesh* SisterSkeletalMesh;

	// Brother Character
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Player Meshes", meta = (AllowPrivateAccess = "true"))
	USkeletalMesh* BrotherSkeletalMesh;

	// Animation Instances
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Animation Instances", meta = (AllowPrivateAccess = "true"))
	TSubclassOf<UAnimInstance> SisterAnimInstance;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Animation Instances", meta = (AllowPrivateAccess = "true"))
	TSubclassOf<UAnimInstance> BrotherAnimInstance;

public:

	UPROPERTY(ReplicatedUsing = OnRep_MovementVector, VisibleAnywhere, BlueprintReadOnly)
	FVector2D MovementVector = FVector2D(0);

	UFUNCTION()
	void OnRep_MovementVector();

	UFUNCTION(Server,Reliable)
	void Server_SetMovementVector(FVector2D NewMovementVector);

	/** Handles movement input */
	UFUNCTION()
	void Move(const FInputActionValue& Value);

	UFUNCTION()
	void StopMove(const FInputActionValue& Value);

	/** Handles Jump input */
	UFUNCTION()
	void StartJump(const FInputActionValue& Value);
	UFUNCTION()
	void StopJump(const FInputActionValue& Value);

	/** Handles Crouch input */
	UFUNCTION()
	void ToggleCrouch(const FInputActionValue& Value);

	UFUNCTION()
	void AssignPlayerRoles();

	UFUNCTION()
	void AssignLantern();

	UFUNCTION()
	void Interact(const FInputActionValue& Value);

	// PICKUP NETWORK LOGIC *************
	UFUNCTION(Server,Reliable)
	void Server_SetCurrentPickable(AActor* newCurrent);

	UFUNCTION(Server,Reliable)
	void Server_PickupItem(AActor* newPickable);

	UFUNCTION()
	FVector HandleDropPosition();

	UFUNCTION(Server,Reliable)
	void Server_DropItem(FVector DropLocation,FRotator DropRotation);

	UPROPERTY(Replicated, VisibleAnywhere, BlueprintReadOnly, Category = "Interactable")
	AActor* CurrentPickable = nullptr;

	// PUSH/PULL NETWORK LOGIC ************

	UFUNCTION(Server, Reliable)
	void Server_SetCurrentPushable(AActor* newCurrent);

	UFUNCTION(Server , Reliable)
	void Server_StartPushing(AActor* newPushable);

	UFUNCTION(Server, Reliable)
	void Server_StopPushing();

	UFUNCTION(Server, Reliable)
	void Server_MovePushable();

	UPROPERTY(Replicated, VisibleAnywhere, BlueprintReadOnly, Category = "Interactable")
	FVector RepPushableLocation;

	UPROPERTY(Replicated, VisibleAnywhere, BlueprintReadOnly, Category = "Interactable")
	AActor* CurrentPushable = nullptr;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Interactable")
	float PushForce = 200.0f;

	float BoxInterpSpeed = 30.f;

	// INTERACTIBLE LOGIC ************

	UFUNCTION()
	void OnRep_HeldItem();

	UPROPERTY(ReplicatedUsing = OnRep_HeldItem)
	AActor* HeldItem = nullptr;

	UPROPERTY(Replicated)
	AActor* PushedItem = nullptr;

	// LEDGE GRABS LOGIX ************

	UFUNCTION()
	void SendForwardTrace();

	UFUNCTION()
	void SendLedgeTrace(FVector HitPoint);

	UFUNCTION()
	void CreateLedgeNewTransform();

	UFUNCTION()
	void LocateHandsOnLedge(FVector HitPoint);

	UFUNCTION()
	void SendHandTrace(FVector HandLocation);
	
	FVector WallNormal = FVector(0);
	FVector WallPoint = FVector(0);
	FVector LedgePoint = FVector(0);
	FVector NewRight = FVector(0);

	FVector RightHandLocation = FVector(0);
	FVector LeftHandLocation = FVector(0);

	bool CanHang = true;
	FTimerHandle HangTimer;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Montages")
	UAnimMontage* ClimbMontage;
	UFUNCTION()
	void HangTimerSolver();



public:

	/** Returns CameraBoom subobject **/
	FORCEINLINE class USpringArmComponent* GetCameraBoom() const { return CameraBoom; }

	/** Returns FollowCamera subobject **/
	FORCEINLINE class UCameraComponent* GetFollowCamera() const { return FollowCamera; }

protected:

	void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const;
};
