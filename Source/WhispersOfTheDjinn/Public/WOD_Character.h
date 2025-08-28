// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Animation/AnimInstance.h"
#include "Logging/LogMacros.h"
#include "Lantern.h"
#include "InteractionComponent.h"
#include "BaseAnimInstance.h"
#include "WOD_Character.generated.h"

UCLASS()
class WHISPERSOFTHEDJINN_API AWOD_Character : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	AWOD_Character();
protected:

	virtual void BeginPlay() override;
	virtual void Landed(const FHitResult& Hit) override;
	virtual void OnRep_PlayerState() override;
	virtual void PossessedBy(AController* NewController) override;

	/** Initialize input action bindings */
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

private:

	/** Camera boom positioning the camera behind the character */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class USpringArmComponent* CameraBoom;

	/** Follow camera */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class UCameraComponent* FollowCamera;

	/** Follow camera */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Asset", meta = (AllowPrivateAccess = "true"))
	
	UInteractionComponent* InteractionComponent;
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


protected:

	/** Move Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
	class UInputAction* MoveAction;

	/** Jump Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
	class UInputAction* JumpAction;

	/** Jump Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
	class UInputAction* CrouchAction;

	/** Jump Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
	class UInputAction* InteractAction;

private:

	/** Handles movement input */
	UFUNCTION()
	void Move(const FInputActionValue& Value);

	/** Handles Jump input */
	UFUNCTION()
	void StartJump(const FInputActionValue& Value);
	UFUNCTION()
	void StopJump(const FInputActionValue& Value);

	/** Handles Crouch input */
	UFUNCTION()
	void ToggleCrouch(const FInputActionValue& Value);

	void CrouchStart();
	void UncrouchStart();

	UFUNCTION()
	void AssignPlayerRoles();

	class AWOD_PlayerState* PS = nullptr;
	UBaseAnimInstance* AnimInstance;

public:

	/** Returns CameraBoom subobject **/
	FORCEINLINE class USpringArmComponent* GetCameraBoom() const { return CameraBoom; }

	/** Returns FollowCamera subobject **/
	FORCEINLINE class UCameraComponent* GetFollowCamera() const { return FollowCamera; }

	UFUNCTION()
	void AssignLantern();

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Abilities")
	bool bIsCrouching = false;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Abilities")
	bool bIsGrounded = true;
};
