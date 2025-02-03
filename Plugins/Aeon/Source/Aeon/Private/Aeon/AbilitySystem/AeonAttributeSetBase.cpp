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
#include "Aeon/AbilitySystem/AeonAttributeSetBase.h"
#include "AbilitySystemComponent.h"
#include "Aeon/Logging.h"
#include "Logging/StructuredLog.h"

void UAeonAttributeSetBase::AddTagBasedOnValueRelativeToThreshold(const FGameplayAttribute& Attribute,
                                                                  const FGameplayTag& Tag,
                                                                  const float Value,
                                                                  const float ThresholdValue,
                                                                  const bool bAbove,
                                                                  const bool bReplicated,
                                                                  const float ErrorTolerance) const
{
    if (auto const AbilitySystemComponent = GetOwningAbilitySystemComponent())
    {
        const bool bTagPresent = AbilitySystemComponent->HasMatchingGameplayTag(Tag);
        const bool bThresholdTriggered =
            bAbove ? Value + ErrorTolerance > ThresholdValue : Value - ErrorTolerance < ThresholdValue;
        if (bThresholdTriggered && !bTagPresent)
        {
            if (bReplicated)
            {
                AbilitySystemComponent->SetReplicatedLooseGameplayTagCount(Tag, 1);
            }
            else
            {
                AbilitySystemComponent->SetLooseGameplayTagCount(Tag, 1);
            }
        }
        else if (!bThresholdTriggered && bTagPresent)
        {
            if (bReplicated)
            {
                AbilitySystemComponent->SetReplicatedLooseGameplayTagCount(Tag, 0);
            }
            else
            {
                AbilitySystemComponent->SetLooseGameplayTagCount(Tag, 0);
            }
        }
    }
    else
    {
        UE_LOGFMT(Aeon,
                  Error,
                  "AddTagBasedOnValueRelativeToThreshold invoked attribute='{Attribute}', "
                  "value={Value}, threshold={Threshold}, relationship={Relationship}, "
                  "with {RepStat} tag {Tag}",
                  Attribute.GetName(),
                  Value,
                  ThresholdValue,
                  bAbove ? TEXT("above") : TEXT("below"),
                  bReplicated ? TEXT("replicated") : TEXT("loose"),
                  Tag.GetTagName());
    }
}

void UAeonAttributeSetBase::AdjustAttributeAfterMaxValueChanges(const FGameplayAttribute& Attribute,
                                                                const float OldMaxValue,
                                                                const float NewMaxValue,
                                                                const float ErrorTolerance) const
{
    if (auto const AbilitySystemComponent = GetOwningAbilitySystemComponent())
    {
        if (OldMaxValue > 0.f && !FMath::IsNearlyEqual(OldMaxValue, NewMaxValue, ErrorTolerance))
        {
            const float ExistingBaseValue = AbilitySystemComponent->GetNumericAttributeBase(Attribute);
            const float NewBaseValue = ExistingBaseValue * NewMaxValue / OldMaxValue;
            AbilitySystemComponent->SetNumericAttributeBase(Attribute, NewBaseValue);
        }
    }
    else
    {
        UE_LOGFMT(Aeon,
                  Error,
                  "AdjustAttributeAfterMaxValueChanges invoked for attribute '{Attribute}' "
                  "to change value from {OldValue} to {NewValue} but no AbilitySystemComponent "
                  "owns AttributeSet",
                  Attribute.GetName(),
                  OldMaxValue,
                  NewMaxValue);
    }
}
