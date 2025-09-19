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
#include "AeonAI/Services/BTService_AttributesToBlackboard.h"
#include "AbilitySystemComponent.h"
#include "AbilitySystemGlobals.h"
#include "Aeon/Logging.h"
#include "BehaviorTree/BehaviorTreeComponent.h"
#include "BehaviorTree/Blackboard/BlackboardKeyType_Bool.h"
#include "BehaviorTree/Blackboard/BlackboardKeyType_Float.h"
#include "BehaviorTree/Blackboard/BlackboardKeyType_Int.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "GameFramework/Actor.h"
#include "Logging/StructuredLog.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(BTService_AttributesToBlackboard)

void FAttributeToBlackboardRuntimeData::Reset()
{
    LastKnownValue = TNumericLimits<float>::Lowest();
    DelegateHandle.Reset();
    BlackboardKeyID = FBlackboard::InvalidKey;
    EntryType = EAttributesToBlackboard_EntryType::Unknown;
}

UBTService_AttributesToBlackboard::UBTService_AttributesToBlackboard()
{
    NodeName = TEXT("Attributes → Blackboard");
    bNotifyBecomeRelevant = true;
    bNotifyCeaseRelevant = true;
    bNotifyTick = true;

    // Blackboard key type filter: Object/Actor for source actor key
    SourceActorKey.AddObjectFilter(this, GET_MEMBER_NAME_CHECKED(ThisClass, SourceActorKey), AActor::StaticClass());
}

void UBTService_AttributesToBlackboard::ResetRuntimeData()
{
    for (auto i = 0; i < RuntimeData.Num(); i++)
    {
        RuntimeData[i].Reset();
    }
}

void UBTService_AttributesToBlackboard::OnBecomeRelevant(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
    Super::OnBecomeRelevant(OwnerComp, NodeMemory);

    RuntimeData.SetNum(Mappings.Num());

    ResetRuntimeData();
    CacheProperties(OwnerComp);
    WriteAllValues();
}

void UBTService_AttributesToBlackboard::TickNode(UBehaviorTreeComponent& OwnerComp,
                                                 uint8* NodeMemory,
                                                 const float DeltaSeconds)
{
    Super::TickNode(OwnerComp, NodeMemory, DeltaSeconds);

    // Re-resolve if the source actor changed externally during execution.
    CacheProperties(OwnerComp);

    if (EAttributesToBlackboard_UpdateStrategy::Poll == UpdateStrategy)
    {
        WriteAllValues();
    }
}

void UBTService_AttributesToBlackboard::OnCeaseRelevant(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
    Super::OnCeaseRelevant(OwnerComp, NodeMemory);
    UnbindAttributeDelegates();
    ResetRuntimeData();
    CachedSourceAbilitySystemComponent = nullptr;
    CachedSourceActor = nullptr;
    CachedTargetBlackboardComponent = nullptr;
}

void UBTService_AttributesToBlackboard::InitializeFromAsset(UBehaviorTree& Asset)
{
    Super::InitializeFromAsset(Asset);
    if (const auto BBAsset = GetBlackboardAsset())
    {
        for (auto Mapping : Mappings)
        {
            Mapping.BlackboardKey.ResolveSelectedKey(*BBAsset);
        }
    }
}

void UBTService_AttributesToBlackboard::CacheProperties(UBehaviorTreeComponent& OwnerComp)
{
    CacheBlackboardProperties(OwnerComp);
    CacheAbilitySystemProperties(OwnerComp);
}

