#pragma once

#include "BehaviorTree/BTTaskNode.h"
#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "BTTask_ActivateAbilityByTag.generated.h"

class UAbilitySystemComponent;

/**
 * Defines how the ability tag is chosen during task execution.
 */
UENUM()
enum class EAeonAbilityTagSource : uint8
{
    FromBlackboard UMETA(DisplayName = "From Blackboard"),
    FromProperty UMETA(DisplayName = "From Property")
};

/**
 * Behavior tree task that attempts to activate a random gameplay ability
 * on the controlled pawn using a tag, either from a property or a blackboard key.
 */
UCLASS()
class AEONAI_API UBTTask_ActivateAbilityByTag : public UBTTaskNode
{
    GENERATED_BODY()

public:
    UBTTask_ActivateAbilityByTag();

    virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& BehaviorTreeComponent, uint8* NodeMemory) override;
    virtual void InitializeFromAsset(UBehaviorTree& Asset) override;

    virtual FString GetStaticDescription() const override;

protected:
    FGameplayTag ResolveAbilityTag(UBehaviorTreeComponent& BehaviorTreeComponent);

    /** Determines whether to use the Blackboard or Property gameplay tag */
    UPROPERTY(EditAnywhere, Category = "Ability")
    EAeonAbilityTagSource AbilityTagSource{ EAeonAbilityTagSource::FromProperty };

    /** The gameplay tag used if AbilityTagSource is FromProperty */
    UPROPERTY(EditAnywhere,
              Category = "Ability",
              meta = (EditCondition = "AbilityTagSource == EAeonAbilityTagSource::FromProperty", EditConditionHides))
    FGameplayTag AbilityTag{ FGameplayTag::EmptyTag };

    /** The blackboard key that should return a gameplay tag */
    UPROPERTY(EditAnywhere,
              Category = "Blackboard",
              meta = (EditCondition = "AbilityTagSource == EAeonAbilityTagSource::FromBlackboard", EditConditionHides))
    FBlackboardKeySelector AbilityTagKey;
};
