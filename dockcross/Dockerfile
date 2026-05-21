# SPDX-FileCopyrightText: 2025 SEMAFOR Informatik & Energie AG, Basel
# SPDX-FileCopyrightText: 2015 Steeve Morin, Rob Burns, Matthew McCormick, Jean-Christophe-Fillion-Robin, Bensuperpc
# SPDX-License-Identifier: MIT
#
# https://github.com/dockcross/dockcross
# Not using dockcross/windows-shared-x64 because that includes gcc 11
# Changing MXE_PLUGIN_DIRS in settings.mk was too late because mxe is built
# already and two gcc would be installed
FROM dockcross/base
LABEL maintainer="Simon Bader <bader@semafor.ch>"
#
# Before including this script, make sure to set:
#
# WINEARCH environment variable to either "win64" or "win32"
# MXE_TARGET_ARCH argument to either "x86_64" or "i686". See http://mxe.cc/
# MXE_TARGET_THREAD argument to either "" or ".posix". Default is win32. See https://mxe.cc/
#
# For example:
#
ENV WINEARCH=win64
ENV WINEPREFIX=/tmp/wine
ARG MXE_TARGET_ARCH=x86_64
ARG MXE_TARGET_THREAD=
ARG MXE_TARGET_LINK=shared
#
# mxe master 2026-02-23
# qt6: update to version 6.9.2
# ARG MXE_GIT_TAG=be3ac46b3f6e501bef517e527bf4d5c3d5ccfc99
# qt6: update to version 6.10.2
# ARG MXE_GIT_TAG=9e26afc79b8e8c59ca863a35dd09755ceb09a3ca
# qt6: update to version 6.11.0
ARG MXE_GIT_TAG=717610f71f487cb3bc4c3caf2b9af1c760ab1de3

ENV CMAKE_TOOLCHAIN_FILE=/usr/src/mxe/usr/${MXE_TARGET_ARCH}-w64-mingw32.${MXE_TARGET_LINK}${MXE_TARGET_THREAD}/share/cmake/mxe-conf.cmake

ARG DEBIAN_FRONTEND=noninteractive

