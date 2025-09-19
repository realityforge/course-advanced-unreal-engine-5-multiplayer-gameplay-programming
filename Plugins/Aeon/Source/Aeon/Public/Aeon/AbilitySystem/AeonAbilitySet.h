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
#include "Engine/DataAsset.h"
#include "GameplayTagContainer.h"
#include "ScalableFloat.h"
#include "AeonAbilitySet.generated.h"

class UGameplayAbility;
struct FActiveGameplayEffectHandle;
struct FGameplayAbilitySpecHandle;
class UAeonAbilitySet;
class UAbilitySystemComponent;
class UAttributeSet;
class UGameplayEffect;

/**
 * Data representing a Gameplay Ability that can be granted when granting an AbilitySet.
 */
USTRUCT(BlueprintType)
struct FAeonGameplayAbilityEntry
{
    GENERATED_BODY()

#if WITH_EDITORONLY_DATA
    friend UAeonAbilitySet;

    /** The transient title property to use in the editor. */
    UPROPERTY(VisibleDefaultsOnly, Transient, meta = (EditCondition = "false", EditConditionHides))
    FString EditorFriendlyTitle;
#endif

    /** The Gameplay Ability to grant. */
    UPROPERTY(EditDefaultsOnly, meta = (AllowAbstract = "false"))
    TSubclassOf<UGameplayAbility> Ability{ nullptr };

    /** The Base Level of the ability to grant. The Level may be modified when granted to an ASC. */
    UPROPERTY(EditDefaultsOnly)
    int32 Level{ 1 };

    /**
     * Tag indicating input tags that can trigger the Ability or specify how input is processed.
     */
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (Categories = "Input"))
    FGameplayTagContainer InputTags;

#if WITH_EDITOR

private:
    /**
     * Derive the transient title property for use in the editor.
     */
    void InitEditorFriendlyTitleProperty();
#endif
};

/**
 * Data representing a Gameplay Effect that can be granted when granting an AbilitySet.
 */
USTRUCT(BlueprintType)
struct FAeonGameplayEffectEntry
{
    GENERATED_BODY()

#if WITH_EDITORONLY_DATA
    friend UAeonAbilitySet;

    /** The transient title property to use in the editor. */
    UPROPERTY(VisibleDefaultsOnly, Transient, meta = (EditCondition = "false", EditConditionHides))
    FString EditorFriendlyTitle;
#endif

    /** The Gameplay Effect to grant. */
    UPROPERTY(EditDefaultsOnly, meta = (AllowAbstract = "false"))
    TSubclassOf<UGameplayEffect> Effect{ nullptr };

    /** The Base Level of the effect to grant. The Level may be modified when granted to an ASC. */
    UPROPERTY(EditDefaultsOnly)
    int32 Level{ 1 };

#if WITH_EDITOR

private:
    /**
     * Derive the transient title property for use in the editor.
     */
    void InitEditorFriendlyTitleProperty();
#endif
};

/**
 * Data representing an AttributeSet that can be granted when granting an AbilitySet.
 */
USTRUCT(BlueprintType)
struct FAeonAttributeSetEntry
{
    GENERATED_BODY()

#if WITH_EDITORONLY_DATA
    friend UAeonAbilitySet;

    /** The transient title property to use in the editor. */
    UPROPERTY(VisibleDefaultsOnly, Transient, meta = (EditCondition = "false", EditConditionHides))
    FString EditorFriendlyTitle;
#endif

    /** The AttributeSet to grant. */
    UPROPERTY(EditDefaultsOnly, meta = (AllowAbstract = "false"))
    TSubclassOf<UAttributeSet> AttributeSet{ nullptr };

#if WITH_EDITOR

private:
    /**
     * Derive the transient title property for use in the editor.
     */
    void InitEditorFriendlyTitleProperty();
#endif
};

USTRUCT(Blueprintable)
struct FAeonAttributeInitializer
{
    GENERATED_BODY()

#if WITH_EDITORONLY_DATA
    friend UAeonAbilitySet;

    /** The transient title property to use in the editor. */
    UPROPERTY(VisibleDefaultsOnly, Transient, meta = (EditCondition = "false", EditConditionHides))
    FString EditorFriendlyTitle;
#endif

    /**
     * The attribute to initialize.
     * The Attribute is expected to be defined in a FAeonAttributeSetEntry in the same AeonAbilitySet.
     */
    UPROPERTY(EditDefaultsOnly)
    FGameplayAttribute Attribute;

    /** The Base Level of the Attribute. The Level may be modified when granted to an ASC. */
    UPROPERTY(EditDefaultsOnly)
    int32 Level{ 1 };

    /**
     * The scaled value used to assign the attribute.
     */
    UPROPERTY(EditDefaultsOnly)
    FScalableFloat Value;

#if WITH_EDITOR

private:
    /**
     * Derive the transient title property for use in the editor.
     */
    void InitEditorFriendlyTitleProperty();
#endif
};

/**
 * A collection of handles and references as a result of granting a UAeonAbilitySet to an ASC.
 *
 * It should be noted that Gameplay Abilities and Gameplay Effects that were not successfully
 * granted will not appear in the lists below.
 */
USTRUCT(BlueprintType)
struct AEON_API FAeonAbilitySetHandles
{
    GENERATED_BODY()

