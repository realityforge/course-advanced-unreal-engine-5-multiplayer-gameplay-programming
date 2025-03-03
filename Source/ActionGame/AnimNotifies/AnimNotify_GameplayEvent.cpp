#include "AnimNotifies/AnimNotify_GameplayEvent.h"
#include "AbilitySystemBlueprintLibrary.h"

UAnimNotify_GameplayEvent::UAnimNotify_GameplayEvent()
{
#if WITH_EDITORONLY_DATA
    NotifyColor = FColor::Purple;
    bShouldFireInEditor = false;
#endif // WITH_EDITORONLY_DATA
}

void UAnimNotify_GameplayEvent::Notify(USkeletalMeshComponent* MeshComp,
                                       UAnimSequenceBase* Animation,
                                       const FAnimNotifyEventReference& EventReference)
{
    Super::Notify(MeshComp, Animation, EventReference);
    UAbilitySystemBlueprintLibrary::SendGameplayEventToActor(MeshComp->GetOwner(), Payload.EventTag, Payload);
}
