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
# MXE_TARGET_LINK argument to either "static" or "shared"
#
# For example:
#
ENV WINEARCH=win64
ENV WINEPREFIX=/tmp/wine
ARG MXE_TARGET_ARCH=x86_64
ARG MXE_TARGET_THREAD=
ARG MXE_TARGET_LINK=shared
ARG MXE_TARGET_LINK2=static
#
# mxe master 2025-09-29
# qt6: update to version 6.9.2
ARG MXE_GIT_TAG=be3ac46b3f6e501bef517e527bf4d5c3d5ccfc99

ENV CMAKE_TOOLCHAIN_FILE=/usr/src/mxe/usr/${MXE_TARGET_ARCH}-w64-mingw32.${MXE_TARGET_LINK}${MXE_TARGET_THREAD}/share/cmake/mxe-conf.cmake
ENV CMAKE_TOOLCHAIN_FILE2=/usr/src/mxe/usr/${MXE_TARGET_ARCH}-w64-mingw32.${MXE_TARGET_LINK2}${MXE_TARGET_THREAD}/share/cmake/mxe-conf.cmake

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
  yarnpkg global add tree-sitter-cli && \
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
  cd /usr/src/mxe && \
  echo "MXE_TARGETS := ${MXE_TARGET_ARCH}-w64-mingw32.${MXE_TARGET_LINK}${MXE_TARGET_THREAD} ${MXE_TARGET_ARCH}-w64-mingw32.${MXE_TARGET_LINK2}${MXE_TARGET_THREAD}" > settings.mk && \
  echo "MXE_USE_CCACHE :="                                         >> settings.mk && \
  echo "MXE_PLUGIN_DIRS := plugins/gcc14"                          >> settings.mk && \
  echo "LOCAL_PKG_LIST := cc cmake cppzmq curl libxml2 libxslt libltdl jsoncpp qt6 qwt zlib flex log4cplus treesitter"  >> settings.mk && \
  echo ".DEFAULT local-pkg-list:"                                  >> settings.mk && \
  echo "local-pkg-list: \$(LOCAL_PKG_LIST)"                        >> settings.mk

COPY flex.mk libxslt.mk qwt.mk log4cplus.mk treesitter.mk /usr/src/mxe/src
#COPY gcc-14.3.0.tar.xz /usr/src/mxe/pkg/
#
# Build MXE
#
RUN set -eux; \
  cd /usr/src/mxe && \
  make JOBS=$(nproc) && \
  #
  # Cleanup: By keeping the MXE build system (Makefile, ...), derived images will be able to install
  #          additional packages.
  #
  rm -rf log pkg && \
  #
  # Update MXE toolchain file
  #
  echo 'set(CMAKE_CROSSCOMPILING_EMULATOR "/usr/bin/wine")' >> ${CMAKE_TOOLCHAIN_FILE} && \
  echo 'set(CMAKE_CROSSCOMPILING_EMULATOR "/usr/bin/wine")' >> ${CMAKE_TOOLCHAIN_FILE2} && \
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
  ln -s /usr/src/mxe/usr/bin/${MXE_TARGET_ARCH}-w64-mingw32.${MXE_TARGET_LINK2}${MXE_TARGET_THREAD}-cmake cmake-static && \
  ln -s /usr/src/mxe/usr/bin/${MXE_TARGET_ARCH}-w64-mingw32.${MXE_TARGET_LINK}${MXE_TARGET_THREAD}-cpack cpack && \
  ln -s /usr/src/mxe/usr/bin/${MXE_TARGET_ARCH}-w64-mingw32.${MXE_TARGET_LINK2}${MXE_TARGET_THREAD}-cpack cpack-static && \
  #
  # clean up temporary wine files
  #
  rm -rf /tmp/wine-* && \
  #
  # create FlexLexer.h link (flex is for some unknown reason installed inside x86_64-pc-linux-gnu)
  #
  ln -s /usr/src/mxe/usr/x86_64-pc-linux-gnu/include/FlexLexer.h \
    /usr/src/mxe/usr/${MXE_TARGET_ARCH}-w64-mingw32.${MXE_TARGET_LINK}/include/ && \
  ln -s /usr/src/mxe/usr/x86_64-pc-linux-gnu/include/FlexLexer.h \
    /usr/src/mxe/usr/${MXE_TARGET_ARCH}-w64-mingw32.${MXE_TARGET_LINK2}/include/ && \
  # must correct include path for jsoncpp
  cd /usr/src/mxe/usr/${MXE_TARGET_ARCH}-w64-mingw32.${MXE_TARGET_LINK}/include && \
    ln -s . jsoncpp && \
  cd /usr/src/mxe/usr/${MXE_TARGET_ARCH}-w64-mingw32.${MXE_TARGET_LINK2}/include && \
    ln -s . jsoncpp

