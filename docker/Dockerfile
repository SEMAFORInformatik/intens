# Dockerfile for semafor/intens
FROM ubuntu:24.04 AS builder
ARG CMAKE_BUILD_TYPE=Release
ARG nproc=8

RUN set -eux; \
  apt-get update; \
  DEBIAN_FRONTEND=noninteractive \
  apt-get install -y --no-install-recommends \
    build-essential \
    cmake \
    libprotobuf-dev \
    protobuf-compiler \
    pkg-config \
    libssl-dev \
    libnsl-dev \
    libfl-dev \
    bison \
    gettext \
    libcurl4-openssl-dev \
    libxml2-dev \
    libxslt-dev \
    libjsoncpp-dev \
    liblog4cplus-dev \
    cppzmq-dev \
    libltdl-dev \
    qt6-base-dev \
    qt6-5compat-dev \
    qt6-svg-dev \
    qt6-networkauth-dev \
    qt6-charts-dev \
    wget \
    libgtest-dev \
    zlib1g-dev \
    libxkbcommon-dev

# 2025-05 qwt-qt6 package are not provided
WORKDIR /usr/local
RUN wget --no-check-certificate https://deac-riga.dl.sourceforge.net/project/qwt/qwt/6.3.0/qwt-6.3.0.tar.bz2 && \
    tar xfj qwt-6.3.0.tar.bz2 && \
    cd qwt-6.3.0 && qmake6 qwt.pro && make -j${nproc} && \
    cp ./lib/libqwt.so* /usr/lib/ && mkdir -p /usr/include/qwt/ && cp ./src/*.h /usr/include/qwt/
#
COPY . /root/intens/
WORKDIR /root/intens/build
RUN cmake -DCMAKE_BUILD_TYPE=${CMAKE_BUILD_TYPE} .. && \
  nice make -j${nproc} && make install && \
  ln -s /usr/local/intens/* /usr/local/intens/current && \
  for f in /usr/local/intens/current/bin/*; do ln -s $f /usr/local/bin/; done
#
# copy required shared libs
RUN  mkdir lib; \
  for l in $(ldd /usr/local/intens/current/bin/intens | egrep -o '/lib.*\.[0-9a-z]+'); \
    do cp --parents "$l" .; done; \
  platform_dir=/usr/lib/x86_64-linux-gnu/qt5/plugins/platforms; \
  for p in minimal xcb; \
  do cp --parents $platform_dir/libq${p}.so .; \
    for l in $(ldd $platform_dir/libq${p}.so | egrep -o '/lib.*\.[0-9a-z]+'); \
      do cp --parents "$l" .; \
    done; \
  done
###########
# RUNTIME #
###########
FROM ubuntu:24.04 AS runtime
LABEL maintainer="Ronald Tanner <tar@semafor.ch>"
LABEL org.label-schema.vendor="Semafor Informatik & Energie AG"
LABEL org.label-schema.url="https://semafor.ch"
ARG EXTRA_PACKAGES=""

RUN set -eux; \
  apt-get update; \
  DEBIAN_FRONTEND=noninteractive \
  apt-get install -y --no-install-recommends \
    ca-certificates \
    gettext \
    locales \
    libqt6gui6 \
    libqt6svg6 \
    libqt6printsupport6 \
    libqt6charts6 \
    libqt6networkauth6 \
    libqt6svgwidgets6 \
    libcurl4 \
    libzmq5 \
    libltdl7 \
    libxml2 \
    libxslt1.1 \
    liblog4cplus-2.0.5t64 $EXTRA_PACKAGES; \
  echo "en_US.UTF-8 UTF-8"> /etc/locale.gen;\
  echo "de_CH.UTF-8 UTF-8">> /etc/locale.gen;\
  echo "de_DE.UTF-8 UTF-8">> /etc/locale.gen;\
  echo "de_AT.UTF-8 UTF-8">> /etc/locale.gen;\
  echo "fr_FR.UTF-8 UTF-8">> /etc/locale.gen;\
  echo "es_ES.UTF-8 UTF-8">> /etc/locale.gen;\
  echo "sv_SE.UTF-8 UTF-8">> /etc/locale.gen;\
  locale-gen; \
  rm -rf /var/lib/apt/lists/*

COPY --from=builder /usr/local/intens /usr/local/intens
COPY --from=builder /root/intens/build/lib/ /tmp/lib
COPY --from=builder /root/intens/build/usr/ /tmp/usr
RUN ln -s /usr/local/intens/* /usr/local/intens/current
ENV QT_QPA_PLATFORM=offscreen

RUN for l in $(find /tmp/lib -type f); \
  do dest=$(echo $l |sed s@/tmp@@); [ -f $dest ] || cp $l $dest; done; rm -fr /tmp/lib; \
  for l in $(find /tmp/usr -type f); \
  do dest=$(echo $l |sed s@/tmp@@); dir=$(dirname $dest);\
    [ -f $dest ] || (mkdir -p $dir; cp $l $dest); done; rm -fr /tmp/usr
ENV PATH=/usr/local/intens/current/bin:$PATH
ENV INTENS_HOME=/usr/local/intens/current

ENV APPHOME=/opt/app
ENV IS_WEBTENS=1

# add user and group intens

# ubuntu:23.10 creates a user and group ubuntu (1000:1000)
# rename them to intens (as we need 1000:1000 to be intens)
RUN groupmod --new-name intens ubuntu; \
  usermod --login=intens --home=/home/intens --move-home --comment=Intens --shell=/bin/bash ubuntu

# Configfile for dummy xserver
COPY docker/docker-entrypoint.sh /

WORKDIR ${APPHOME}

EXPOSE 15560 15561 15562

ENTRYPOINT [ "/docker-entrypoint.sh" ]
CMD [ "sh", "-c", "bash ./bin/${APPNAME}" ]
