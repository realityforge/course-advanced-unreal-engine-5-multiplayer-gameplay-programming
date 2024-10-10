#pragma once

#include "Animation/AnimNotifies/AnimNotify.h"
#include "CoreMinimal.h"
#include "AnimNotify_Step.generated.h"

enum class EFoot : uint8;

UCLASS()
class ACTIONGAME_API UAnimNotify_Step : public UAnimNotify
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, meta = (AllowPrivateAccess = "true"))
    EFoot Foot;

public:
    virtual void Notify(USkeletalMeshComponent* MeshComp,
                        UAnimSequenceBase* Animation,
                        const FAnimNotifyEventReference& EventReference) override;
};
