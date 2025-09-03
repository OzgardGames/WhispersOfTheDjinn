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
	bool CanCarry = true;

	UPROPERTY(Replicated, EditAnywhere, BlueprintReadOnly)
	bool bIsSprinting = false;


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

	// PICKUP NETWORK LOGIC
	UFUNCTION(Server, Reliable)
	void ServerSetHeldItem(AActor* newItem);

	UFUNCTION(Server,Reliable)
	void ServerSetCurrentInteractable(AActor* newCurrent);

	UFUNCTION(Server, Reliable)
	void ServerDropHeldItem();

	UFUNCTION()
	void RunPickupAction();

	UFUNCTION()
	void ApplyHeldItem();

	UFUNCTION()
	void OnRep_HeldItem();
	
	UPROPERTY(Replicated, VisibleAnywhere, BlueprintReadOnly, Category = "Interactable")
	AActor* CurrentInteractable = nullptr;

	UPROPERTY(ReplicatedUsing = OnRep_HeldItem)
	AActor* HeldItem = nullptr;

public:

	/** Returns CameraBoom subobject **/
	FORCEINLINE class USpringArmComponent* GetCameraBoom() const { return CameraBoom; }

	/** Returns FollowCamera subobject **/
	FORCEINLINE class UCameraComponent* GetFollowCamera() const { return FollowCamera; }

protected:

	void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const;
};
