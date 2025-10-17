# Dockerfile for system admin manual generation
FROM ubuntu:24.04
ENV AFMPATH=/usr/share/texlive/texmf-dist/fonts/afm
ENV RGBPATH=/usr/share/X11
ENV TEXINPUTS=/dirtree:

RUN set -eux; \
  apt-get update; \
  DEBIAN_FRONTEND=noninteractive \
  apt-get install -y --no-install-recommends \
    ghc libghc-old-time-dev happy wget unzip make \
    texlive-latex-extra texlive-luatex texlive-fonts-recommended latexmk; \
    rm -rf /var/lib/apt/lists/*; \
  wget --no-check-certificate https://github.com/FranklinChen/Ebnf2ps/archive/refs/tags/1.0.15.tar.gz; \
  tar xf 1.0.15.tar.gz; (cd Ebnf2ps-1.0.15; ghc Setup.hs; \
    ./Setup configure; ./Setup build; ./Setup install); \
  wget --no-check-certificate https://mirrors.ctan.org/macros/generic/dirtree.zip; \
    unzip dirtree; cd dirtree; tex dirtree.ins; \
  cd /usr/share/texlive/texmf-dist/fonts/afm; \
    ln -s adobe/times/ptmr8a.afm Times-Roman.afm; \
    ln -s adobe/helvetic/phvr8a.afm Helvetica.afm; \
    ln -s adobe/helvetic/phvb8a.afm Helvetica-Bold.afm
WORKDIR /work/doc/system-admin
