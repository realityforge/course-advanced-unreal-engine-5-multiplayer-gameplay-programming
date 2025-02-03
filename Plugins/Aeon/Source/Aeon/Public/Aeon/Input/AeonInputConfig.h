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

#include "Aeon/Logging.h"
#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "EnhancedInputComponent.h"
#include "GameplayTagContainer.h"
#include "Logging/StructuredLog.h"
#include "AeonInputConfig.generated.h"

class UInputAction;
class UInputMappingContext;

/**
 * Association of an Input.Native subtag to an InputAction.
 */
USTRUCT(BlueprintType)
struct FAeonNativeInputAction
{
    GENERATED_BODY()

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (Categories = "Input.Native"))
    FGameplayTag InputTag{ FGameplayTag::EmptyTag };

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
    TObjectPtr<UInputAction> InputAction{ nullptr };

    bool IsValid() const { return InputTag.IsValid() && InputAction; }
};

/**
 * Association of a Input.Ability subtag to an InputAction.
 */
USTRUCT(BlueprintType)
struct FAeonAbilityInputAction
{
    GENERATED_BODY()

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (Categories = "Input.Ability"))
    FGameplayTag InputTag{ FGameplayTag::EmptyTag };

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
    TObjectPtr<UInputAction> InputAction{ nullptr };

    bool IsValid() const { return InputTag.IsValid() && InputAction; }
};

/**
 * DataAsset containing InputAction to GameplayTag mapping.
 */
UCLASS(BlueprintType, Const)
class AEON_API UAeonInputConfig final : public UDataAsset
{
    GENERATED_BODY()

    /**
     * The Default MappingContext applied to when using config.
     */
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
    TObjectPtr<UInputMappingContext> DefaultMappingContext;

    /**
     * A set of "well-know" tags that are bound to the InputAction.
     * These are manually bound.
     * The callbacks all take a FInputActionValue reference parameter.
     * The supported tags are those with the prefix "Input.Native."
     */
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (TitleProperty = "InputTag", AllowPrivateAccess = "true"))
    TArray<FAeonNativeInputAction> NativeInputActions;

    /**
     * A set of tags that are bound to InputAction.
     * The callbacks all take a FGameplayTag parameter.
     * The supported tags are those with the prefix "Input.Ability."
     */
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (TitleProperty = "InputTag", AllowPrivateAccess = "true"))
    TArray<FAeonAbilityInputAction> AbilityInputActions;

public:
    /**
     * Return the InputAction registered with the specified Tag.
     *
     * @param InInputTag the InputTag to look for.
     * @return the InputAction, if present.
     */
    UFUNCTION(BlueprintCallable, Category = "Aeon|Input")
    const UInputAction* FindNativeInputActionByTag(const FGameplayTag& InInputTag) const;

    FORCEINLINE UInputMappingContext* GetDefaultMappingContext() const { return DefaultMappingContext; };

#if WITH_EDITOR
    virtual EDataValidationResult IsDataValid(FDataValidationContext& Context) const override;
#endif

    template <class UserObject, typename CallbackFunc>
    void BindAbilityInputAction(UEnhancedInputComponent* InputComponent,
                                UserObject* ContextObject,
                                CallbackFunc InputPressedFunc,
                                CallbackFunc InputReleasedFunc);

    template <class UserObject, typename CallbackFunc>
    void BindNativeInputAction(UEnhancedInputComponent* InputComponent,
                               const FGameplayTag& InputTag,
                               ETriggerEvent TriggerEvent,
                               UserObject* ContextObject,
                               CallbackFunc Func);
};

template <class UserObject, typename CallbackFunc>
void UAeonInputConfig::BindAbilityInputAction(UEnhancedInputComponent* InputComponent,
                                              UserObject* ContextObject,
                                              CallbackFunc InputPressedFunc,
                                              CallbackFunc InputReleasedFunc)
{
    checkf(InputComponent, TEXT("InputComponent is null"));
    int32 Index = 0;
    for (const auto& Action : AbilityInputActions)
    {
        if (Action.IsValid())
        {
            InputComponent->BindAction(Action.InputAction,
                                       ETriggerEvent::Started,
                                       ContextObject,
                                       InputPressedFunc,
                                       Action.InputTag);
            InputComponent->BindAction(Action.InputAction,
                                       ETriggerEvent::Completed,
                                       ContextObject,
                                       InputReleasedFunc,
                                       Action.InputTag);
        }
        else
        {
            UE_LOGFMT(Aeon,
                      Error,
                      "BindAbilityInputAction: Invalid action at AbilityInputActions[{Index}] "
                      "(InputTag='{Tag}' InputAction='{Action}') in InputConfig named {Config}",
                      Index,
                      Action.InputTag.GetTagName(),
                      IsValid(Action.InputAction) ? Action.InputAction.GetFullName() : TEXT("?"),
                      GetName());
        }
        Index++;
    }
}

template <class UserObject, typename CallbackFunc>
void UAeonInputConfig::BindNativeInputAction(UEnhancedInputComponent* InputComponent,
                                             const FGameplayTag& InputTag,
                                             ETriggerEvent TriggerEvent,
                                             UserObject* ContextObject,
                                             CallbackFunc Func)
{
    checkf(InputComponent, TEXT("InputComponent is null"));
    if (const auto Action = FindNativeInputActionByTag(InputTag))
    {
        InputComponent->BindAction(Action, TriggerEvent, ContextObject, Func);
    }
    else
    {
        UE_LOGFMT(Aeon,
                  Warning,
                  "BindNativeInputAction: Unable to locate NativeInputAction "
                  "for InputTag '{Tag}' in InputConfig named '{Config}'",
                  InputTag.GetTagName(),
                  GetName());
    }
}
