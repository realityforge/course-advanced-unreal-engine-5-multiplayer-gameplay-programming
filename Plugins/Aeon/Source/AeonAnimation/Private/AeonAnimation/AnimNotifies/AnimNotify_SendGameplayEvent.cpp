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
#include "AeonAnimation/AnimNotifies/AnimNotify_SendGameplayEvent.h"
#include "AbilitySystemBlueprintLibrary.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(AnimNotify_SendGameplayEvent)

UAnimNotify_SendGameplayEvent::UAnimNotify_SendGameplayEvent()
{
#if WITH_EDITORONLY_DATA
    NotifyColor = FColor::Purple;
    bShouldFireInEditor = false;
#endif // WITH_EDITORONLY_DATA
}

void UAnimNotify_SendGameplayEvent::Notify(USkeletalMeshComponent* MeshComp,
                                           UAnimSequenceBase* Animation,
                                           const FAnimNotifyEventReference& EventReference)
{
    Super::Notify(MeshComp, Animation, EventReference);
    UAbilitySystemBlueprintLibrary::SendGameplayEventToActor(MeshComp->GetOwner(), EventTag, FGameplayEventData());
}
