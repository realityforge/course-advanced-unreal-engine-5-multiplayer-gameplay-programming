#include "ActionGameStatics.h"
#include "ActionGameTypes.h"

const UItemStaticData* UActionGameStatics::GetItemStaticData(const TSubclassOf<UItemStaticData> ItemDataClass)
{
    return IsValid(ItemDataClass) ? ItemDataClass->GetDefaultObject<UItemStaticData>() : nullptr;
}