ENV PATH=${PATH}:/usr/src/mxe/usr/bin
# static qtnetworkauth
ENV CROSS_TRIPLE=${MXE_TARGET_ARCH}-w64-mingw32.${MXE_TARGET_LINK2}${MXE_TARGET_THREAD}
ENV AS=/usr/src/mxe/usr/bin/${CROSS_TRIPLE}-as \
    AR=/usr/src/mxe/usr/bin/${CROSS_TRIPLE}-ar \
    CC=/usr/src/mxe/usr/bin/${CROSS_TRIPLE}-gcc \
    CPP=/usr/src/mxe/usr/bin/${CROSS_TRIPLE}-cpp \
    CXX=/usr/src/mxe/usr/bin/${CROSS_TRIPLE}-g++ \
    LD=/usr/src/mxe/usr/bin/${CROSS_TRIPLE}-ld \
    FC=/usr/src/mxe/usr/bin/${CROSS_TRIPLE}-gfortran

# 2025-09 qt6-networkauth package is not provided
RUN cd /tmp && wget --no-check-certificate https://download.qt.io/archive/qt/6.9/6.9.2/submodules/qtnetworkauth-everywhere-src-6.9.2.tar.xz  && \
    tar xf qtnetworkauth-everywhere-src-6.9.2.tar.xz && \
    cd qtnetworkauth-everywhere-src-6.9.2 && mkdir build-static && \
    cd build-static && \
    cmake-static -DCMAKE_INSTALL_PREFIX:PATH=/usr/src/mxe/usr/${CROSS_TRIPLE}/qt6 .. && \
    make -j${nproc} && \
    make install

# shared qtnetworkauth
ENV CROSS_TRIPLE=${MXE_TARGET_ARCH}-w64-mingw32.${MXE_TARGET_LINK}${MXE_TARGET_THREAD}
ENV AS=/usr/src/mxe/usr/bin/${CROSS_TRIPLE}-as \
    AR=/usr/src/mxe/usr/bin/${CROSS_TRIPLE}-ar \
    CC=/usr/src/mxe/usr/bin/${CROSS_TRIPLE}-gcc \
    CPP=/usr/src/mxe/usr/bin/${CROSS_TRIPLE}-cpp \
    CXX=/usr/src/mxe/usr/bin/${CROSS_TRIPLE}-g++ \
    LD=/usr/src/mxe/usr/bin/${CROSS_TRIPLE}-ld \
    FC=/usr/src/mxe/usr/bin/${CROSS_TRIPLE}-gfortran

# 2025-09 qt6-networkauth package is not provided
RUN cd /tmp && \
    cd qtnetworkauth-everywhere-src-6.9.2 && mkdir build && \
    cd build && \
    cmake -DCMAKE_INSTALL_PREFIX:PATH=/usr/src/mxe/usr/${CROSS_TRIPLE}/qt6 .. && \
    make -j${nproc} && \
    make install && \
    rm -rf /tmp/qtnetworkauth*

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