#
# WINE is used as an emulator for try_run and tests with CMake.
#
# Other dependencies are from the listed MXE requirements:
#   http://mxe.cc/#requirements
# 'cmake' is omitted because it is installed from source in the base image
#
RUN set -eux; \
  apt-get update && \
  apt-get install --no-install-recommends --yes \
    autopoint \
    bash \
    bison \
    bzip2 \
    flex \
    gettext \
    git \
    g++ \
    g++-multilib \
    gperf \
    intltool \
    libffi-dev \
    libgdk-pixbuf2.0-dev \
    libtool-bin \
    libltdl-dev \
    libssl-dev \
    libxml-parser-perl \
    libc6-dev-i386 \
    lzip \
    make \
    nsis \
    openssl \
    osslsigncode \
    p7zip-full \
    patch \
    perl \
    pkg-config \
    python3 \
    ruby \
    scons \
    sed \
    unzip \
    wget \
    wine \
    xz-utils \
    xvfb \
    xauth \
    python3-mako libopengl-dev \
    libgl1-mesa-dev libglu1-mesa-dev \
    yarnpkg \
  && \
  #
  # Install Wine
  #
  dpkg --add-architecture i386 && \
  apt-get update && \
  apt-get install -y wine32 && \
  wine reg add 'HKLM\Software\Microsoft\Windows NT\CurrentVersion' /v CurrentVersion /d 10.0 /f && \
  wine hostname && \
  yarnpkg global add tree-sitter-cli@0.25.10 && \
  rm -rf /var/lib/apt/lists/*

RUN set -eux; \
  #
  # Download MXE sources
  #
  cd /usr/src && \
  git clone https://github.com/mxe/mxe.git && \
  cd mxe && \
  git checkout ${MXE_GIT_TAG} && \
  #
  # Configure "settings.mk" required to build MXE
  #
  echo "MXE_TARGETS := ${MXE_TARGET_ARCH}-w64-mingw32.${MXE_TARGET_LINK}${MXE_TARGET_THREAD}" > settings.mk && \
  echo "MXE_USE_CCACHE :="                                         >> settings.mk && \
  echo "MXE_PLUGIN_DIRS := plugins/gcc14"                          >> settings.mk && \
  echo "LOCAL_PKG_LIST := cc cmake cppzmq curl libxml2 libxslt libltdl jsoncpp qt6 boost zlib flex log4cplus treesitter qwt"  >> settings.mk && \
  echo ".DEFAULT local-pkg-list:"                                  >> settings.mk && \
  echo "local-pkg-list: \$(LOCAL_PKG_LIST)"                        >> settings.mk

COPY flex.mk qwt.mk log4cplus.mk treesitter.mk /usr/src/mxe/src
#COPY qt6-qtquick3d.mk qt6-qtgraphs.mk /usr/src/mxe/src/qt/qt6/
#
# Build MXE
#
RUN set -eux; \
  cd /usr/src/mxe; \
  make JOBS=$(nproc) && \
  make clean-junk && make clean-pkg; \
  #
  # Cleanup: By keeping the MXE build system (Makefile, ...), derived images will be able to install
  #          additional packages.
  #
  rm -rf log pkg; \
  # fix missing pkg-config files (shared only)
    for pkg in log4cplus gettext boost libzmq curl libxml2 libxslt jsoncpp; do \
    if ! ./usr/bin/x86_64-w64-mingw32.${MXE_TARGET_LINK}-pkg-config --modversion ${pkg} >/dev/null 2>&1; then \
      version=$(grep -e '^$(PKG)_VERSION' ./src/${pkg}.mk | sed 's/^.*:=\(.*\)/\1/'); \
      echo "Version: $version" > ./usr/x86_64-w64-mingw32.${MXE_TARGET_LINK}/lib/pkgconfig/${pkg}.pc; \
    fi; \
    done; \
    version=$(grep -e '^#define ZMQ_VERSION_' ./usr/x86_64-w64-mingw32.${MXE_TARGET_LINK}/include/zmq.h | \
    cut -d ' ' -f 3 | tr '\n' '.' | sed 's/.$//'); \
    [ -n "$version" ] && echo "Version: $version" > ./usr/x86_64-w64-mingw32.${MXE_TARGET_LINK}/lib/pkgconfig/libzmq.pc; \
  # Update MXE toolchain file
  #
  echo 'set(CMAKE_CROSSCOMPILING_EMULATOR "/usr/bin/wine")' >> ${CMAKE_TOOLCHAIN_FILE}; \
  #
  # Add a sysmbolic link for makensis
  #
  ln -s /usr/bin/makensis /usr/bin/${MXE_TARGET_ARCH}-w64-mingw32.${MXE_TARGET_LINK}${MXE_TARGET_THREAD}-makensis && \
  #
  # Replace cmake and cpack binaries
  #
  cd /usr/bin && \
  rm cmake cpack && \
  ln -s /usr/src/mxe/usr/bin/${MXE_TARGET_ARCH}-w64-mingw32.${MXE_TARGET_LINK}${MXE_TARGET_THREAD}-cmake cmake && \
  ln -s /usr/src/mxe/usr/bin/${MXE_TARGET_ARCH}-w64-mingw32.${MXE_TARGET_LINK}${MXE_TARGET_THREAD}-cpack cpack && \
  #
  # clean up temporary wine files
  #
  rm -rf /tmp/wine-* && \
  # boost install fix
  cp /usr/src/mxe/usr/x86_64-w64-mingw32.shared/bin/libboost_iostreams-mt-x64.dll /usr/src/mxe/usr/x86_64-w64-mingw32.shared/lib/libboost_iostreams-mt-x64.dll && \
  cp /usr/src/mxe/usr/x86_64-w64-mingw32.shared/bin/libboost_random-mt-x64.dll /usr/src/mxe/usr/x86_64-w64-mingw32.shared/lib/libboost_random-mt-x64.dll && \
  cp /usr/src/mxe/usr/x86_64-w64-mingw32.shared/bin/libboost_regex-mt-x64.dll /usr/src/mxe/usr/x86_64-w64-mingw32.shared/lib/libboost_regex-mt-x64.dll && \
  #
  # create FlexLexer.h link (flex is for some unknown reason installed inside x86_64-pc-linux-gnu)
  #
  ln -s /usr/src/mxe/usr/x86_64-pc-linux-gnu/include/FlexLexer.h \
    /usr/src/mxe/usr/${MXE_TARGET_ARCH}-w64-mingw32.${MXE_TARGET_LINK}/include/ && \
  # must correct include path for jsoncpp
  cd /usr/src/mxe/usr/${MXE_TARGET_ARCH}-w64-mingw32.${MXE_TARGET_LINK}/include && \
    ln -s . jsoncpp 

ENV PATH=${PATH}:/usr/src/mxe/usr/bin

ENV CROSS_TRIPLE=${MXE_TARGET_ARCH}-w64-mingw32.${MXE_TARGET_LINK}${MXE_TARGET_THREAD}
ENV AS=/usr/src/mxe/usr/bin/${CROSS_TRIPLE}-as \
    AR=/usr/src/mxe/usr/bin/${CROSS_TRIPLE}-ar \
    CC=/usr/src/mxe/usr/bin/${CROSS_TRIPLE}-gcc \
    CPP=/usr/src/mxe/usr/bin/${CROSS_TRIPLE}-cpp \
    CXX=/usr/src/mxe/usr/bin/${CROSS_TRIPLE}-g++ \
    LD=/usr/src/mxe/usr/bin/${CROSS_TRIPLE}-ld \
    FC=/usr/src/mxe/usr/bin/${CROSS_TRIPLE}-gfortran

ENV WINEDLLOVERRIDES='winemenubuilder.exe,mscoree,mshtml=d'

RUN wget https://www.python.org/ftp/python/3.11.9/python-3.11.9-amd64.exe -O python.exe \
    && rm -rf /tmp/wine \
    && xvfb-run sh -c "wine reg add 'HKLM\Software\Microsoft\Windows NT\CurrentVersion' /v CurrentVersion /d 10.0 /f; wineserver -w" \
    && xvfb-run sh -c "wine python.exe /quiet TargetDir=C:\\Python Include_doc=0 InstallAllUsers=1 PrependPath=1 Include_launcher=0 Include_tcltk=0 Include_test=0 Include_tools=0 Shortcuts=0 Include_debug=0 Include_dev=0; wineserver -w" \
    && rm -rf /tmp/wine/wineserver /tmp/wine-*


WORKDIR /work

# Build-time metadata as defined at http://label-schema.org
ARG BUILD_DATE
ARG IMAGE=hub.semafor.ch/semafor/intens/dockcross-wintens-build
ARG VERSION=latest
ARG VCS_REF
ARG VCS_URL
LABEL org.label-schema.build-date=$BUILD_DATE \
      org.label-schema.name=$IMAGE \
      org.label-schema.version=$VERSION \
      org.label-schema.vcs-ref=$VCS_REF \
      org.label-schema.vcs-url=$VCS_URL \
      org.label-schema.schema-version="1.0"

# call the new docker image hub.semafor.ch/semafor/intens/dockcross-wintens-build
ENV DEFAULT_DOCKCROSS_IMAGE=hub.semafor.ch/semafor/intens/dockcross-wintens-build
