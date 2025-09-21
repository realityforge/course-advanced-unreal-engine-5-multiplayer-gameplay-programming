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

#include "Abilities/GameplayAbilityTypes.h"
#include "Animation/AnimNotifies/AnimNotify.h"
#include "CoreMinimal.h"
#include "AnimNotify_SendGameplayEvent.generated.h"

/**
 * UAnimNotify_SendGameplayEvent is an animation notify class that sends a gameplay event
 * to the owner of the skeletal mesh component when triggered. This can be used to communicate
 * gameplay-specific actions or states to the owner actor during animations.
 */
UCLASS(DisplayName = "Send Gameplay Event")
class AEONANIMATION_API UAnimNotify_SendGameplayEvent : public UAnimNotify
{
    GENERATED_BODY()

    /** Tag of the event to send */
    UPROPERTY(EditAnywhere, Category = AnimNotify, meta = (GameplayTagFilter = "GameplayEventTagsCategory"))
    FGameplayTag EventTag;

public:
    UAnimNotify_SendGameplayEvent();

    virtual void Notify(USkeletalMeshComponent* MeshComp,
                        UAnimSequenceBase* Animation,
                        const FAnimNotifyEventReference& EventReference) override;
};
