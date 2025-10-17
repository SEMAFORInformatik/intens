import vscode from "vscode"
import fs from "fs"
import path from 'path';
import { promisify } from "util";
import { spawn } from "child_process";

type GenType = "application" | "component" | "variant"

export default async () => {
  const genType = await vscode.window.showQuickPick(["application", "component", "variant"], {
    placeHolder: "Select application or component",
    title: "Intens application generator"
  }) as GenType;

  if (!genType) return;

  ({
    application: genApplication,
    component: genComponent,
    variant: genVariant
  }[genType]())
}

const genApplication = async () => {
  const identifier = await vscode.window.showInputBox({
    title: "Intens application identifier (without blanks)",
    placeHolder: "my-app"
  })

  if (!identifier) return;

  const name = await vscode.window.showInputBox({
    title: "Intens application name",
    value: identifier
  })

  if (!name) return;

  const shortname = await vscode.window.showInputBox({
    title: "Intens application shortname",
    value: name.toUpperCase(),
  })

  if (!shortname) return;


  let directory, proceed;

  do {
    directory = await vscode.window.showOpenDialog({
      canSelectFiles: false,
      canSelectFolders: true,
      canSelectMany: false,
      openLabel: "Select directory"
    })

    if (!directory) return;

    const isEmptyDir = (await promisify(fs.readdir)(directory[0].fsPath)).length === 0;

    if (!isEmptyDir) {
      proceed = await vscode.window.showQuickPick(["yes", "no"], {
        title: "Directory is not empty. Continue anyway?",
      })

      // escape == abort == undefined
      if (!proceed) return;

    } else {
      break;
    }
  } while (proceed === "no")

  const genPath = path.join(vscode.workspace.getConfiguration("intens").get("installation")!, "bin", "intens-gen")
  const result = spawn("python", [
    genPath,
    "--create", "application",
    "--identifier", identifier,
    "--name", name,
    "--shortName", shortname,
    "--destinationDirectory", directory[0].fsPath
  ])
  result.stdin.write("c\n")
}

const genComponent = async () => {
  const identifier = await vscode.window.showInputBox({
    title: "Identifier of the component (without blanks)",
    placeHolder: "mycomponent"
  })

  if (!identifier) return;

  const compType = await vscode.window.showInputBox({
    title: "component type (without blanks)",
    value: identifier.substring(0, 1).toUpperCase().concat(identifier.substring(1)),
  })

  if (!compType) return;

  const compTypeLabel = await vscode.window.showInputBox({
    title: "component type label",
    value: compType
  })

  if (!compTypeLabel) return;

  const compTypeLabelShort = await vscode.window.showInputBox({
    title: "component type label short name",
    value: compTypeLabel.substring(0, 2).toUpperCase()
  })

  if (!compTypeLabelShort) return;

  const genPath = path.join(vscode.workspace.getConfiguration("intens").get("installation")!, "bin", "intens-gen")
  spawn("python", [
    genPath,
    "--create", "component",
    "--identifier", identifier,
    "--componentType", compType,
    "--name", compTypeLabel,
    "--shortName", compTypeLabelShort,
    "--destinationDirectory", vscode.workspace.workspaceFolders?.[0]?.uri?.fsPath + ""
  ])
}
const genVariant = async () => {
  const identifier = await vscode.window.showInputBox({
    title: "Identifier of the variant (without blanks)",
    placeHolder: "myvariant"
  })

  if (!identifier) return;

  const compType = await vscode.window.showInputBox({
    title: "component type (without blanks)",
    value: identifier.substring(0, 1).toUpperCase().concat(identifier.substring(1)),
  })

  if (!compType) return;

  const varTypeLabel = await vscode.window.showInputBox({
    title: "variant type label",
    value: compType
  })

  if (!varTypeLabel) return;

  const varTypeLabelShort = await vscode.window.showInputBox({
    title: "variant type label short name",
    value: varTypeLabel.substring(0, 2).toUpperCase()
  })

  if (!varTypeLabelShort) return;

  const genPath = path.join(vscode.workspace.getConfiguration("intens").get("installation")!, "bin", "intens-gen")
  spawn("python", [
    genPath,
    "--create", "component",
    "--identifier", identifier,
    "--componentType", compType,
    "--name", varTypeLabel,
    "--shortName", varTypeLabelShort,
    "--destinationDirectory", vscode.workspace.workspaceFolders?.[0]?.uri?.fsPath + ""
  ])
}
