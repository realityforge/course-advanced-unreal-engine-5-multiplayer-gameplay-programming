#pragma once

#include "CoreMinimal.h"
#include "NativeGameplayTags.h"

namespace AeonGameplayTags
{
    // --------------------------------------------------- //
    // Input Tags
    // --------------------------------------------------- //

    // If present in an abilities InputTags then cancel ability once the input is released
    AEON_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Input_Ability_CancelOnRelease)
    // If present in an abilities InputTags then pressing the input will toggle activation state
    AEON_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Input_Ability_Toggle)

} // namespace AeonGameplayTags
