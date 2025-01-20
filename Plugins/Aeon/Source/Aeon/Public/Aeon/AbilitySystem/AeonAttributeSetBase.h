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
#pragma once

#include "AttributeSet.h"
#include "CoreMinimal.h"
#include "AeonAttributeSetBase.generated.h"

struct FGameplayTag;

/**
 * Base class for AttributeSet that defines some common helper functions.
 */
UCLASS(Abstract)
class AEON_API UAeonAttributeSetBase : public UAttributeSet
{
    GENERATED_BODY()

    /**
     * Add a Gameplay Tag to the owning AbilitySystemComponent if the specified attribute's value is above or below
     * the specified threshold (based on  otherwise remove the Tag.
     *
     * <p>This is used to add a tag when Max Health has been reached.</p>
     *
     * @param Attribute The Attribute that is associated with the value.
     * @param Tag The Tag to add or remove.
     * @param Value The value of the attribute.
     * @param ThresholdValue The threshold value to test against.
     * @param bAbove True to add tag if above threshold, false to add tag if below threshold.
     * @param bReplicated True to mark the tag as replicated.
     * @param ErrorTolerance The error tolerance when comparing against the threshold value.
     */
    void AddTagBasedOnValueRelativeToThreshold(const FGameplayAttribute& Attribute,
                                               const FGameplayTag& Tag,
                                               const float Value,
                                               const float ThresholdValue,
                                               const bool bAbove = true,
                                               const bool bReplicated = false,
                                               const float ErrorTolerance = 0.001f) const;

protected:
    /**
     * Adjust the Base value of the specified attribute to maintain the same relative proportion to the MaxValue when
     * the Max value changes from OldMaxValue to NewMaxValue.
     *
     * @param Attribute The Attribute that has the Base value adjusted.
     * @param OldMaxValue The old Max value.
     * @param NewMaxValue The new Max value.
     * @param ErrorTolerance The error tolerance when checking for equality.
     */
    void AdjustAttributeAfterMaxValueChanges(const FGameplayAttribute& Attribute,
                                             const float OldMaxValue,
                                             const float NewMaxValue,
                                             const float ErrorTolerance = 0.001f) const;

    /**
     * Add a Gameplay Tag to the owning AbilitySystemComponent if the specified attribute's value is above the specified
     * threshold otherwise remove the Tag.
     *
     * <p>For example, this can be used to add a tag when Health reaches Max Health.</p>
     *
     * @param Attribute The Attribute that is associated with the value.
     * @param Tag The Tag to add or remove.
     * @param Value The value of the attribute.
     * @param ThresholdValue The threshold value to test against.
     * @param bReplicated True to mark the tag as replicated.
     * @param ErrorTolerance The error tolerance when comparing against the threshold value.
     */
    FORCEINLINE void AddTagIfValueAboveThreshold(const FGameplayAttribute& Attribute,
                                                 const FGameplayTag& Tag,
                                                 const float Value,
                                                 const float ThresholdValue,
                                                 const bool bReplicated = false,
                                                 const float ErrorTolerance = 0.001f) const
    {
        AddTagBasedOnValueRelativeToThreshold(Attribute, Tag, Value, ThresholdValue, true, bReplicated, ErrorTolerance);
    }

    /**
     * Add a Gameplay Tag to the owning AbilitySystemComponent if the specified attribute's value is below the specified
     * threshold otherwise remove the Tag.
     *
     * <p>For example, this can be used to add a tag when Health is below 25% of Max Health.</p>
     *
     * @param Attribute The Attribute that is associated with the value.
     * @param Tag The Tag to add or remove.
     * @param Value The value of the attribute.
     * @param ThresholdValue The threshold value to test against.
     * @param bReplicated True to mark the tag as replicated.
     * @param ErrorTolerance The error tolerance when comparing against the threshold value.
     */
    FORCEINLINE void AddTagIfValueBelowThreshold(const FGameplayAttribute& Attribute,
                                                 const FGameplayTag& Tag,
                                                 const float Value,
                                                 const float ThresholdValue,
                                                 const bool bReplicated = false,
                                                 const float ErrorTolerance = 0.001f) const
    {
        AddTagBasedOnValueRelativeToThreshold(Attribute,
                                              Tag,
                                              Value,
                                              ThresholdValue,
                                              false,
                                              bReplicated,
                                              ErrorTolerance);
    }
};
