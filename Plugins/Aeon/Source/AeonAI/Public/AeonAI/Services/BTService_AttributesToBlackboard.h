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

#include "BehaviorTree/BTService.h"
#include "CoreMinimal.h"
#include "GameplayEffectTypes.h"
#include "BTService_AttributesToBlackboard.generated.h"

class UAbilitySystemComponent;
class UBlackboardComponent;

/** How the service updates blackboard values. */
UENUM(BlueprintType)
enum class EAttributesToBlackboard_UpdateStrategy : uint8
{
    Subscribe UMETA(DisplayName = "Subscribe to Attribute Changes"),
    Poll UMETA(DisplayName = "Poll Each Service Tick")
};

/** The strategy for updating the Blackboard when we cannot resolve an AbilitySystemComponent from the SourceActor. */
UENUM(BlueprintType)
enum class EAttributesToBlackboard_MissingPolicy : uint8
{
    Leave UMETA(DisplayName = "Leave Blackboard Unchanged"),
    Clear UMETA(DisplayName = "Clear/Zero Blackboard Keys")
};

USTRUCT(BlueprintType)
struct FAttributeToBlackboardMapping
{
    GENERATED_BODY()

    /** GAS attribute to mirror. */
    UPROPERTY(EditAnywhere, Category = "Attributes")
    FGameplayAttribute Attribute;

    /** Blackboard key to set (supports Float, Int, Bool). */
    UPROPERTY(EditAnywhere, Category = "Blackboard")
    FBlackboardKeySelector BlackboardKey;
};

enum class EAttributesToBlackboard_EntryType : uint8
{
    Unknown,
    Float,
    Int,
    Bool
};

/** Runtime state to support a specific FAttributeToBlackboardMapping. */
struct FAttributeToBlackboardRuntimeData
{
    /** Delegate handles per mapping (parallel to Mappings). */
    FDelegateHandle DelegateHandle;

    /** Last known value supplied to the Blackboard. */
    float LastKnownValue;

    /** Cached Blackboard key ID for faster writes. */
    FBlackboard::FKey BlackboardKeyID{ FBlackboard::InvalidKey };

    /** Cached Blackboard entry type for this key. */
    EAttributesToBlackboard_EntryType EntryType{ EAttributesToBlackboard_EntryType::Unknown };

    void Reset();
};

/**
 * Behavior Tree service that mirrors Gameplay Ability System (GAS) attribute values into Blackboard keys.
 *
 * Overview
 * - Reads numeric attributes from an AbilitySystemComponent (ASC) owned by a "Source Actor" taken from the Blackboard.
 * - Writes those values into configurable Blackboard keys (Float, Int, or Bool).
 * - Supports two update strategies:
 *   - Subscribe: Subscribes to GAS attribute-changed delegates and writes on change.
 *   - Poll: Reads attributes every service tick and writes the latest values.
 *
 * Data Flow
 * 1) SourceActorKey is resolved from the running Behavior Tree's Blackboard.
 * 2) The service locates the SourceActor's AbilitySystemComponent (ASC).
 * 3) For each entry in Mappings:
 *    - Reads the specified Attribute from the ASC as a float.
 *    - Converts the value to the Blackboard key's type:
 *        Float key  -> Attribute value (float).
 *        Int key    -> FloorToInt(Attribute value).
 *        Bool key   -> Attribute value != 0 (true if non-zero).
 *    - Optionally avoids redundant writes if the value hasn't changed (within tolerance).
 *
 * Handling Missing Data
 * - If the SourceActor or its ASC cannot be resolved, MissingPolicy is applied:
 *     Leave -> Do nothing; Blackboard values remain unchanged.
 *     Clear -> Reset mapped Blackboard keys to zero/false.
 *
 * Performance Notes
 * - Keys and Blackboard types are cached for fast writes.
 * - When UpdateStrategy is Subscribe, the service only writes when notified (efficient).
 * - When UpdateStrategy is Poll, the service writes every tick (use for attributes without change delegates or where
 * polling is preferred).
 *
 * Usage
 * - Add this service to a Behavior Tree.
 * - Set SourceActorKey to an Object/Actor key that points to the pawn/actor owning the ASC.
 * - Add one or more Mappings from GAS attributes to Blackboard keys (Float/Int/Bool).
 * - Optionally toggle bOnlyUpdateWhenChanged and adjust ChangeTolerance for float comparisons.
 *
 * Limitations
 * - Only Float, Int, and Bool Blackboard key types are supported.
 * - Attributes must be valid FGameplayAttribute handles.
 */
