variable "GITHUB_REF_NAME" {
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
  tags = ["ghcr.io/semaforinformatik/intens:${GITHUB_REF_NAME}", "ghcr.io/semaforinformatik/intens:latest"]
  cache-from = ["type=gha,scope=intens"]
  cache-to = ["type=gha,mode=max,scope=intens"]
}

target "dev" {
  context = "."
  dockerfile = "docker/Dockerfile"
  args = {
    CMAKE_BUILD_TYPE = "Debug"
    EXTRA_PACKAGES = "gdb"
  }
  tags = ["ghcr.io/semaforinformatik/intens/dev:${GITHUB_REF_NAME}", "ghcr.io/semaforinformatik/intens/dev:latest"]
  cache-from = ["type=gha,scope=intens-dev"]
  cache-to = ["type=gha,mode=max,scope=intens-dev"]
}

target "citest" {
  context = "."
  dockerfile = "docker/Dockerfile"
  args = {
    CMAKE_BUILD_TYPE = "Debug"
    EXTRA_PACKAGES = "make cmake g++ python3-venv"
  }
  tags = ["ghcr.io/semaforinformatik/intens/citest:${GITHUB_REF_NAME}", "ghcr.io/semaforinformatik/intens:latest"]
}

target "dockcross" {
  context = "dockcross"
  dockerfile = "Dockerfile"
  tags = ["ghcr.io/semaforinformatik/intens/dockcross:latest"]
}
