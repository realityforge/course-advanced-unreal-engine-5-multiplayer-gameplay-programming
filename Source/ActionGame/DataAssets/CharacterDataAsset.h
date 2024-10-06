#pragma once

#include "ActionGameTypes.h"
#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "CharacterDataAsset.generated.h"

UCLASS(BlueprintType, Blueprintable)
class ACTIONGAME_API UCharacterDataAsset : public UDataAsset
{
    GENERATED_BODY()

public:
    UPROPERTY(EditDefaultsOnly)
    FCharacterData CharacterData;
};
