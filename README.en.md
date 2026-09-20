# fcitx5-vi-cooperative

English / [日本語](README.md)

fcitx5-vi-cooperative is an addon that reproduces uim's "vi-cooperative mode" behavior on fcitx5.

## About uim's vi-cooperative mode

uim has a feature called "vi-cooperative mode" that supports the transition from vi's insert mode to normal mode. It is not limited to vi: whenever the IME is on and there is no uncommitted (preedit) text, pressing Escape (or Ctrl+[) turns the IME off.

1. Pressing Escape while there is uncommitted text → the uncommitted text is cancelled and the IME stays on (mozc's normal behavior).
2. Pressing Escape while there is no uncommitted text → the IME is turned off (switches to direct input), and the Escape key itself is still passed through to the target application (vi).

This addon uses fcitx5's `EventWatcherPhase::PostInputMethod` to perform this check after the input method's own key processing has finished, reproducing uim's behavior.

In uim, this feature is implemented individually, but identically, for each input method. This addon instead relies on fcitx5's own `InputPanel::empty()` and `KeyEvent::filtered()` APIs, so it does not depend on any specific input method. It only acts when the current input method is not already a direct-input layout (an ID starting with `keyboard-`, meaning there is nothing left to turn off), so it should work with input methods other than mozc as well. That said, **it has only been tested with mozc**. Behavior with other input methods is not individually supported.

## Requirements

Tested on the following environment on Gentoo Linux.

- app-i18n/fcitx-5.1.21
- sys-devel/gcc-15.3.0
- app-i18n/mozc-3.33.6133
- dev-build/make-4.4.1-r102
- dev-util/pkgconf-3.0.7

## Required packages

### Gentoo

```
emerge --ask app-i18n/fcitx
USE="fcitx5" emerge --ask app-i18n/mozc
```

### Ubuntu

```
sudo apt install fcitx5 fcitx5-mozc libfcitx5core-dev build-essential pkg-config
```

## Building

```
make
```

## Installation

### Home directory

```
make install
```

This installs into the following two locations.

| File | Destination |
|---|---|
| `libvicooperative.so` | `~/.local/lib/fcitx5/` |
| `vicooperative.conf` | `~/.local/share/fcitx5/addon/` |

`~/.local/lib/fcitx5/` is not searched automatically by fcitx5, so you need to set an environment variable (see below).

### System-wide

```
sudo make install-system
```

This installs into the directories returned by `pkg-config --variable=libdir Fcitx5Core` and `pkg-config --variable=prefix Fcitx5Core`.

| File | Destination (Gentoo) |
|---|---|
| `libvicooperative.so` | `/usr/lib64/fcitx5/` |
| `vicooperative.conf` | `/usr/share/fcitx5/addon/` |

Since it is installed into fcitx5's standard directories, it will be picked up and enabled as soon as fcitx5 is restarted.

## Starting it up

If you installed under your home directory with `make install`, you need to set an environment variable, since fcitx5 does not search that location automatically. If you installed system-wide with `make install-system`, this step is not needed. Restart the already-running fcitx5 (right-click its tray icon and restart it).

fcitx5 lets you specify where to look for addons via the `FCITX_ADDON_DIRS` environment variable. Once this variable is set, fcitx5 only searches the directories listed in it. So if you installed under your home directory, you need to include both that directory and the system directory in this variable.

Set an environment variable containing both the home and system directories, then restart fcitx5, like this:

```
$ pkg-config --variable=libdir Fcitx5Core
/usr/lib64

$ export FCITX_ADDON_DIRS="$HOME/.local/lib/fcitx5:/usr/lib64/fcitx5"

$ fcitx5 -r -d # or restart it from the tray icon
```

If you want this to keep working on subsequent logins, set this environment variable somewhere that is read before fcitx5 starts (e.g. `~/.xprofile`).

## Troubleshooting

- You see `Could not locate library libvicooperative.so ...` → `FCITX_ADDON_DIRS` is not set, or the path is wrong.
- The addon loads, but nothing happens when you press Escape → the current input method does not start with `keyboard-` (check with `fcitx5-remote -n`).
