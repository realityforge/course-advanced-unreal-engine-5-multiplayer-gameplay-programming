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

#include "EnsureAbilityDefinesNoRelationshipTagsAction.h"
#include "Abilities/GameplayAbility.h"
#include "RuleRanger/RuleRangerUtilities.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(EnsureAbilityDefinesNoRelationshipTagsAction)

// ReSharper disable once CppMemberFunctionMayBeStatic
void UEnsureAbilityDefinesNoRelationshipTagsAction::EnsureEmptyTagContainer(URuleRangerActionContext* ActionContext,
                                                                            UGameplayAbility* const Ability,
                                                                            const FName& PropertyName)
{
    if (const auto Property = Ability->GetClass()->FindPropertyByName(PropertyName))
    {
        // ReSharper disable once CppTooWideScopeInitStatement
        const auto TagContainer = Property->ContainerPtrToValuePtr<FGameplayTagContainer>(Ability);
        if (TagContainer && !TagContainer->IsEmpty())
        {
            ActionContext->Error(FText::FromString(
                FString::Printf(TEXT("Ability defines tags for property %s but it is expected that "
                                     "all relationships are managed via the TagRelationship mechanisms."),
                                *PropertyName.ToString())));
        }
    }
}

void UEnsureAbilityDefinesNoRelationshipTagsAction::Apply_Implementation(URuleRangerActionContext* ActionContext,
                                                                         UObject* Object)
{
    static const FName CancelAbilitiesWithTagName("CancelAbilitiesWithTag");
    static const FName BlockAbilitiesWithTagName("BlockAbilitiesWithTag");
    static const FName ActivationRequiredTagsName("ActivationRequiredTags");
    static const FName ActivationBlockedTagsName("ActivationBlockedTags");
    static const FName SourceRequiredTagsName("SourceRequiredTags");
    static const FName SourceBlockedTagsName("SourceBlockedTags");
    static const FName TargetRequiredTagsName("TargetRequiredTags");
    static const FName TargetBlockedTagsName("TargetBlockedTags");

    const auto Ability = FRuleRangerUtilities::ToObject<UGameplayAbility>(Object);
    EnsureEmptyTagContainer(ActionContext, Ability, CancelAbilitiesWithTagName);
    EnsureEmptyTagContainer(ActionContext, Ability, BlockAbilitiesWithTagName);
    EnsureEmptyTagContainer(ActionContext, Ability, ActivationRequiredTagsName);
    EnsureEmptyTagContainer(ActionContext, Ability, ActivationBlockedTagsName);
    EnsureEmptyTagContainer(ActionContext, Ability, SourceRequiredTagsName);
    EnsureEmptyTagContainer(ActionContext, Ability, SourceBlockedTagsName);
    EnsureEmptyTagContainer(ActionContext, Ability, TargetRequiredTagsName);
    EnsureEmptyTagContainer(ActionContext, Ability, TargetBlockedTagsName);
}

UClass* UEnsureAbilityDefinesNoRelationshipTagsAction::GetExpectedType()
{
    return UGameplayAbility::StaticClass();
}
