#pragma once

#include <fcitx-config/configuration.h>
#include <fcitx-config/option.h>
#include <fcitx-utils/key.h>

namespace fcitx {

FCITX_CONFIGURATION(
    ViCooperativeConfig,
    KeyListOption escapeKeys{
        this, "EscapeKeys",
        "Keys that turn off the IME",
        {Key("Escape"), Key("Control+bracketleft")},
        KeyListConstrain(KeyConstrainFlag::AllowModifierLess)};);

} // namespace fcitx