void UBTService_AttributesToBlackboard::CacheBlackboardProperties(UBehaviorTreeComponent& OwnerComp)
{
    const auto BlackboardComponent = OwnerComp.GetBlackboardComponent();
    // Only reconfigure Blackboard Cached values if BlackboardComponent changes
    if (BlackboardComponent != CachedTargetBlackboardComponent.Get())
    {
        CachedTargetBlackboardComponent = BlackboardComponent;
        if (BlackboardComponent)
        {
            for (auto i = 0; i < Mappings.Num(); i++)
            {
                RuntimeData[i].BlackboardKeyID =
                    BlackboardComponent->GetKeyID(Mappings[i].BlackboardKey.SelectedKeyName);
            }
        }
        else
        {
            UE_LOGFMT(LogAeon,
                      Error,
                      "{Method}: BlackboardComponent removed or not present on owner.",
                      *GetNameSafe(this));
            for (auto i = 0; i < Mappings.Num(); i++)
            {
                RuntimeData[i].BlackboardKeyID = FBlackboard::InvalidKey;
            }
        }

        // Force a re-cache of data reliant on asset
        CachedTargetBlackboardData = nullptr;
    }

    const auto BlackboardAsset = BlackboardComponent->GetBlackboardAsset();
    if (BlackboardAsset != CachedTargetBlackboardData.Get())
    {
        CachedTargetBlackboardData = BlackboardAsset;
        if (BlackboardAsset)
        {
            for (auto i = 0; i < Mappings.Num(); i++)
            {
                auto BlackboardKey = Mappings[i].BlackboardKey;
                if (BlackboardKey.NeedsResolving())
                {
                    BlackboardKey.ResolveSelectedKey(*BlackboardAsset);
                }
                if (BlackboardKey.SelectedKeyType)
                {
                    const auto KeyType = BlackboardKey.SelectedKeyType;
                    if (KeyType->IsChildOf(UBlackboardKeyType_Float::StaticClass()))
                    {
                        RuntimeData[i].EntryType = EAttributesToBlackboard_EntryType::Float;
                    }
                    else if (KeyType->IsChildOf(UBlackboardKeyType_Int::StaticClass()))
                    {
                        RuntimeData[i].EntryType = EAttributesToBlackboard_EntryType::Int;
                    }
                    else if (KeyType->IsChildOf(UBlackboardKeyType_Bool::StaticClass()))
                    {
                        RuntimeData[i].EntryType = EAttributesToBlackboard_EntryType::Bool;
                    }
                    else
                    {
                        RuntimeData[i].EntryType = EAttributesToBlackboard_EntryType::Unknown;
                        UE_LOGFMT(LogAeon,
                                  Error,
                                  "{Method} - Attempted to use invalid Blackboard Key {BlackboardKey} "
                                  "for Attribute {Attribute} of Type {Type}",
                                  GetNameSafe(this),
                                  GetNameSafe(Mappings[i].BlackboardKey.SelectedKeyType),
                                  Mappings[i].Attribute.GetName(),
                                  GetNameSafe(KeyType));
                    }
                }
            }
        }
        else
        {
            UE_LOGFMT(LogAeon,
                      Error,
                      "{Method}: BlackboardAsset not present on BlackboardComponent.",
                      *GetNameSafe(this));
        }

        if (CachedSourceAbilitySystemComponent.IsValid())
        {
            WriteAllValues();
        }
    }
}

void UBTService_AttributesToBlackboard::CacheAbilitySystemProperties(UBehaviorTreeComponent& OwnerComp)
{
    const auto BlackboardComponent = OwnerComp.GetBlackboardComponent();
    AActor* SourceActor = nullptr;
    if (BlackboardComponent && SourceActorKey.SelectedKeyName.IsValid())
    {
        SourceActor = Cast<AActor>(BlackboardComponent->GetValueAsObject(SourceActorKey.SelectedKeyName));
    }

    // If the source actor has changed, drop bindings
    if (SourceActor != CachedSourceActor.Get())
    {
        UnbindAttributeDelegates();
        CachedSourceActor = SourceActor;
        // force re-resolve
        CachedSourceAbilitySystemComponent = nullptr;
    }

    if (!CachedSourceAbilitySystemComponent.IsValid() && SourceActor)
    {
        if (const auto AbilitySystemComponent = UAbilitySystemGlobals::GetAbilitySystemComponentFromActor(SourceActor))
        {
            CachedSourceAbilitySystemComponent = AbilitySystemComponent;
            if (EAttributesToBlackboard_UpdateStrategy::Subscribe == UpdateStrategy)
            {
                BindAttributeDelegates();
                if (CachedTargetBlackboardData.IsValid())
                {
                    WriteAllValues();
                }
            }
        }
        else
        {
            UE_LOGFMT(LogAeon,
                      Error,
                      "{Method}: SourceActor does not have an AbilitySystemComponent to source Attribute data from.",
                      *GetNameSafe(this));
        }
    }
}

void UBTService_AttributesToBlackboard::BindAttributeDelegates()
{
    if (const auto AbilitySystemComponent = CachedSourceAbilitySystemComponent.Get())
    {
        for (auto i = 0; i < Mappings.Num(); i++)
        {
            const auto& Attribute = Mappings[i].Attribute;
            if (Attribute.IsValid())
            {
                auto& Delegate = AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(Attribute);
                RuntimeData[i].DelegateHandle = Delegate.AddUObject(this, &ThisClass::OnAttributeChanged);
            }
        }
    }
}

void UBTService_AttributesToBlackboard::UnbindAttributeDelegates()
{
    if (const auto AbilitySystemComponent = CachedSourceAbilitySystemComponent.Get())
    {
        for (auto i = 0; i < RuntimeData.Num(); i++)
        {
            if (RuntimeData[i].DelegateHandle.IsValid() && Mappings.IsValidIndex(i) && Mappings[i].Attribute.IsValid())
            {
                auto& Delegate = AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(Mappings[i].Attribute);
                Delegate.Remove(RuntimeData[i].DelegateHandle);
                RuntimeData[i].DelegateHandle.Reset();
            }
        }
    }
}

void UBTService_AttributesToBlackboard::OnAttributeChanged(const FOnAttributeChangeData& Data)
{
    // When any subscribed attribute changes, update all mappings for that attribute.
    if (const auto BlackboardComponent = CachedTargetBlackboardComponent.Get())
    {
        for (auto i = 0; i < Mappings.Num(); i++)
        {
            const auto& Mapping = Mappings[i];
            if (Mapping.Attribute == Data.Attribute)
            {
                check(RuntimeData.IsValidIndex(i));
                MaybeWriteValueToBB(BlackboardComponent, RuntimeData[i], Data.NewValue);
                break;
            }
        }
    }
}

