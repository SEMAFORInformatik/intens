import vscode from 'vscode';
// import { Parser, Language, Tree, Node } from 'web-tree-sitter';
import path from 'path';
import intens from "./intens.json"
import intensGrammar from "./intens.tmGrammar.json"
import {
  LanguageClient,
  LanguageClientOptions,
  ServerOptions,
} from 'vscode-languageclient/node';
import intensGen from './intensGen';

let client: LanguageClient;
// Grammar class
// const parserPromise = Parser.init();
// class Grammar {
//   // Parser
//   parser?: Parser;
//   // Grammar
//   readonly simpleTerms: { [sym: string]: string } = {};

//   // Constructor
//   constructor() {
//     this.simpleTerms = intens;
//   }

//   // Parser initialization
//   async init() {
//     // Load wasm parser
//     await parserPromise;
//     this.parser = new Parser();
//     let langFile = path.join(__dirname, "tree-sitter-intens.wasm");
//     const langObj = await Language.load(langFile);
//     this.parser.setLanguage(langObj);
//   }

//   // Build syntax tree
//   tree(doc: string) {
//     if (!this.parser) throw new Error("Parser not initialized");
//     const tree = this.parser!.parse(doc);
//     return tree;
//   }

//   // Parse syntax tree
//   parse(tree: Tree) {
//     // Travel tree and peek terms
//     let terms: { term: string; range: vscode.Range }[] = [];
//     let stack: Node[] = [];
//     let node = tree.rootNode.firstChild;
//     while (stack.length > 0 || node) {
//       // Get every child onto the stack recursively
//       if (node) {
//         stack.push(node);
//         node = node.firstChild;
//         continue
//       }

//       node = stack.pop() || null;
//       if (!node) continue;
//       let type = node.type;
//       if (!node.isNamed)
//         type = '"' + type + '"';

//       // Simple one-level terms
//       const term = this.simpleTerms[type];

//       // If term is found add it
//       if (term) {
//         terms.push({
//           term: term,
//           range: new vscode.Range(
//             new vscode.Position(
//               node.startPosition.row,
//               node.startPosition.column),
//             new vscode.Position(
//               node.endPosition.row,
//               node.endPosition.column))
//         });
//       }

//       // Go to the next sibling to continue pushing every child there onto the stack
//       node = node.nextSibling
//     }
//     return terms;
//   }
// }

// Semantic token legend
const termMap = new Map<string, { type: string, modifiers?: string[] }>();
function buildLegend() {
  // Terms vocabulary
  termMap.set("type", { type: "type" });
  termMap.set("scope", { type: "namespace" });
  termMap.set("function", { type: "function" });
  termMap.set("variable", { type: "variable" });
  termMap.set("number", { type: "number" });
  termMap.set("string", { type: "string" });
  termMap.set("comment", { type: "comment" });
  termMap.set("constant", { type: "variable", modifiers: ["readonly", "defaultLibrary"] });
  termMap.set("directive", { type: "macro" });
  termMap.set("control", { type: "keyword" });
  termMap.set("operator", { type: "operator" });
  termMap.set("modifier", { type: "type", modifiers: ["modification"] });
  termMap.set("punctuation", { type: "punctuation" });
  // Tokens and modifiers in use
  let tokens: string[] = [];
  let modifiers: string[] = [];
  termMap.forEach(t => {
    if (!tokens.includes(t.type))
      tokens.push(t.type);
    t.modifiers?.forEach(m => {
      if (!modifiers.includes(m))
        modifiers.push(m);
    });
  });
  // Construct semantic token legend
  return new vscode.SemanticTokensLegend(tokens, modifiers);
}
const legend = buildLegend();

// Semantic token provider
// class TokensProvider implements vscode.DocumentSemanticTokensProvider {
//   grammar: Grammar | null = null;
//   readonly trees: { [doc: string]: Tree } = {};

//   // Provide document tokens
//   async provideDocumentSemanticTokens(
//     doc: vscode.TextDocument,
//     token: vscode.CancellationToken): Promise<vscode.SemanticTokens> {
//     // Grammar
//     if (!this.grammar) {
//       this.grammar = new Grammar();
//       await this.grammar.init();
//     }
//     // Parse document
//     const grammar = this.grammar;
//     const tree = grammar.tree(doc.getText());
//     const terms = grammar.parse(tree!);
//     this.trees[doc.uri.toString()] = tree!;
//     // Build tokens
//     const builder = new vscode.SemanticTokensBuilder(legend);
//     terms.forEach((t) => {
//       const type = termMap.get(t.term)?.type;
//       if (!type) return;
//       const modifiers = termMap.get(t.term)?.modifiers;
//       if (t.range.start.line === t.range.end.line)
//         return builder.push(t.range, type, modifiers);
//       let line = t.range.start.line;
//       builder.push(new vscode.Range(t.range.start,
//         doc.lineAt(line).range.end), type, modifiers);
//       for (line = line + 1; line < t.range.end.line; line++)
//         builder.push(doc.lineAt(line).range, type, modifiers);
//       builder.push(new vscode.Range(doc.lineAt(line).range.start,
//         t.range.end), type, modifiers);
//     });
//     return builder.build();
//   }
// }

// Extension activation
export async function activate(context: vscode.ExtensionContext) {
  // const engine = new TokensProvider();
  // context.subscriptions.push(
  //   vscode.languages.registerDocumentSemanticTokensProvider(
  //     { language: "intens", scheme: "file" }, engine, legend));

  const executable = "intens-language-server" + (process.platform === "win32" ? ".exe" : "")

  const inPath = path.join(vscode.workspace.getConfiguration("intens").get("installation")!, "bin")
  const serverOptions: ServerOptions = {
    command: path.join(inPath, executable)
  };

  // Options to control the language client
  const clientOptions: LanguageClientOptions = {
    // Register the server for plain text documents
    documentSelector: [{ scheme: "file", language: "intens" }],
  };

  // Create the language client and start the client.
  client = new LanguageClient(
    "languageServerIntens",
    "Intens Language Server",
    serverOptions,
    clientOptions
  );

  // Start the client. This will also launch the server
  client.start();

  context.subscriptions.push(vscode.commands.registerCommand("intens.gen", intensGen))
}

export function deactivate(): Thenable<void> | undefined {
  if (!client) {
    return undefined;
  }
  return client.stop();
}
