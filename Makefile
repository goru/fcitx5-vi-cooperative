CXX      := g++
CXXFLAGS := -std=c++20 -fPIC -Wall $(shell pkg-config --cflags Fcitx5Core)
LDFLAGS  := -shared $(shell pkg-config --libs Fcitx5Core)

ADDON_LIB_DIR  := $(HOME)/.local/lib/fcitx5
ADDON_CONF_DIR := $(HOME)/.local/share/fcitx5/addon

PREFIX             := $(shell pkg-config --variable=prefix Fcitx5Core)
LIBDIR             := $(shell pkg-config --variable=libdir Fcitx5Core)
SYSTEM_ADDON_LIB_DIR  := $(LIBDIR)/fcitx5
SYSTEM_ADDON_CONF_DIR := $(PREFIX)/share/fcitx5/addon

libvicooperative.so: vicooperative.cpp vicooperativeconfig.h
	$(CXX) $(CXXFLAGS) $(LDFLAGS) -o $@ $<

# User-level install: no root required, but needs FCITX_ADDON_DIRS set
# before fcitx5 starts. See README.md.
install: libvicooperative.so
	install -Dm755 libvicooperative.so "$(ADDON_LIB_DIR)/libvicooperative.so"
	install -Dm644 vicooperative.conf "$(ADDON_CONF_DIR)/vicooperative.conf"

# System-wide install: requires root (run with sudo/doas), but no
# environment variable is needed afterwards -- `fcitx5 -r -d` just works.
install-system: libvicooperative.so
	install -Dm755 libvicooperative.so "$(DESTDIR)$(SYSTEM_ADDON_LIB_DIR)/libvicooperative.so"
	install -Dm644 vicooperative.conf "$(DESTDIR)$(SYSTEM_ADDON_CONF_DIR)/vicooperative.conf"

clean:
	rm -f libvicooperative.so

.PHONY: install install-system clean
