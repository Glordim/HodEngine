#pragma once
#include "HodEngine/UIEditor/Export.hpp"

#include <HodEngine/UI/AnimationClip.hpp>

namespace hod::inline editor
{
	/// @brief The shape of an easing (see ui::AnimationKey::Ease): the blend factor over the segment, 0 and 1 marked, room
	/// left for what overshoots. Drawn at the cursor, e.g. in the tooltip of an easing's entry in a menu or a combo, with a word
	/// on the easings whose shape depends on more than the segment.
	HOD_UI_EDITOR_API void DrawEasingPreview(ui::AnimationKey::Easing easing);
}
