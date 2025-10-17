(gen_comment) @comment
(gen_preprocessor) @keyword.directive
(gen_real_or_int_value) @constant.numeric.integer
(gen_quote_string) @string
(gen_translated_string ["_(" ")"] @type.builtin)
(gen_identifier) @variable.other.member

[
% for a in builtin:
"${a}"
% endfor
] @function.builtin

[
% for a in operator:
"${a}"
% endfor
] @operator

[
% for a in bracket:
"${a}"
% endfor
] @punctuation.bracket

[
% for a in delimiter:
"${a}"
% endfor
] @punctuation.delimiter


[
% for a in type:
"${a}"
% endfor
] @type.builtin


[
% for a in keyword:
"${a}"
% endfor
] @keyword

