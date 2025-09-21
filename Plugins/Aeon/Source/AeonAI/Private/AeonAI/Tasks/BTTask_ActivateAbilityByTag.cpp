#include "AeonAI/Tasks/BTTask_ActivateAbilityByTag.h"
#include "AIController.h"
#include "AbilitySystemComponent.h"
#include "Aeon/AeonFunctionLibrary.h"
#include "Aeon/Logging.h"
#include "BehaviorTree/BehaviorTree.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "GameFramework/Actor.h"
#include "GameplayTagContainer.h"
#include "Logging/StructuredLog.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(BTTask_ActivateAbilityByTag)

UBTTask_ActivateAbilityByTag::UBTTask_ActivateAbilityByTag()
{
    NodeName = TEXT("Activate Ability By Tag");
}

FString UBTTask_ActivateAbilityByTag::GetStaticDescription() const
{
    if (EAeonAbilityTagSource::FromProperty == AbilityTagSource)
    {
        return FString::Printf(TEXT("Activate Ability: [%s]"), *AbilityTag.ToString());
    }
    else
    {
        return FString::Printf(TEXT("Activate Ability: BB Key [%s]"), *AbilityTagKey.SelectedKeyName.ToString());
    }
}

void UBTTask_ActivateAbilityByTag::InitializeFromAsset(UBehaviorTree& Asset)
{
    Super::InitializeFromAsset(Asset);

    if (EAeonAbilityTagSource::FromBlackboard == AbilityTagSource)
    {
        AbilityTagKey.ResolveSelectedKey(*Asset.GetBlackboardAsset());
    }
}

FGameplayTag UBTTask_ActivateAbilityByTag::ResolveAbilityTag(UBehaviorTreeComponent& BehaviorTreeComponent)
{
    if (EAeonAbilityTagSource::FromProperty == AbilityTagSource)
    {
        return AbilityTag;
    }
    else if (const auto Blackboard = BehaviorTreeComponent.GetBlackboardComponent())
    {
        const FName TagName = Blackboard->GetValueAsName(AbilityTagKey.SelectedKeyName);
        if (TagName.IsNone())
        {
            UE_LOGFMT(LogAeon,
                      Warning,
                      "ActivateAbilityByTag task failed: Blackboard key '{Key}' has no tag",
                      AbilityTagKey.SelectedKeyName);
            return FGameplayTag::EmptyTag;
        }
        else
        {
            return FGameplayTag::RequestGameplayTag(TagName);
        }
    }
    else
    {
        UE_LOGFMT(LogAeon, Error, "ActivateAbilityByTag task failed: Blackboard component missing");
        return FGameplayTag::EmptyTag;
    }
}

EBTNodeResult::Type UBTTask_ActivateAbilityByTag::ExecuteTask(UBehaviorTreeComponent& BehaviorTreeComponent,
                                                              uint8* NodeMemory)
{
    if (const auto AIController = BehaviorTreeComponent.GetAIOwner())
    {
        if (const auto Pawn = AIController->GetPawn())
        {
            if (const auto AbilitySystemComponent = Pawn->FindComponentByClass<UAbilitySystemComponent>())
            {
                const auto Tag = ResolveAbilityTag(BehaviorTreeComponent);
                if (Tag.IsValid())
                {
                    if (UAeonFunctionLibrary::TryActivateRandomSingleAbilityByTag(AbilitySystemComponent, Tag))
                    {
                        UE_LOGFMT(LogAeon,
                                  VeryVerbose,
                                  "ActivateAbilityByTag task succeeded: Activated ability with tag '{Tag}' on '{Pawn}'",
                                  Tag.GetTagName(),
                                  Pawn->GetActorNameOrLabel());
                        return EBTNodeResult::Succeeded;
                    }
                    else
                    {
                        UE_LOGFMT(LogAeon,
                                  Verbose,
                                  "ActivateAbilityByTag task failed: No matching ability or "
                                  "activation failed for tag '{Tag}' on '{Pawn}'",
                                  Tag.GetTagName(),
                                  Pawn->GetActorNameOrLabel());
                        return EBTNodeResult::Failed;
                    }
                }
                else
                {
                    UE_LOGFMT(LogAeon,
                              Warning,
                              "ActivateAbilityByTag task failed: Resolved tag is invalid for Pawn '{Pawn}'",
                              Pawn->GetActorNameOrLabel());
                    return EBTNodeResult::Failed;
                }
            }
            else
            {
                UE_LOGFMT(LogAeon,
                          Warning,
                          "ActivateAbilityByTag task failed: no AbilitySystemComponent found on Pawn '{Pawn}'",
                          Pawn->GetActorNameOrLabel());
                return EBTNodeResult::Failed;
            }
        }
        else
        {
            UE_LOGFMT(LogAeon, Error, "ActivateAbilityByTag task failed: AIController has no controlled Pawn");
            return EBTNodeResult::Failed;
        }
    }
    else
    {
        UE_LOGFMT(LogAeon, Error, "ActivateAbilityByTag task failed: missing AIController");
        return EBTNodeResult::Failed;
    }
}
