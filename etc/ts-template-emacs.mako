(defvar intens-ts-font-lock-rules-loaded
  '(
    :language intens
    :override t
    :feature variable
    ((gen_identifier) @font-lock-variable-name-face)

    :language intens
    :override t
    :feature number
    ((gen_real_or_int_value) @font-lock-number-face)

    :language intens
    :override t
    :feature preprocessor
    ((gen_preprocessor) @font-lock-preprocessor-face)

    :language intens
    :override t
    :feature operator
    ([
      % for a in operator:
      "${a}"
      % endfor
    ] @font-lock-operator-face)

    :language intens
    :override t
    :feature bracket
    ([
      % for a in bracket:
      "${a}"
      % endfor
    ] @font-lock-bracket-face)

    :language intens
    :override t
    :feature delimiter
    ([
      % for a in delimiter:
      "${a}"
      % endfor
    ] @font-lock-delimiter-face)

    :language intens
    :override t
    :feature builtin-function
    ([
      % for a in builtin:
      "${a}"
      % endfor
    ] @font-lock-builtin-face)

    :language intens
    :override t
    :feature type
    ([
      % for a in type:
      "${a}"
      % endfor
    ] @font-lock-type-face)

    :language intens
    :override t
    :feature constant
    ([
      % for a in constant:
      "${a}"
      % endfor
    ] @font-lock-constant-face)

    :language intens
    :override t
    :feature keyword
    ([
      % for a in keyword:
      "${a}"
      % endfor
    ] @font-lock-keyword-face)

    :language intens
    :override t
    :feature comment
    ((gen_comment) @font-lock-comment-face)

    :language intens
    :override t
    :feature string
    ((gen_quote_string) @font-lock-string-face)
    )
)

