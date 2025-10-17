group "default" {
  targets = ["ebnf-latex"]
}

target "ebnf-latex" {
  context = "."
  dockerfile = "Dockerfile"
  tags = ["hub.semafor.ch/semafor/intens/ebnf-latex:latest"]
}
