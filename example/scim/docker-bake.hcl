variable "TAG_NAME" {
  default = "latest"
}

group "default" {
 targets = ["scim", "api-gateway"]
}

target "scim" {
  dockerfile = "docker/Dockerfile"
  tags = ["hub.semafor.ch/semafor/scim:${TAG_NAME}", "hub.semafor.ch/semafor/scim:latest"]
  cache-to = ["type=registry,image-manifest=true,ref=hub-cache.semafor.ch/semafor/scim:latest"]
  cache-from = ["type=registry,image-manifest=true,ref=hub-cache.semafor.ch/semafor/scim:latest"]
}

target "api-gateway" {
  context = "api-gateway"
  dockerfile = "Dockerfile"
  tags = ["hub.semafor.ch/semafor/scim/api-gateway:${TAG_NAME}", "hub.semafor.ch/semafor/scim/api-gateway:latest"]
  cache-to = ["type=registry,image-manifest=true,ref=hub-cache.semafor.ch/semafor/scim/api-gateway:latest"]
  cache-from = ["type=registry,image-manifest=true,ref=hub-cache.semafor.ch/semafor/scim/api-gateway:latest"]
}

