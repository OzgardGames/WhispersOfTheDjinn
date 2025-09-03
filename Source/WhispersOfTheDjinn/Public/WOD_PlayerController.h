#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "EnhancedInputSubsystems.h"
#include "InputMappingContext.h"
#include "WOD_PlayerController.generated.h"

UCLASS()
class WHISPERSOFTHEDJINN_API AWOD_PlayerController : public APlayerController
{
    GENERATED_BODY()

public:
    AWOD_PlayerController();
    virtual void BeginPlay() override;

    virtual void OnPossess(APawn* InPawn) override;
    virtual void OnRep_PlayerState() override;

    UFUNCTION()
    void ApplyLocalInputMapping();
    void BindActionToController() ;

protected:
    /** Input Mapping Context assigned in editor */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
    UInputMappingContext* IMC_PlayerOne;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Input")
    UInputMappingContext* IMC_PlayerTwo;

    /** Priority of the mapping */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
    int32 MappingPriority = 0;

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

    class UInteractionComponent* InteractionComponent;
};