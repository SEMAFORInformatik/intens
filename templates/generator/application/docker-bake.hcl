variable "TAG_NAME" {
  default = "latest"
}

group "default" {
 targets = ["${application}", "api-gateway"]
}

target "${application}" {
  dockerfile = "docker/Dockerfile"
  tags = ["ghcr.io/semaforinformatik/${application}:@@{TAG_NAME}", "ghcr.io/semaforinformatik/${application}:latest"]
}

target "api-gateway" {
  context = "api-gateway"
  dockerfile = "Dockerfile"
  tags = ["ghcr.io/semaforinformatik/${application}/api-gateway:@@{TAG_NAME}", "ghcr.io/semaforinformatik/${application}/api-gateway:latest"]
}

