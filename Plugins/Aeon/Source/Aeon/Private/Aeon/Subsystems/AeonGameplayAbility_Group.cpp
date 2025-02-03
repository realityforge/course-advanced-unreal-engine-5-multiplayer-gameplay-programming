/*
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */
#include "Aeon/Subsystems/AeonGameplayAbility_Group.h"
#include "AbilitySystemComponent.h"
#include "Aeon/Logging.h"
#include "Aeon/Subsystems/GroupAbilitySystem.h"
#include "Logging/StructuredLog.h"
#include "Misc/DataValidation.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(AeonGameplayAbility_Group)

UAeonGameplayAbility_Group::UAeonGameplayAbility_Group(const FObjectInitializer& ObjectInitializer)
    : Super(ObjectInitializer)
{
    SetAbilityActivationPolicy(EAeonAbilityActivationPolicy::OnGiven);
    InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerExecution;
}

void UAeonGameplayAbility_Group::ActivateAbility(const FGameplayAbilitySpecHandle Handle,
                                                 const FGameplayAbilityActorInfo* ActorInfo,
                                                 const FGameplayAbilityActivationInfo ActivationInfo,
                                                 const FGameplayEventData* TriggerEventData)
{
    // The subsystem only exists when the node has authority. Thus, interacting with subsystem is safe.
    if (const auto GroupAbilitySystem = UWorld::GetSubsystem<UGroupAbilitySystem>(GetWorld()))
    {
        if (ensureAlways(GroupTag.IsValid()))
        {
            const auto AbilitySystemComponent = GetAbilitySystemComponentFromActorInfo_Ensured();
            GroupAbilitySystem->RegisterAbilitySystemComponent(GroupTag, AbilitySystemComponent);
        }
        else
        {
            UE_LOGFMT(Aeon, Error, "AeonGameplayAbility_Group unable to register with group as GroupTag is invalid.");
        }
    }
    Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);
}

void UAeonGameplayAbility_Group::EndAbility(const FGameplayAbilitySpecHandle Handle,
                                            const FGameplayAbilityActorInfo* ActorInfo,
                                            const FGameplayAbilityActivationInfo ActivationInfo,
                                            const bool bReplicateEndAbility,
                                            const bool bWasCancelled)
{
    // The subsystem only exists when the node has authority. Thus, interacting with subsystem is safe.
    if (const auto GroupAbilitySystem = UWorld::GetSubsystem<UGroupAbilitySystem>(GetWorld()))
    {
        if (ensureAlways(GroupTag.IsValid()))
        {
            const auto AbilitySystemComponent = GetAbilitySystemComponentFromActorInfo_Ensured();
            GroupAbilitySystem->UnregisterAbilitySystemComponent(GroupTag, AbilitySystemComponent);
        }
        else
        {
            UE_LOGFMT(Aeon, Error, "AeonGameplayAbility_Group unable to unregister from group as GroupTag is invalid.");
        }
    }
    Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
}

#if WITH_EDITOR
EDataValidationResult UAeonGameplayAbility_Group::IsDataValid(FDataValidationContext& Context) const
{
    auto Result = CombineDataValidationResults(Super::IsDataValid(Context), EDataValidationResult::Valid);
    if (!GetClass()->HasAnyClassFlags(CLASS_Abstract) && !GroupTag.IsValid())
    {
        Context.AddError(FText::FromString(FString::Printf(TEXT("GroupTag is invalid"))));
        Result = EDataValidationResult::Invalid;
    }
    return Result;
}
#endif
