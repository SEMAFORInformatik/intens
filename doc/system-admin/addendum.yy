/* identifier */
IDENTIFIER /* DOCUMENTATION:DIAGRAM */
  : "letter" opt_identifier_character_list
  ;

opt_identifier_character_list /* DOCUMENTATION:UNFOLD */
  : opt_identifier_character
  | opt_identifier_character_list opt_identifier_character
  ;

opt_identifier_character /* DOCUMENTATION:UNFOLD */
  :
  | "letter"
  | "digit"
  | "#"
  | "_"
  ;

/* string */
STRING_CONSTANT /* DOCUMENTATION:DIAGRAM */
  : '"' opt_character_list '"'
  | "_(" '"' character_list '"' ")"
  | "RESOURCE(" '"' character_list '"' ")"
  ;

opt_character_list /* DOCUMENTATION:UNFOLD */
  :
  | character_list
  ;

character_list /* DOCUMENTATION:UNFOLD */
  :  character
  | character_list character
  ;

/* integer */
INT_CONSTANT /* DOCUMENTATION:UNFOLD */
  : integer
  ;
integer /* DOCUMENTATION:DIAGRAM */
  : "digit" opt_integer_character_list
  | "RESOURCE(" '"' character_list '"' ")"
  ;
opt_integer_character_list /* DOCUMENTATION:UNFOLD */
  : opt_integer_character
  | opt_integer_character_list opt_integer_character
  ;

opt_integer_character /* DOCUMENTATION:UNFOLD */
  :
  | "digit"
  ;

/* multifont_string */
multifont_string /* DOCUMENTATION:DIAGRAM */
  : "\"" opt_multifont_list "\""
  ;

opt_multifont_list /* DOCUMENTATION:UNFOLD */
  : opt_multifont
  | opt_multifont_list opt_multifont
  ;

opt_multifont /* DOCUMENTATION:UNFOLD */
  :
  | character_list
  | "@" fontnames "@"
  ;

fontnames /* DOCUMENTATION:UNFOLD */
  : P
  | fontname
  ;

/* wildcard */
wildcard /* DOCUMENTATION:DIAGRAM */
  : "*"
  | "#"
  ;
