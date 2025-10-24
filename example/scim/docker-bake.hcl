variable "TAG_NAME" {
  default = "latest"
}

group "default" {
 targets = ["scim", "api-gateway"]
}

target "scim" {
  dockerfile = "docker/Dockerfile"
  tags = ["ghcr.io/semaforinformatik/scim:${TAG_NAME}", "ghcr.io/semaforinformatik/scim:latest"]
}

target "api-gateway" {
  context = "api-gateway"
  dockerfile = "Dockerfile"
  tags = ["ghcr.io/semaforinformatik/scim/api-gateway:${TAG_NAME}", "ghcr.io/semaforinformatik/scim/api-gateway:latest"]
}

