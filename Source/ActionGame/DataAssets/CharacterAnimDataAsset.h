#pragma once

#include "ActionGameTypes.h"
#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "CharacterAnimDataAsset.generated.h"

/**
 *
 */
UCLASS()
class ACTIONGAME_API UCharacterAnimDataAsset : public UDataAsset
{
    GENERATED_BODY()

public:
    UPROPERTY(EditDefaultsOnly)
    FCharacterAnimData CharacterAnimData;
};
