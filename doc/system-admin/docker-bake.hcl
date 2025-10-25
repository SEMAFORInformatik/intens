group "default" {
  targets = ["ebnf-latex"]
}

target "ebnf-latex" {
  context = "."
  dockerfile = "Dockerfile"
  tags = ["ghcr.io/semaforinformatik/intens/ebnf-latex:latest"]
}
