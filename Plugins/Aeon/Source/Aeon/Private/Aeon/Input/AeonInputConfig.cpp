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
#include "Aeon/Input/AeonInputConfig.h"
#include "Misc/DataValidation.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(AeonInputConfig)

#if WITH_EDITOR
void FAeonNativeInputAction::InitEditorFriendlyTitleProperty()
{
    EditorFriendlyTitle = InputTag.ToString();
}

void FAeonAbilityInputAction::InitEditorFriendlyTitleProperty()
{
    EditorFriendlyTitle = InputTag.ToString();
}
#endif

const UInputAction* UAeonInputConfig::FindNativeInputActionByTag(const FGameplayTag& InInputTag) const
{
    for (const auto& Action : NativeInputActions)
    {
        if (Action.InputTag == InInputTag && Action.InputAction)
        {
            return Action.InputAction;
        }
    }
    return nullptr;
}

#if WITH_EDITOR
EDataValidationResult UAeonInputConfig::IsDataValid(FDataValidationContext& Context) const
{
    auto Result = CombineDataValidationResults(Super::IsDataValid(Context), EDataValidationResult::Valid);

    for (int32 Index = 0; Index < NativeInputActions.Num(); ++Index)
    {
        if (const auto& InputAction = NativeInputActions[Index]; !InputAction.IsValid())
        {
            if (!InputAction.InputTag.IsValid())
            {
                Context.AddError(
                    FText::FromString(FString::Printf(TEXT("NativeInputActions[%d].InputTag is invalid"), Index)));
                Result = EDataValidationResult::Invalid;
            }
            if (!InputAction.InputAction)
            {
                Context.AddError(
                    FText::FromString(FString::Printf(TEXT("NativeInputActions[%d].InputAction is invalid"), Index)));
                Result = EDataValidationResult::Invalid;
            }
        }
    }

    for (int32 Index = 0; Index < AbilityInputActions.Num(); ++Index)
    {
        if (const auto& InputAction = AbilityInputActions[Index]; !InputAction.IsValid())
        {
            if (!InputAction.InputTag.IsValid())
            {
                Context.AddError(
                    FText::FromString(FString::Printf(TEXT("AbilityInputActions[%d].InputTag is invalid"), Index)));
                Result = EDataValidationResult::Invalid;
            }
            if (!InputAction.InputAction)
            {
                Context.AddError(
                    FText::FromString(FString::Printf(TEXT("AbilityInputActions[%d].InputAction is invalid"), Index)));
                Result = EDataValidationResult::Invalid;
            }
        }
    }
    return Result;
}

void UAeonInputConfig::UpdateNativeInputActionsEditorFriendlyTitles()
{
    for (int32 Index = 0; Index < NativeInputActions.Num(); ++Index)
    {
        NativeInputActions[Index].InitEditorFriendlyTitleProperty();
    }
}

void UAeonInputConfig::UpdateAbilityInputActionsEditorFriendlyTitles()
{
    for (int32 Index = 0; Index < AbilityInputActions.Num(); ++Index)
    {
        AbilityInputActions[Index].InitEditorFriendlyTitleProperty();
    }
}

void UAeonInputConfig::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{
    Super::PostEditChangeProperty(PropertyChangedEvent);

    if (PropertyChangedEvent.Property)
    {
        // ReSharper disable once CppTooWideScopeInitStatement
        const auto PropertyName = PropertyChangedEvent.Property->GetFName();

        if ((GET_MEMBER_NAME_CHECKED(ThisClass, NativeInputActions)) == PropertyName)
        {
            UpdateNativeInputActionsEditorFriendlyTitles();
        }
        else if ((GET_MEMBER_NAME_CHECKED(ThisClass, AbilityInputActions)) == PropertyName)
        {
            UpdateAbilityInputActionsEditorFriendlyTitles();
        }
    }
}

void UAeonInputConfig::PostEditChangeChainProperty(FPropertyChangedChainEvent& PropertyChangedEvent)
{
    Super::PostEditChangeChainProperty(PropertyChangedEvent);

    // ReSharper disable once CppTooWideScopeInitStatement
    const auto PropertyName = PropertyChangedEvent.PropertyChain.GetActiveMemberNode()->GetValue()->GetFName();

    if ((GET_MEMBER_NAME_CHECKED(ThisClass, NativeInputActions)) == PropertyName)
    {
        UpdateNativeInputActionsEditorFriendlyTitles();
    }
    else if ((GET_MEMBER_NAME_CHECKED(ThisClass, AbilityInputActions)) == PropertyName)
    {
        UpdateAbilityInputActionsEditorFriendlyTitles();
    }
}

void UAeonInputConfig::PostLoad()
{
    Super::PostLoad();
    UpdateNativeInputActionsEditorFriendlyTitles();
    UpdateAbilityInputActionsEditorFriendlyTitles();
}
#endif
