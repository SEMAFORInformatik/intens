{
  "$schema": "https://raw.githubusercontent.com/martinring/tmlanguage/master/tmlanguage.json",
  "name": "Intens",
  "patterns": [
    {
      "include": "#comment"
    },
    {
      "include": "#preprocessor"
    },
    {
      "include": "#operators"
    },
    {
      "include": "#builtins"
    },
    {
      "include": "#identifier"
    },
    {
      "include": "#strings"
    },
    {
      "name": "constant.numeric.intens",
      "match": "[eE]?[+-]?[[:digit:]]+(\\.[[:digit:]]*)?"
    }
  ],
  "repository": {
    "operators": {
      "patterns": [
        {
          "match": ">=",
          "name": "keyword.operator.intens"
        },
        {
          "match": ">",
          "name": "keyword.operator.intens"
        },
        {
          "match": "<",
          "name": "keyword.operator.intens"
        },
        {
          "match": "<=",
          "name": "keyword.operator.intens"
        },
        {
          "match": "!=",
          "name": "keyword.operator.intens"
        },
        {
          "match": "!",
          "name": "keyword.operator.intens"
        },
        {
          "match": "\\|\\|",
          "name": "keyword.operator.intens"
        },
        {
          "match": "&&",
          "name": "keyword.operator.intens"
        },
        {
          "match": "&",
          "name": "keyword.operator.intens"
        },
        {
          "match": "\\+\\+",
          "name": "keyword.operator.intens"
        },
        {
          "match": "\\+",
          "name": "keyword.operator.intens"
        },
        {
          "match": "\\-\\-",
          "name": "keyword.operator.intens"
        },
        {
          "match": "\\-",
          "name": "keyword.operator.intens"
        },
        {
          "match": "\\+=",
          "name": "keyword.operator.intens"
        },
        {
          "match": "=",
          "name": "keyword.operator.intens"
        },
        {
          "match": "\\*",
          "name": "keyword.operator.intens"
        },
        {
          "match": "/",
          "name": "keyword.operator.intens"
        },
        {
          "match": "{",
          "name": "keyword.operator.intens"
        },
        {
          "match": "}",
          "name": "keyword.operator.intens"
        },
        {
          "match": "\\(",
          "name": "keyword.operator.intens"
        },
        {
          "match": "\\)",
          "name": "keyword.operator.intens"
        },
        {
          "match": "\\[",
          "name": "keyword.operator.intens"
        },
        {
          "match": "\\]",
          "name": "keyword.operator.intens"
        },
        {
          "match": ",",
          "name": "keyword.operator.intens"
        },
        {
          "match": "\\.",
          "name": "keyword.operator.intens"
        },
        {
          "match": ";",
          "name": "keyword.operator.intens"
        }
      ]
    },
    "builtins": {
      "patterns": [
% for a in builtin:
        {
          "match": "${a}",
          "name": "entity.name.function.intens"
        },
% endfor

% for a in type:
        {
          "match": "${a}",
          "name": "storage.type.intens"
        },
% endfor

% for a in constant:
        {
          "match": "${a}",
          "name": "constant.language.intens"
        },
% endfor

% for a in keyword:
        {
          "match": "${a}",
          "name": "constant.language.intens"
        },
% endfor
        {
        }
      ]
    },
    "strings": {
      "name": "string.quoted.double.intens",
      "begin": "\"",
      "end": "\"",
      "patterns": [
        {
          "name": "constant.character.escape.intens",
          "match": "\\."
        }
      ]
    },
    "comment": {
      "name": "comment.line.double-slash.intens",
      "begin": "//",
      "end": "$"
    },
    "identifier": {
      "patterns": [
        {
          "name": "variable.other",
          "match": "[a-zA-Z][a-zA-Z_0-9\\.]*"
        }
      ]
    },
    "preprocessor": {
      "patterns": [
        {
          "name": "keyword.control.directive",
          "begin": "^#",
          "end": "$"
        }
      ]
    }
  },
  "scopeName": "source.intens"
}