void UBTService_AttributesToBlackboard::WriteAllValues()
{
    if (const auto AbilitySystemComponent = CachedSourceAbilitySystemComponent.Get())
    {
        WriteAllMappings(AbilitySystemComponent);
    }
    else
    {
        ApplyMissingPolicy();
    }
}

void UBTService_AttributesToBlackboard::WriteAllMappings(const UAbilitySystemComponent* AbilitySystemComponent)
{
    if (const auto BlackboardComponent = CachedTargetBlackboardComponent.Get())
    {
        for (auto i = 0; i < Mappings.Num(); i++)
        {
            const auto& Mapping = Mappings[i];
            if (Mapping.Attribute.IsValid())
            {
                check(RuntimeData.IsValidIndex(i));
                const auto Value = AbilitySystemComponent->GetNumericAttribute(Mapping.Attribute);
                MaybeWriteValueToBB(BlackboardComponent, RuntimeData[i], Value);
            }
        }
    }
}

void UBTService_AttributesToBlackboard::ApplyMissingPolicy()
{
    if (EAttributesToBlackboard_MissingPolicy::Clear == MissingPolicy)
    {
        if (const auto BlackboardComponent = CachedTargetBlackboardComponent.Get())
        {
            for (auto i = 0; i < Mappings.Num(); i++)
            {
                const auto& Runtime = RuntimeData[i];
                if (FBlackboard::InvalidKey != Runtime.BlackboardKeyID)
                {
                    if (EAttributesToBlackboard_EntryType::Float == Runtime.EntryType)
                    {
                        BlackboardComponent->SetValue<UBlackboardKeyType_Float>(Runtime.BlackboardKeyID, 0.f);
                    }
                    else if (EAttributesToBlackboard_EntryType::Int == Runtime.EntryType)
                    {
                        BlackboardComponent->SetValue<UBlackboardKeyType_Int>(Runtime.BlackboardKeyID, 0);
                    }
                    else if (EAttributesToBlackboard_EntryType::Bool == Runtime.EntryType)
                    {
                        BlackboardComponent->SetValue<UBlackboardKeyType_Bool>(Runtime.BlackboardKeyID, false);
                    }
                }
            }
        }
    }
}

bool UBTService_AttributesToBlackboard::IsAttributeValueNearlyEqual(const FAttributeToBlackboardRuntimeData& Data,
                                                                    const float Value) const
{
    // Compare after converting the attribute value to the target Blackboard entry type.
    if (EAttributesToBlackboard_EntryType::Int == Data.EntryType)
    {
        return FMath::FloorToInt(Value) == FMath::FloorToInt(Data.LastKnownValue);
    }
    else if (EAttributesToBlackboard_EntryType::Bool == Data.EntryType)
    {
        return !FMath::IsNearlyZero(Value) == !FMath::IsNearlyZero(Data.LastKnownValue);
    }
    else
    {
        return FMath::IsNearlyEqual(Value, Data.LastKnownValue, ChangeTolerance);
    }
}

// ReSharper disable once CppMemberFunctionMayBeConst
void UBTService_AttributesToBlackboard::MaybeWriteValueToBB(UBlackboardComponent* const BlackboardComponent,
                                                            FAttributeToBlackboardRuntimeData& Data,
                                                            const float Value)
{
    if (bOnlyUpdateWhenChanged)
    {
        if (!IsAttributeValueNearlyEqual(Data, Value))
        {
            Data.LastKnownValue = Value;
            WriteValueToBB(BlackboardComponent, Data, Value);
        }
    }
    else
    {
        WriteValueToBB(BlackboardComponent, Data, Value);
    }
}

// ReSharper disable once CppMemberFunctionMayBeStatic
void UBTService_AttributesToBlackboard::WriteValueToBB(UBlackboardComponent* const BlackboardComponent,
                                                       const FAttributeToBlackboardRuntimeData& Data,
                                                       const float Value) const
{
    const auto KeyID = Data.BlackboardKeyID;
    if (FBlackboard::InvalidKey != KeyID)
    {
        if (EAttributesToBlackboard_EntryType::Float == Data.EntryType)
        {
            BlackboardComponent->SetValue<UBlackboardKeyType_Float>(KeyID, Value);
        }
        else if (EAttributesToBlackboard_EntryType::Int == Data.EntryType)
        {
            BlackboardComponent->SetValue<UBlackboardKeyType_Int>(KeyID, FMath::FloorToInt(Value));
        }
        else if (EAttributesToBlackboard_EntryType::Bool == Data.EntryType)
        {
            BlackboardComponent->SetValue<UBlackboardKeyType_Bool>(KeyID, !FMath::IsNearlyZero(Value));
        }
    }
}
