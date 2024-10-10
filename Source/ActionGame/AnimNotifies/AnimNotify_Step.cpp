#include "AnimNotifies/AnimNotify_Step.h"
#include "ActionGameCharacter.h"
#include "ActorComponents/FootstepsComponent.h"

void UAnimNotify_Step::Notify(USkeletalMeshComponent* MeshComp,
                              UAnimSequenceBase* Animation,
                              const FAnimNotifyEventReference& EventReference)
{
    Super::Notify(MeshComp, Animation, EventReference);

    if (const auto Character = MeshComp ? Cast<AActionGameCharacter>(MeshComp->GetOwner()) : nullptr)
    {
        if (const auto FootstepsComponent = Character->GetFootstepsComponent())
        {
            FootstepsComponent->HandleFootStep(Foot);
        }
    }
}
