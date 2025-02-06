#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "ActionGameStatics.generated.h"

class UItemStaticData;

UCLASS()
class ACTIONGAME_API UActionGameStatics : public UBlueprintFunctionLibrary
{
    GENERATED_BODY()

public:
    UFUNCTION(BlueprintCallable, BlueprintPure, Category = "ActionGameStatics")
    static const UItemStaticData* GetItemStaticData(TSubclassOf<UItemStaticData> ItemDataClass);
};
