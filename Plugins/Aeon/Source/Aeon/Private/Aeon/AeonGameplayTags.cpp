#include "Aeon/AeonGameplayTags.h"

namespace AeonGameplayTags
{
    // --------------------------------------------------- //
    // Input Tags
    // --------------------------------------------------- //

    UE_DEFINE_GAMEPLAY_TAG_COMMENT(Input_Ability_CancelOnRelease,
                                   "Input.Ability.CancelOnRelease",
                                   "If present in an abilities InputTags then cancel "
                                   "ability once the input is released")
    UE_DEFINE_GAMEPLAY_TAG_COMMENT(Input_Ability_Toggle,
                                   "Input.Ability.Toggle",
                                   "If present in an abilities InputTags then pressing "
                                   "the input will toggle activation state")

} // namespace AeonGameplayTags
