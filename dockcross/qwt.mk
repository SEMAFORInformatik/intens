# This file is part of MXE. See LICENSE.md for licensing information.

PKG             := qwt
$(PKG)_WEBSITE  := https://qwt.sourceforge.io/
$(PKG)_DESCR    := Qwt
$(PKG)_VERSION  := 6.3.0
$(PKG)_CHECKSUM := 268d8d41974d263014fea13168324452f41e02880db573794f559995a4e11331
$(PKG)_SUBDIR   := $(PKG)-$($(PKG)_VERSION)
$(PKG)_FILE     := $($(PKG)_SUBDIR).zip
$(PKG)_URL      := https://$(SOURCEFORGE_MIRROR)/project/$(PKG)/$(PKG)/$($(PKG)_VERSION)/$($(PKG)_FILE)
$(PKG)_QT_DIR   := qt6
$(PKG)_DEPS     := cc qt6-qtbase qt6-qtsvg

define $(PKG)_UPDATE
    $(WGET) -q -O- 'https://sourceforge.net/projects/qwt/files/qwt/' | \
    $(SED) -n 's,.*/projects/.*/\([0-9][^"]*\)/".*,\1,p' | \
    head -1
endef

define $(PKG)_BUILD
    $(if $(BUILD_STATIC),\
        echo "QWT_CONFIG -= QwtDll" >> '$(SOURCE_DIR)/qwtconfig.pri')

	sed -if 's@C:/Qwt-.*@$(PREFIX)/$(TARGET)/qt6@' '$(SOURCE_DIR)'/qwtconfig.pri
    # doesn't support out-of-source build
    cd '$(SOURCE_DIR)' && $(PREFIX)/$(TARGET)/$($(PKG)_QT_DIR)/bin/qmake qwt.pro
    $(MAKE) -C '$(SOURCE_DIR)' -j '$(JOBS)'
    $(MAKE) -C '$(SOURCE_DIR)' -j 1 install

    #build sinusplot example to test linkage
    cd '$(SOURCE_DIR)/examples/sinusplot' && $(PREFIX)/$(TARGET)/$($(PKG)_QT_DIR)/bin/qmake
    $(MAKE) -C '$(SOURCE_DIR)/examples/sinusplot' -f 'Makefile.Release' -j '$(JOBS)'

    # install
    $(INSTALL) -m755 '$(SOURCE_DIR)/examples/bin/sinusplot.exe' '$(PREFIX)/$(TARGET)/bin/test-qwt.exe'
endef
