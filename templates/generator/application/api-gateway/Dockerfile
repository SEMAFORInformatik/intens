FROM python:3.13-slim-bookworm AS builder
# Turns off writing .pyc files; superfluous on an ephemeral container.
ENV PYTHONDONTWRITEBYTECODE=1
# Seems to speed things up
ENV PYTHONUNBUFFERED=1

WORKDIR /app
RUN python -m venv /opt/venv
# Enable venv
ENV PATH="/opt/venv/bin:$PATH"

COPY ./requirements.txt .
RUN pip install  --no-cache-dir -r requirements.txt

FROM python:3.13-slim-bookworm AS runner
ARG REVISION
ENV API_GATEWAY_PORT=15558
ENV API_GATEWAY_LOGGER_PORT=15574
ENV WITHOUT_HTML_TAGS=1
WORKDIR /app
COPY --from=builder /opt/venv /opt/venv
COPY . .
RUN [ -z "$REVISION" ] ||echo "$REVISION" > vcs.info
ENV PATH="/opt/venv/bin:$PATH"

EXPOSE @@{API_GATEWAY_PORT} @@{API_GATEWAY_LOGGER_PORT}
ENTRYPOINT [ "python", "./api-gateway.py" ]
