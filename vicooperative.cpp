#include <fcitx/addonfactory.h>
#include <fcitx/addoninstance.h>
#include <fcitx/addonmanager.h>
#include <fcitx/inputcontext.h>
#include <fcitx/inputpanel.h>
#include <fcitx/instance.h>
#include <fcitx-config/iniparser.h>
#include <fcitx-utils/key.h>

#include "vicooperativeconfig.h"

namespace fcitx {

class ViCooperative final : public AddonInstance {
public:
    explicit ViCooperative(Instance *instance) : instance_(instance) {
        reloadConfig();

        // Registering at PostInputMethod (rather than the default) makes
        // this handler run AFTER the active input method engine's own key
        // processing, so the engine gets first refusal on Escape.
        //
        // Using the plain (EventType, phase, EventHandler) overload rather
        // than the templated watchEvent<EventType::...>(phase, callback)
        // convenience form for portability -- the latter isn't available in
        // all fcitx5 versions' headers.
        handler_ = instance_->watchEvent(
            EventType::InputContextKeyEvent, EventWatcherPhase::PostInputMethod,
            [this](Event &event) {
                auto &keyEvent = static_cast<KeyEvent &>(event);
                if (keyEvent.isRelease()) {
                    return;
                }
                auto *ic = keyEvent.inputContext();
                // Skip when already on a raw keyboard layout (nothing to
                // cancel/turn off). This keeps the check general across any
                // fcitx5 input method engine.
                if (instance_->inputMethod(ic).starts_with("keyboard-")) {
                    return;
                }
                if (!keyEvent.key().checkKeyList(*config_.escapeKeys)) {
                    return;
                }
                // If input method already consumed this key (e.g. to cancel an
                // in-progress conversion), or preedit is still non-empty,
                // do nothing -- let the first Escape just cancel conversion,
                // exactly like uim's vi-cooperative mode.
                if (keyEvent.filtered() || !ic->inputPanel().empty()) {
                    return;
                }
                // Nothing left to cancel: turn the IME off (switches the
                // last focused input context to the first input method,
                // i.e. the raw keyboard layout) and let the raw key reach
                // the client (vi) unfiltered.
                instance_->deactivate();
            });
    }

    const Configuration *getConfig() const override { return &config_; }
    void setConfig(const RawConfig &config) override {
        config_.load(config, true);
        safeSaveAsIni(config_, "conf/vicooperative.conf");
    }
    void reloadConfig() override {
        readAsIni(config_, "conf/vicooperative.conf");
    }

private:
    Instance *instance_;
    ViCooperativeConfig config_;
    std::unique_ptr<HandlerTableEntry<EventHandler>> handler_;
};

class ViCooperativeFactory : public AddonFactory {
public:
    AddonInstance *create(AddonManager *manager) override {
        return new ViCooperative(manager->instance());
    }
};

} // namespace fcitx

FCITX_ADDON_FACTORY_V2(vicooperative, fcitx::ViCooperativeFactory);
