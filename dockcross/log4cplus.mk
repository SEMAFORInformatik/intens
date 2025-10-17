# This file is part of MXE. See LICENSE.md for licensing information.

PKG             := log4cplus
$(PKG)_WEBSITE  := https://master.dl.sourceforge.net/project/log4cplus/log4cplus-stable/
$(PKG)_IGNORE   :=
$(PKG)_VERSION  := 2.1.2
$(PKG)_FILE     := log4cplus-2.1.2.tar.xz
$(PKG)_CHECKSUM := fbdabb4ef734fe1cc62169b23f0b480cc39127ac7b09b810a9c1229490d67e9e
$(PKG)_SUBDIR   := $(PKG)-$($(PKG)_VERSION)
$(PKG)_URL      := https://master.dl.sourceforge.net/project/log4cplus/log4cplus-stable/2.1.2/log4cplus-2.1.2.tar.xz
$(PKG)_GH_CONF  :=
$(PKG)_DEPS     := cc

define $(PKG)_BUILD
    cd '$(BUILD_DIR)' && $(TARGET)-cmake \
        -DWITH_UNIT_TESTS=OFF \
        -DLOG4CPLUS_BUILD_TESTING=OFF \
        -DLOG4CPLUS_BUILD_LOGGINGSERVER=OFF \
        -DUNICODE=OFF \
        '$(SOURCE_DIR)'
    $(MAKE) -C '$(BUILD_DIR)' -j '$(JOBS)'
    $(MAKE) -C '$(BUILD_DIR)' -j 1 install

endef
