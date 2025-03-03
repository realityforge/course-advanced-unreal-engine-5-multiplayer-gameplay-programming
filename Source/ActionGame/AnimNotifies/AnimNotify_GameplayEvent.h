#pragma once

#include "Abilities/GameplayAbilityTypes.h"
#include "Animation/AnimNotifies/AnimNotify.h"
#include "CoreMinimal.h"
#include "AnimNotify_GameplayEvent.generated.h"

// This should probably be called UAnimNotify_SendGameplayEventToOwner
UCLASS()
class ACTIONGAME_API UAnimNotify_GameplayEvent : public UAnimNotify
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere)
    FGameplayEventData Payload;

public:
    UAnimNotify_GameplayEvent();

    virtual void Notify(USkeletalMeshComponent* MeshComp,
                        UAnimSequenceBase* Animation,
                        const FAnimNotifyEventReference& EventReference) override;
};
