variable "TAG_NAME" {
  default = "latest"
}

group "all" {
 targets = ["default", "dev"]
}

target "default" {
  context = "."
  dockerfile = "docker/Dockerfile"
  args = {
    CMAKE_BUILD_TYPE = "Release"
  }
  tags = ["hub.semafor.ch/semafor/intens:${TAG_NAME}", "hub.semafor.ch/semafor/intens:latest"]
  # cache-to = ["type=registry,image-manifest=true,ref=hub-cache.semafor.ch/semafor/intens:latest"]
  # cache-from = ["type=registry,image-manifest=true,ref=hub-cache.semafor.ch/semafor/intens:latest"]
}

target "dev" {
  context = "."
  dockerfile = "docker/Dockerfile"
  args = {
    CMAKE_BUILD_TYPE = "Debug"
    EXTRA_PACKAGES = "gdb"
  }
  tags = ["hub.semafor.ch/semafor/intens/dev:${TAG_NAME}", "hub.semafor.ch/semafor/intens/dev:latest"]
  # cache-to = ["type=registry,image-manifest=true,ref=hub-cache.semafor.ch/semafor/intens/dev:latest"]
  # cache-from = ["type=registry,image-manifest=true,ref=hub-cache.semafor.ch/semafor/intens/dev:latest"]
}

target "citest" {
  context = "."
  dockerfile = "docker/Dockerfile"
  args = {
    CMAKE_BUILD_TYPE = "Debug"
    EXTRA_PACKAGES = "make cmake g++ python3-venv"
  }
  tags = ["hub.semafor.ch/semafor/intens/citest:${TAG_NAME}", "hub.semafor.ch/semafor/intens/citest:latest"]
  # cache-to = ["type=registry,image-manifest=true,ref=hub-cache.semafor.ch/semafor/intens/citest:latest"]
  # cache-from = ["type=registry,image-manifest=true,ref=hub-cache.semafor.ch/semafor/intens/citest:latest"]
}
