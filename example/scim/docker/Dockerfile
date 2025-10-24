# Dockerfile for scim
FROM ghcr.io/semaforinformatik/intens:latest AS build
ENV QT_QPA_PLATFORM=minimal
RUN apt-get update && DEBIAN_FRONTEND=noninteractive apt-get install -y --no-install-recommends \
    build-essential cmake gettext python3-venv; \
    python3 -m venv /opt/venv
# Enable venv
ENV PATH="/opt/venv/bin:$PATH"
RUN pip install mako

WORKDIR /work
COPY . .
RUN cmake . && cmake --build . && cmake --install . --prefix /usr/local

FROM ghcr.io/semaforinformatik/intens:latest AS runtime
WORKDIR /opt/app
ENV IS_WEBTENS=1
ENV INTENS_REPLY_PORT=15560
ENV DB_HOST=db
ENV APPNAME=scim
ENV USERDIR=scim
COPY --from=build /usr/local/etc/ etc/
COPY --from=build /usr/local/bitmaps/ bitmaps/
COPY --from=build /usr/local/config/ config/
COPY --from=build /usr/local/bin/ bin/
