# https://docs.astral.sh/uv/guides/integration/docker/#caching
FROM python:3.13-slim-bookworm
COPY --from=ghcr.io/astral-sh/uv:latest /uv /uvx /bin/

ARG REVISION
ENV API_GATEWAY_PORT=15558
ENV API_GATEWAY_LOGGER_PORT=15574
ENV WITHOUT_HTML_TAGS=1
WORKDIR /app
# Install dependencies
RUN --mount=type=cache,target=/root/.cache/uv \
    --mount=type=bind,source=uv.lock,target=uv.lock \
    --mount=type=bind,source=pyproject.toml,target=pyproject.toml \
    uv sync --locked --no-install-project
COPY . .
# Sync the project
RUN --mount=type=cache,target=/root/.cache/uv \
    uv sync --locked

RUN [ -z "$REVISION" ] ||echo "$REVISION" > vcs.info

EXPOSE ${API_GATEWAY_PORT} ${API_GATEWAY_LOGGER_PORT}
ENTRYPOINT [ "uv", "run", "api-gateway.py" ]
