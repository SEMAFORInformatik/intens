variable "TAG_NAME" {
  default = "latest"
}

group "default" {
 targets = ["${application}", "api-gateway"]
}

target "${application}" {
  dockerfile = "docker/Dockerfile"
  tags = ["hub.semafor.ch/semafor/${application}:@@{TAG_NAME}", "hub.semafor.ch/semafor/${application}:latest"]
  cache-to = ["type=registry,image-manifest=true,ref=hub-cache.semafor.ch/semafor/${application}:latest"]
  cache-from = ["type=registry,image-manifest=true,ref=hub-cache.semafor.ch/semafor/${application}:latest"]
}

target "api-gateway" {
  context = "api-gateway"
  dockerfile = "Dockerfile"
  tags = ["hub.semafor.ch/semafor/${application}/api-gateway:@@{TAG_NAME}", "hub.semafor.ch/semafor/${application}/api-gateway:latest"]
  cache-to = ["type=registry,image-manifest=true,ref=hub-cache.semafor.ch/semafor/${application}/api-gateway:latest"]
  cache-from = ["type=registry,image-manifest=true,ref=hub-cache.semafor.ch/semafor/${application}/api-gateway:latest"]
}