    friend UAeonAbilitySet;

private:
    /** The AbilitySystemComponent that the AeonAbilitySet was granted to. */
    UPROPERTY()
    TObjectPtr<UAbilitySystemComponent> AbilitySystemComponent{ nullptr };

    /** Loose Gameplay Tags granted. */
    UPROPERTY()
    FGameplayTagContainer Tags;

    /** The SpecHandle instances resulting from successfully granting Gameplay Abilities. */
    UPROPERTY()
    TArray<FGameplayAbilitySpecHandle> AbilitySpecHandles;

    /** The EffectHandle instances resulting from successfully granting Gameplay Effects. */
    UPROPERTY()
    TArray<FActiveGameplayEffectHandle> EffectHandles;

    /** The references to "granted" AttributeSets. */
    UPROPERTY()
    TArray<TObjectPtr<UAttributeSet>> AttributeSets;

public:
    /**
     * Remove any granted abilities, effects and attributes from the associated AbilitySystemComponent.
     */
    void RemoveFromAbilitySystemComponent();

    /**
     * Return true if RemoveFromAbilitySystemComponent has not been invoked.
     *
     * @return true if the grants have not yet been removed.
     */
    bool IsValid() const;
};

/**
 * Container data asset that defines gameplay abilities, effects and attribute sets that can be granted to an ASC.
 *
 * NOTE: This is a renaming of the existing UAeonAbilitySystemGrantsBase and that is now considered deprecated. The
 * class was renamed and aligned with equivalent names that existed in the Lyra sample project to make it easier
 * to understand.
 */
UCLASS(BlueprintType, Const)
class AEON_API UAeonAbilitySet final : public UPrimaryDataAsset
{
    GENERATED_BODY()

    /** Loose Gameplay Tags to add to the ASC when the AeonAbilitySet is granted to the ASC. */
    UPROPERTY(EditDefaultsOnly)
    FGameplayTagContainer Tags;

    /** Gameplay Abilities to grant to the ASC when the AeonAbilitySet is granted to the ASC. */
    UPROPERTY(EditDefaultsOnly, meta = (TitleProperty = "EditorFriendlyTitle"))
    TArray<FAeonGameplayAbilityEntry> Abilities;

    /** Gameplay Effects to grant to the ASC when the AeonAbilitySet is granted to the ASC. */
    UPROPERTY(EditDefaultsOnly, meta = (TitleProperty = "EditorFriendlyTitle"))
    TArray<FAeonGameplayEffectEntry> Effects;

    /** AttributeSets to grant to the ASC when the AeonAbilitySet is granted to the ASC. */
    UPROPERTY(EditDefaultsOnly, meta = (TitleProperty = "EditorFriendlyTitle"))
    TArray<FAeonAttributeSetEntry> AttributeSets;

    /** Attribute base values to initialize on the ASC when the AeonAbilitySet is granted to the ASC. */
    UPROPERTY(EditDefaultsOnly, meta = (TitleProperty = "EditorFriendlyTitle"))
    TArray<FAeonAttributeInitializer> AttributeValues;

public:
    explicit UAeonAbilitySet(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

    /**
     * Grant the contained abilities, effects and attributes to the specified ASC.
     *
     * Note that the order in which they are granted is:
     * - AttributeSets in the order they appear in the array, then
     * - Abilities in the order they appear in the array, and finally
     * - Effects in the order they appear in the array
     *
     * The ordering is to allow effects to modify attributes, and activate-on-given abilities to prepare other systems
     * for effects activation.
     *
     * @param AbilitySystemComponent The ASC to grant the abilities, effects and attributes to.
     * @param OutGrantedHandles The handles that can be used to tak away anything that was granted. May pass nullptr.
     * @param LevelDelta The Delta to apply to Level of effects and abilities before granting them to the ASC.
     * @param SourceObject The source-object to apply when granting abilities.
     */
    void GrantToAbilitySystem(UAbilitySystemComponent* AbilitySystemComponent,
                              FAeonAbilitySetHandles* OutGrantedHandles = nullptr,
                              int32 LevelDelta = 0,
                              UObject* SourceObject = nullptr) const;

#pragma region AActor Interface
#if WITH_EDITOR
    virtual EDataValidationResult IsDataValid(FDataValidationContext& Context) const override;
#endif // WITH_EDITOR
#pragma endregion

#if WITH_EDITOR

private:
    /**
     * Update the titles for abilities to improve editor experience.
     */
    void UpdateAbilityEditorFriendlyTitles();

    /**
     * Update the titles for effects to improve editor experience.
     */
    void UpdateEffectEditorFriendlyTitles();

    /**
     * Update the titles for AttributeSets to improve editor experience.
     */
    void UpdateAttributeSetEditorFriendlyTitles();

    /**
     * Update the titles for AttributeValues to improve editor experience.
     */
    void UpdateAttributeValueEditorFriendlyTitles();

public:
    /**
     * Updates the titles of abilities, effects, and attribute sets.
     */
    virtual void PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent) override;

    /**
     * Updates the titles of abilities, effects, and attribute sets.
     * This is called when properties that are inside of structs are modified.
     */
    virtual void PostEditChangeChainProperty(FPropertyChangedChainEvent& PropertyChangedEvent) override;

    /**
     * Updates the titles of abilities, effects, and attribute sets after the asset is initially loaded.
     */
    virtual void PostLoad() override;

#endif // WITH_EDITOR
};
