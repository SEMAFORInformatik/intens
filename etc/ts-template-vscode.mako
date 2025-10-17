{
    "gen_preprocessor": "macro",
    "gen_real_or_int_value": "number",
    "gen_quote_string": "string",
    "gen_translated_string": "",
    "gen_identifier": "variable",
% for a in builtin:
"\"${a}\"": "function",
% endfor

% for a in operator:
"\"${a}\"": "operator",
% endfor

% for a in bracket:
"\"${a}\"": "punctuation",
% endfor

% for a in delimiter:
"\"${a}\"": "punctuation",
% endfor


% for a in type:
"\"${a}\"": "type",
% endfor


% for a in keyword:
"\"${a}\"": "constant",
% endfor
    "gen_comment": "comment"
}