UCLASS(meta = (DisplayName = "Attributes → Blackboard"))
class AEONAI_API UBTService_AttributesToBlackboard : public UBTService
{
    GENERATED_BODY()

public:
    UBTService_AttributesToBlackboard();

    /** Initialize runtime cache and push initial values (or apply missing policy) when the service becomes active. */
    virtual void OnBecomeRelevant(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;

    /** Poll and write values when using the Poll strategy; also re-resolves source/target if they changed. */
    virtual void TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds) override;

    /** Unbind all attribute change delegates and clear cached state when the service is no longer relevant. */
    virtual void OnCeaseRelevant(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;

    /** Resolve Blackboard keys against the asset when the BT asset is initialized. */
    virtual void InitializeFromAsset(UBehaviorTree& Asset) override;

protected:
    /** Blackboard key that indicates the Source Actor that owns the ASC. */
    UPROPERTY(EditAnywhere, Category = "Attributes")
    FBlackboardKeySelector SourceActorKey;

    /** Attribute → Blackboard mappings. */
    UPROPERTY(EditAnywhere, Category = "Attributes")
    TArray<FAttributeToBlackboardMapping> Mappings;

    /** Update strategy. */
    UPROPERTY(EditAnywhere, Category = "Attributes")
    EAttributesToBlackboard_UpdateStrategy UpdateStrategy{ EAttributesToBlackboard_UpdateStrategy::Subscribe };

    /** Missing AbilitySystemComponent policy. */
    UPROPERTY(EditAnywhere, Category = "Attributes")
    EAttributesToBlackboard_MissingPolicy MissingPolicy{ EAttributesToBlackboard_MissingPolicy::Clear };

    /** Only update Blackboard keys when the attribute value has changed. */
    UPROPERTY(EditAnywhere, Category = "Attributes")
    bool bOnlyUpdateWhenChanged{ true };

    /** Tolerance for float comparisons when bOnlyUpdateWhenChanged is true. */
    UPROPERTY(EditAnywhere,
              Category = "Attributes",
              meta = (EditCondition = "bOnlyUpdateWhenChanged", EditConditionHides))
    float ChangeTolerance{ KINDA_SMALL_NUMBER };

private:
    /** Cached blackboard target where we are writing attribute data. */
    TWeakObjectPtr<UBlackboardComponent> CachedTargetBlackboardComponent{ nullptr };

    /** Cached BlackboardData for blackboard target. */
    TWeakObjectPtr<UBlackboardData> CachedTargetBlackboardData{ nullptr };

    /** Cached source actor. */
    TWeakObjectPtr<AActor> CachedSourceActor{ nullptr };

    /** Cached AbilitySystemComponent where we are reading attribute data from. */
    TWeakObjectPtr<UAbilitySystemComponent> CachedSourceAbilitySystemComponent{ nullptr };

    /** Runtime state related to the Mappings. */
    TArray<FAttributeToBlackboardRuntimeData> RuntimeData;

    void ResetRuntimeData();

    /**
     * Cache properties relating to source AbilitySystem, target Blackboard and the mappings.
     */
    void CacheProperties(UBehaviorTreeComponent& OwnerComp);

    void CacheBlackboardProperties(UBehaviorTreeComponent& OwnerComp);

    void CacheAbilitySystemProperties(UBehaviorTreeComponent& OwnerComp);

    /** Bind delegates to attributes if required. */
    void BindAttributeDelegates();

    /** Unbind all subscribed delegates. */
    void UnbindAttributeDelegates();

    /** Attribute changed callback (shared). */
    void OnAttributeChanged(const FOnAttributeChangeData& Data);

    /** Push initial values immediately. */
    void WriteAllValues();

    /** Read all attributes from the AbilitySystemComponent and write them to the Blackboard. */
    void WriteAllMappings(const UAbilitySystemComponent* AbilitySystemComponent);

    /** Apply our Missing AbilitySystemComponent policy to all mapped keys. */
    void ApplyMissingPolicy();

    /** Test whether the new Value is nearly equal to the last known value within the specified tolerance. */
    bool IsAttributeValueNearlyEqual(const FAttributeToBlackboardRuntimeData& Data, float Value) const;

    /**
     * Update one mapping from a float value into its blackboard key using the conversion policy.
     * Only do this if bOnlyUpdateWhenChanged is false or change tolerance is reached.
     */
    void MaybeWriteValueToBB(UBlackboardComponent* const BlackboardComponent,
                             FAttributeToBlackboardRuntimeData& Data,
                             float Value);

    /** Update one mapping from a float value into its blackboard key using the conversion policy. */
    void WriteValueToBB(UBlackboardComponent* BlackboardComponent,
                        const FAttributeToBlackboardRuntimeData& Data,
                        float Value) const;
};
