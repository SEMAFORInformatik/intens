#!/usr/bin/env node

const util = require("node:util")
const { existsSync, mkdirSync } = require("fs")
const exec = util.promisify(require("node:child_process").exec);

const grammarFoler = "../tree-sitter-intens"
const outFolder = "out"
const output = `${outFolder}/tree-sitter-intens.wasm`

if (!existsSync(output)) {
  mkdirSync(outFolder, { recursive: true })

  console.log("Compiling intens parser");

  exec(`${__dirname}/../node_modules/.bin/tree-sitter generate && ${__dirname}/../node_modules/.bin/tree-sitter build --wasm -o ../vscode/${output}`,
    { cwd: grammarFoler }).catch(err => {
      console.log("Failed to build wasm", err)
    })
}

