# This file is part of MXE. See LICENSE.md for licensing information.

PKG             := treesitter
$(PKG)_WEBSITE  := https://github.com/tree-sitter/tree-sitter
$(PKG)_IGNORE   :=
$(PKG)_VERSION  := 0.25.10
$(PKG)_FILE     := tree-sitter-0.25.10.tar.gz
$(PKG)_CHECKSUM := ad5040537537012b16ef6e1210a572b927c7cdc2b99d1ee88d44a7dcdc3ff44c
$(PKG)_SUBDIR   := tree-sitter-$($(PKG)_VERSION)
$(PKG)_URL      := https://github.com/tree-sitter/tree-sitter/archive/refs/tags/v0.25.10.tar.gz
$(PKG)_GH_CONF  :=
$(PKG)_DEPS     := cc

define $(PKG)_BUILD
    $(MAKE) -C '$(SOURCE_DIR)' -j '$(JOBS)'
    $(MAKE) -C '$(SOURCE_DIR)' -j 1 install

endef
