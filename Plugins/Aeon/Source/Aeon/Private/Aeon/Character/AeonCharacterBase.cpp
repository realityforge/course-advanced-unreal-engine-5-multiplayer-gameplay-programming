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
#include "Aeon/Character/AeonCharacterBase.h"
#include "Aeon/AbilitySystem/AeonAbilitySet.h"
#include "Aeon/AbilitySystem/AeonAbilitySystemComponent.h"
#include "Engine/AssetManager.h"
#include "Engine/StreamableManager.h"
#include "Misc/DataValidation.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(AeonCharacterBase)

AAeonCharacterBase::AAeonCharacterBase(const FObjectInitializer& ObjectInitializer)
    : Super(ObjectInitializer.SetDefaultSubobjectClass<UAeonAbilitySystemComponent>(NAME_AbilitySystemComponent))
{
}

#if WITH_EDITOR
EDataValidationResult AAeonCharacterBase::IsDataValid(FDataValidationContext& Context) const
{
    auto Result = CombineDataValidationResults(Super::IsDataValid(Context), EDataValidationResult::Valid);

    if (!GetClass()->HasAnyClassFlags(CLASS_Abstract))
    {
        // ReSharper disable once CppTooWideScopeInitStatement
        if (AbilitySet.IsNull())
        {
            const auto String = FString::Printf(TEXT("AeonCharacter %s is not an abstract class but has not specified "
                                                     "the property AbilitySet"),
                                                *GetActorNameOrLabel());
            Context.AddError(FText::FromString(String));
            Result = EDataValidationResult::Invalid;
        }
    }

    return Result;
}
#endif

void AAeonCharacterBase::GrantAbilitySet(const UAeonAbilitySet* const Data, FAeonAbilitySetHandles* OutGrantedHandles)
{
    Data->GrantToAbilitySystem(GetAbilitySystemComponentFast(), OutGrantedHandles, GetAbilityLevel(), this);
}

void AAeonCharacterBase::GrantAbilitySetToAbilitySystemSync()
{
    check(!AbilitySet.IsNull());
    if (const auto Data = AbilitySet.LoadSynchronous())
    {
        GrantAbilitySet(Data);
        OnAbilitySystemComponentInitialized();
    }
    else
    {
        ensureMsgf(false,
                   TEXT("AbilitySet %s failed to load synchronously for actor %s"),
                   *AbilitySet.GetAssetName(),
                   *GetActorNameOrLabel());
    }
}

void AAeonCharacterBase::GrantAbilitySetToAbilitySystemAsync()
{
    check(!AbilitySet.IsNull());

    const auto Result = UAssetManager::GetStreamableManager().RequestAsyncLoad(
        AbilitySet.ToSoftObjectPath(),
        FStreamableDelegate::CreateLambda([this]() {
            if (const auto Data = AbilitySet.Get())
            {
                GrantAbilitySet(Data);
                OnAbilitySystemComponentInitialized();
            }
            else
            {
                ensureMsgf(false,
                           TEXT("AbilitySet %s failed to load asynchronously for actor %s"),
                           *AbilitySet.GetAssetName(),
                           *GetActorNameOrLabel());
            }
        }));
    ensureMsgf(Result.IsValid(),
               TEXT("RequestAsyncLoad for AbilitySet %s failed for actor %s"),
               *AbilitySet.GetAssetName(),
               *GetActorNameOrLabel());
}

int32 AAeonCharacterBase::GetAbilityLevel() const
{
    return 1;
}

void AAeonCharacterBase::ConfigureAbilitySystemComponent()
{
    if (HasAuthority())
    {
        if (ensureAlwaysMsgf(!AbilitySet.IsNull(),
                             TEXT("AbilitySet has not been assigned for actor %s"),
                             *GetActorNameOrLabel()))
        {
            if (EAbilitySetLoadPolicy::Sync == AbilitySetLoadPolicy)
            {
                GrantAbilitySetToAbilitySystemSync();
            }
            else
            {
                check(EAbilitySetLoadPolicy::Sync == AbilitySetLoadPolicy);
                GrantAbilitySetToAbilitySystemAsync();
            }
        }
    }
}

void AAeonCharacterBase::OnAbilitySystemComponentInitialized() {}
