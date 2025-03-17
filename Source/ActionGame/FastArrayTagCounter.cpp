#include "FastArrayTagCounter.h"

int32 FFastArrayTagCounter::GetTagCount(FGameplayTag Tag) const
{
    for (auto ItemIter = TagArray.CreateConstIterator(); ItemIter; ++ItemIter)
    {
        // ReSharper disable once CppTooWideScopeInitStatement
        const auto& Item = *ItemIter;
        if (Item.Tag == Tag)
        {
            return Item.Count;
        }
    }
    return 0;
}

void FFastArrayTagCounter::AddTagCount(const FGameplayTag Tag, const int32 Count)
{
    for (auto ItemIter = TagArray.CreateIterator(); ItemIter; ++ItemIter)
    {
        // ReSharper disable once CppTooWideScopeInitStatement
        auto& Item = *ItemIter;
        if (Item.Tag == Tag)
        {
            Item.Count += Count;
            if (Item.Count <= 0)
            {
                ItemIter.RemoveCurrent();
                MarkArrayDirty();
            }
            else
            {
                MarkItemDirty(Item);
            }
            return;
        }
    }

    auto& Item = TagArray.AddDefaulted_GetRef();
    Item.Tag = Tag;
    Item.Count = Count;
    MarkItemDirty(Item);
}

const TArray<FFastArrayTagCounterRecord>& FFastArrayTagCounter::GetTagArray() const
{
    return TagArray;
}
