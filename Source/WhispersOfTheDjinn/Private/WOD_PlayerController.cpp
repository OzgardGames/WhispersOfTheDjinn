// Fill out your copyright notice in the Description page of Project Settings.


#include "WOD_PlayerController.h"
#include "Lantern.h"
#include "WOD_Character.h"
#include "WOD_PlayerState.h"
#include "BaseAnimInstance.h"
#include "InteractionComponent.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/PlayerStart.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "InputActionValue.h"
#include "InputAction.h"

AWOD_PlayerController::AWOD_PlayerController()
{
    InteractionComponent = CreateDefaultSubobject<UInteractionComponent>(TEXT("Interaction Component"));

    bReplicates = true;
    bAlwaysRelevant = true;
}

void AWOD_PlayerController::BeginPlay()
{
    Super::BeginPlay();
}

void AWOD_PlayerController::OnPossess(APawn* InPawn)
{
    Super::OnPossess(InPawn);
    
    ApplyLocalInputMapping();
    BindActionToController();
}

void AWOD_PlayerController::OnRep_PlayerState()
{
    ApplyLocalInputMapping();
    BindActionToController();
}

void AWOD_PlayerController::ApplyLocalInputMapping()
{

    if (!IsLocalController()) return;

    ULocalPlayer* LocalPlayer = GetLocalPlayer();

    if (!LocalPlayer) return;

    UEnhancedInputLocalPlayerSubsystem* Subsystem = LocalPlayer->GetSubsystem<UEnhancedInputLocalPlayerSubsystem>();
    if (!Subsystem) return;

    Subsystem->ClearAllMappings();

    AWOD_PlayerState* PS = LocalPlayer->GetPlayerController(GetWorld())->GetPlayerState<AWOD_PlayerState>();
    if (!PS) return;

    if (PS->GetCoopRole() == ECoopRole::PlayerOne)
    {
        Subsystem->AddMappingContext(IMC_PlayerOne, MappingPriority);
        UE_LOG(LogTemp, Log, TEXT("Player 1 Received Mapping system %s."),*IMC_PlayerOne->GetName());
    }
    else if (PS->GetCoopRole() == ECoopRole::PlayerTwo)
    {
        Subsystem->AddMappingContext(IMC_PlayerTwo, MappingPriority);
        UE_LOG(LogTemp, Log, TEXT("Player 2 Received Mapping system %s."), *IMC_PlayerTwo->GetName());
    }
}

void AWOD_PlayerController::BindActionToController()
{
    // Set up action bindings
    if (UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(InputComponent)) {

        // Moving
        EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, Cast<AWOD_Character>(GetPawn()), &AWOD_Character::Move);
        EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Completed, Cast<AWOD_Character>(GetPawn()), &AWOD_Character::StopMove);

        // Jumping
        EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Started, Cast<AWOD_Character>(GetPawn()), &AWOD_Character::StartJump);
        EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Completed, Cast<AWOD_Character>(GetPawn()), &AWOD_Character::StopJump);

        // Crouching
        EnhancedInputComponent->BindAction(CrouchAction, ETriggerEvent::Started, Cast<AWOD_Character>(GetPawn()), &AWOD_Character::ToggleCrouch);

        EnhancedInputComponent->BindAction(InteractAction, ETriggerEvent::Started, Cast<AWOD_Character>(GetPawn()), &AWOD_Character::Interact);
    }
}




