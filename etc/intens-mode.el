;;; intens-mode-el -- Major mode for editing Intens Description files

;; Author: Simon Bader <bader@semafor.ch>
;; Created: 28 Jan 2008
;; Keywords: INTENS major-mode

;;; Copyright (C) 2008-2021, SEMAFOR Informatik und Energie AG <info@semafor.ch>
;;; SPDX-FileCopyrightText: 2025 SEMAFOR Informatik & Energie AG, Basel
;;;
;;; SPDX-License-Identifier: Apache-2.0

;; This program is free software; you can redistribute it and/or
;; modify it under the terms of the GNU General Public License as
;; published by the Free Software Foundation; either version 2 of
;; the License, or (at your option) any later version.

;; This program is distributed in the hope that it will be
;; useful, but WITHOUT ANY WARRANTY; without even the implied
;; warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
;; PURPOSE.  See the GNU General Public License for more details.

;; You should have received a copy of the GNU General Public
;; License along with this program; if not, write to the Free
;; Software Foundation, Inc., 59 Temple Place, Suite 330, Boston,
;; MA 02111-1307 USA



;; You may wish to add something like the following to your ~/.emacs file
;; (add-to-list 'load-path "/home/bas/projekte/intens/etc") ;;<- adapt path!!
;; (require 'intens-mode)
;; (add-hook 'intens-mode 'turn-on-font-lock)

;;; Code:

(defvar intens-mode-hook 'turn-on-font-lock)
(defvar intens-mode-map
  (let ((intens-mode-map (make-sparse-keymap)))
    (define-key intens-mode-map [(control c) (control c)]
      'comment-region
      )
    intens-mode-map)
  "Keymap for INTENS major mode")

;; Automatically use intens-mode for .des, .des.in
(add-to-list 'auto-mode-alist '("\\.des\\'" . intens-mode))
(add-to-list 'auto-mode-alist '("\\.des.in\\'" . intens-mode))

(with-eval-after-load 'eglot
  (add-to-list 'eglot-server-programs
               '(intens-mode . ("intens-language-server"))))

(add-hook 'intens-mode-hook 'eglot-ensure)

;; font-lock-function-name-face
;; (regexp-opt '(
;;              "DATAPOOL"
;;              "DB_MANAGER"
;;              "DESCRIPTION"
;;              "END"
;;              "FUNCTIONS"
;;              "HELPFILE"
;;              "LANGUAGE"
;;              "OPERATOR"
;;              "STREAMER"
;;              "UI_MANAGER"
;;              ) t)

;; font-lock-keyword-face
;; (regexp-opt '(
;;              "ABORT"
;;              "ABORTTRANSACTION"
;;              "ABS"
;;              "ACOS"
;;              "AFTER_DB_LOGON"
;;              "AFTER_UPDATE_FORMS"
;;              "ALARM_COLOR"
;;              "ALARM_LEVEL"
;;              "ALIGN"
;;              "ALLCYCLES"
;;              "ALLOW"
;;              "ALWAYS"
;;              "ANNOTATION"
;;              "APP_MODAL"
;;              "APP_VERSION_MAJOR"
;;              "APP_VERSION_MINOR"
;;              "APP_VERSION_PATCH"
;;              "APPEND"
;;              "AREA"
;;              "ARG"
;;              "ARROWS"
;;              "ASCII"
;;              "ASIN"
;;              "ASPECT_RATIO"
;;              "ASPECT_RATIO_REF_AXIS"
;;              "ASSIGN_CONSISTENCY"
;;              "ASSIGN_CORR"
;;              "ATAN"
;;              "ATAN2"
;;              "ATTRS"
;;              "AUTO_SCROLL"
;;              "AUTO_WIDTH"
;;              "AUTOCLEAR_DEPENDENCIES"
;;              "AUTOLEVEL"
;;              "AVG"
;;              "AXES_ORIGIN"
;;              "AXES_ORIGIN_X"
;;              "AXES_ORIGIN_Y"
;;              "BAR"
;;              "BASE"
;;              "BASENAME"
;;              "BATCH"
;;              "BEEP"
;;              "BEGINTRANSACTION"
;;              "BINARY"
;;              "BOTTOM"
;;              "BUTTON"
;;              "BUTTON_CANCEL"
;;              "BUTTON_NO"
;;              "BUTTON_YES"
;;              "BUTTONS"
;;              "BY"
;;              "CAPTION"
;;              "CELL"
;;              "CENTER"
;;              "CHANGED"
;;              "CLASS"
;;              "CLASSNAME"
;;              "CLEAR"
;;              "CLEAR_SELECTION"
;;              "CLEARCYCLE"
;;              "CLOSE_BUTTON"
;;              "COL"
;;              "COLOR_SCALE"
;;              "COLORBIT"
;;              "COLSPAN"
;;              "COMBOBOX"
;;              "COMMITTRANSACTION"
;;              "COMPARE"
;;              "COMPOSE"
;;              "COMPOSE_STRING"
;;              "CONFIRM"
;;              "CONFIRM_CANCEL"
;;              "CONTOUR"
;;              "COPY"
;;              "COS"
;;              "CURRENT_DATE"
;;              "CURRENT_DATETIME"
;;              "CURRENT_TIME"
;;              "CYCLE"
;;              "CYCLENAME"
;;              "DAEMON"
;;              "DATA"
;;              "DATA_SIZE"
;;              "DATASET_TEXT"
;;              "DATE"
;;              "DB_VERSION_IGNORE"
;;              "DB_VERSION_MAJOR"
;;              "DB_VERSION_MINOR"
;;              "DB_VERSION_PATCH"
;;              "DBASSEMBLY"
;;              "DBATTR"
;;              "DBLIST"
;;              "DBUNIT"
;;              "DEADLY_SILENT"
;;              "DEBUG"
;;              "DEFAULT_LANG"
;;              "DELAY"
;;              "DELETE"
;;              "DELETECYCLE"
;;              "DELIMITER"
;;              "DEPENDENCIES"
;;              "DESCRIPTION_FILE"
;;              "DIAGRAM"
;;              "DIAGRAM_XPOS"
;;              "DIAGRAM_YPOS"
;;              "DIRNAME"
;;              "DISABLE"
;;              "DISABLE_DRAG"
;;              "DISALLOW"
;;              "DISCRETE"
;;              "DISPLAY"
;;              "DOMAIN"
;;              "DOTS"
;;              "DROP"
;;              "EDITABLE"
;;              "ELSE"
;;              "ENABLE"
;;              "EOLN"
;;              "ERASE"
;;              "ERROR"
;;              "EXIT"
;;              "EXPAND"
;;              "EXPLICIT"
;;              "EXTENSION"
;;              "FALSE"
;;              "FATAL"
;;              "FILENAME"
;;              "FILTER"
;;              "FIRSTCYCLE"
;;              "FIRSTLEVEL"
;;              "FORMAT"
;;              "FORTRAN"
;;              "FRAME"
;;              "FROM_STRING_DATETIME"
;;              "GET"
;;              "GET_SELECTION"
;;              "GET_SORT_CRITERIA"
;;              "GETCYCLE"
;;              "GETTEXT"
;;              "GLOBAL"
;;              "Global_Point"
;;              "Global_Rect"
;;              "GOCYCLE"
;;              "GUI_ELEMENT_METHOD"
;;              "HEADER"
;;              "HELPKEY"
;;              "HELPTEXT"
;;              "HIDDEN"
;;              "HIDE_CYCLE"
;;              "HIDE_EMPTY_FOLDER"
;;              "HIGH"
;;              "HORIZONTAL"
;;              "HOST"
;;              "ICON"
;;              "ICONVIEW"
;;              "IF"
;;              "IGNORE"
;;              "IMAG"
;;              "IN"
;;              "INDENT"
;;              "INDEXED_SET"
;;              "INFO"
;;              "INIT"
;;              "INIT_HEADER"
;;              "INPUT"
;;              "INTENS_PATH"
;;              "INTENS_REVISION"
;;              "INTENS_VERSION"
;;              "INTENS_VERSION_MAJOR"
;;              "INTENS_VERSION_MINOR"
;;              "INTENS_VERSION_PATCH"
;;              "INVALID"
;;              "INVERTED"
;;              "IPADDR"
;;              "JSON"
;;              "JUSTIFY"
;;              "JUSTLEFT"
;;              "JUSTRIGHT"
;;              "KEY"
;;              "LABEL"
;;              "LASTCYCLE"
;;              "LASTLEVEL"
;;              "LATEX"
;;              "LEFT"
;;              "LEGEND"
;;              "LENGTH"
;;              "LINEAR"
;;              "LINESTYLE"
;;              "LOAD"
;;              "LOCALE"
;;              "LOCK"
;;              "LOCKABLE"
;;              "LOG"
;;              "LOG10"
;;              "LOG_WINDOW"
;;              "LOG_X"
;;              "LOG_Y"
;;              "LOGOFF"
;;              "LOGON"
;;              "LOGON_STATUS"
;;              "MAIN"
;;              "MAP"
;;              "MARGIN"
;;              "MARKER"
;;              "MATHEMATICA"
;;              "MATLAB"
;;              "MATRIX"
;;              "MAX_OCCURS"
;;              "MAX_PENDING_FUNCTIONS"
;;              "MAXCYCLE"
;;              "MESSAGE_CURLERROR"
;;              "MESSAGE_UNAUTHORIZED"
;;              "MESSAGEBOX"
;;              "MFM"
;;              "MINMAX"
;;              "MODIFIED"
;;              "MODIFY"
;;              "MOTIF"
;;              "MOVE"
;;              "MULTIPLE_SELECTION"
;;              "NAMESPACE"
;;              "NAVIGATION"
;;              "NEWCYCLE"
;;              "NEXTCYCLE"
;;              "NO_COLORBIT"
;;              "NO_DEPENDENCIES"
;;              "NO_GZ"
;;              "NO_LOG"
;;              "NO_PANEDWINDOW"
;;              "NO_SCROLLBARS"
;;              "NODE"
;;              "NONE"
;;              "NP"
;;              "NS"
;;              "OF"
;;              "OFFSET"
;;              "OLDVALUE"
;;              "OMIT_ACTIVATE"
;;              "OMIT_TTRAIL"
;;              "ON_CYCLE_EVENT"
;;              "ON_CYCLE_SWITCH"
;;              "ON_EOS"
;;              "ON_VIEW_ACTION"
;;              "OPEN"
;;              "OPEN_FILE"
;;              "OPEN_URL"
;;              "OPENLEVELS"
;;              "OPTIONAL"
;;              "ORIENTATION"
;;              "OUT"
;;              "OVERLAY"
;;              "PACK"
;;              "PACKAGES"
;;              "PANEDWINDOW"
;;              "PARENT"
;;              "PASSWORD"
;;              "PASTE"
;;              "PATH"
;;              "PATTERN"
;;              "PERIOD"
;;              "PERSISTENT"
;;              "PIXMAP"
;;              "PLACEHOLDER"
;;              "PLOT"
;;              "PLOT2D_UIMODE"
;;              "POLAR"
;;              "PORT"
;;              "PORT_REQUEST"
;;              "POSITION"
;;              "POST"
;;              "POSTGRES"
;;              "PREVIEW"
;;              "PRINT"
;;              "PRINT_LOG"
;;              "PRINTSTYLE"
;;              "PRIORITY"
;;              "ProgressDialog"
;;              "PROTO"
;;              "PSPLOT"
;;              "PUBLISH"
;;              "PUT"
;;              "PW"
;;              "QUERY"
;;              "QUIT"
;;              "RADIO"
;;              "RANGE"
;;              "READONLY"
;;              "REASON"
;;              "REASON_ACTIVATE"
;;              "REASON_CLEAR"
;;              "REASON_CLOSE"
;;              "REASON_CONNECTION"
;;              "REASON_CYCLE_CLEAR"
;;              "REASON_CYCLE_DELETE"
;;              "REASON_CYCLE_NEW"
;;              "REASON_CYCLE_RENAME"
;;              "REASON_CYCLE_SWITCH"
;;              "REASON_DROP"
;;              "REASON_DUPLICATE"
;;              "REASON_FOCUS"
;;              "REASON_FOCUS_OUT"
;;              "REASON_FUNCTION"
;;              "REASON_GUI_UPDATE"
;;              "REASON_INPUT"
;;              "REASON_INSERT"
;;              "REASON_MOVE"
;;              "REASON_OPEN"
;;              "REASON_PACK"
;;              "REASON_REMOVE"
;;              "REASON_REMOVE_CONNECTION"
;;              "REASON_REMOVE_ELEMENT"
;;              "REASON_SELECT"
;;              "REASON_SELECT_POINT"
;;              "REASON_SELECT_RECTANGLE"
;;              "REASON_TASK"
;;              "REASON_UNSELECT"
;;              "REPLACE"
;;              "REPLY"
;;              "REPORT"
;;              "REQUEST"
;;              "RESET"
;;              "RESET_ERROR"
;;              "RESOURCE"
;;              "RESPONSE"
;;              "REST_JWT_LOGON"
;;              "REST_LOGOFF"
;;              "REST_LOGON"
;;              "REST_SERVICE"
;;              "RESTBASE"
;;              "RESTUSERNAME"
;;              "RESTUSERNAMELIST"
;;              "RETURN"
;;              "RIGHT"
;;              "RMS"
;;              "ROTATE_180"
;;              "ROUND"
;;              "ROUND10"
;;              "ROUND5"
;;              "ROW"
;;              "ROWSPAN"
;;              "RUN"
;;              "SAME_YRANGE"
;;              "SAVE"
;;              "SB"
;;              "SCALAR"
;;              "SCALE"
;;              "SCHEMA"
;;              "SCROLLBARS"
;;              "SELECT_LIST"
;;              "SEND"
;;              "SEPARATOR"
;;              "SERIALIZE"
;;              "SERIALIZE_FORM"
;;              "SET_CURRENT_FORM"
;;              "SET_DB_TIMESTAMP"
;;              "SET_ERROR"
;;              "SET_FUNC_ARGS"
;;              "SET_INDEX"
;;              "SET_MQ_HOST"
;;              "SET_MSG"
;;              "SET_REASON"
;;              "SET_RESOURCE"
;;              "SET_THIS"
;;              "SETTINGS"
;;              "SILENT"
;;              "SIN"
;;              "SIZE"
;;              "SKIP"
;;              "SLIDER"
;;              "SORT"
;;              "SORTORDER"
;;              "SOURCE"
;;              "SOURCE2"
;;              "SQRT"
;;              "STACKING_BAR"
;;              "START"
;;              "STD_WINDOW"
;;              "STEP"
;;              "STOP"
;;              "STRETCH"
;;              "STRING_DATE"
;;              "STRING_DATETIME"
;;              "STRING_TIME"
;;              "STRING_VALUE"
;;              "STYLE"
;;              "STYLESHEET"
;;              "SUBSCRIBE"
;;              "SURFACE"
;;              "TABLESIZE"
;;              "TAG"
;;              "TAN"
;;              "TEMPLATE"
;;              "THIS"
;;              "THUMBNAIL"
;;              "TIMEOUT"
;;              "TIMESTAMP"
;;              "TIMETABLE"
;;              "TO_STRING_DATETIME"
;;              "TO_STRING_TIME"
;;              "TOGGLE"
;;              "TOOLTIP"
;;              "TOP"
;;              "TOUCH"
;;              "TRANSIENT"
;;              "TRANSPARENT"
;;              "TRUE"
;;              "TSEP"
;;              "UI_UPDATE"
;;              "UNIT"
;;              "UNITS"
;;              "UNMAP"
;;              "UNSET"
;;              "UPDATE"
;;              "UPDATE_FORMS"
;;              "URL"
;;              "USER"
;;              "USER_PATH"
;;              "VALID"
;;              "VAR"
;;              "VERSION"
;;              "VERTICAL"
;;              "VISIBLE"
;;              "VOID"
;;              "WARN"
;;              "WEBAPI"
;;              "WHEEL_EVENT"
;;              "WHILE"
;;              "WRAP"
;;              "WRITE_SETTINGS"
;;              "WRITEONLY"
;;              "XANNOTATION"
;;              "XAXIS"
;;              "XAXIS2"
;;              "XML"
;;              "XMLGROUP"
;;              "Y1_STYLE"
;;              "Y2_STYLE"
;;              "YANNOTATION"
;;              "YAXIS"
;;              "YAXIS1"
;;              "YAXIS2"
;;              ) t)

;; font-lock-type-face
;; (regexp-opt '(
;;              "CDATA"
;;              "COLOR"
;;              "COMPLEX"
;;              "FIELDGROUP"
;;              "FILESTREAM"
;;              "FOLDER"
;;              "FORM"
;;              "FUNC"
;;              "FOCUS_FUNC"
;;              "IMAGE"
;;              "INDEX"
;;              "INT"
;;              "INTEGER"
;;              "LATEXREPORT"
;;              "LINEPLOT"
;;              "LIST"
;;              "LISTPLOT"
;;              "MENU"
;;              "MESSAGE_QUEUE"
;;              "NAVIGATOR"
;;              "PLOT2D"
;;              "PLUGIN"
;;              "PROCESS"
;;              "PROCESSGROUP"
;;              "PYTHON"
;;              "REAL"
;;              "REPORTSTREAM"
;;              "SET"
;;              "SOCKET"
;;              "STREAM"
;;              "STRING"
;;              "STRUCT"
;;              "TABLE"
;;              "TASK"
;;              "TEXT_WINDOW"
;;              "THERMO"
;;              "TIMER"
;;              "TRANSACTION"
;;              "UNIPLOT"
;;              "USERGROUPS"
;;              "XRT3DPLOT"
;;              "XRTGRAPH"
;;              ) t)

;; underline
;; (regexp-opt '(
;;              "PLOTGROUP"
;;              "VERSION_1"
;;              "VERSION_2"
;;              ) t)

(defconst intens-font-lock-keywords
  (list
   ;; comments are done using the intens-mode-syntax-table
   ;; '("\\(//[^\n]*\\)" . font-lock-comment-face)
   ;; '("\\(\"[^\n\"]*\"\\)" . font-lock-string-face)
   '("\\<\\(D\\(?:ATAPOOL\\|B_MANAGER\\|ESCRIPTION\\)\\|END\\|FUNCTIONS\\|HELPFILE\\|LANGUAGE\\|\\(?:OPERATO\\|\\(?:STREAM\\|UI_MANAG\\)E\\)R\\)\\>"
     . 'font-lock-function-name-face)
   '("\\<\\(A\\(?:B\\(?:ORT\\(?:TRANSACTION\\)?\\|S\\)\\|COS\\|FTER_\\(?:DB_LOGON\\|UPDATE_FORMS\\)\\|L\\(?:ARM_\\(?:COLOR\\|LEVEL\\)\\|IGN\\|L\\(?:CYCLES\\|OW\\)\\|WAYS\\)\\|NNOTATION\\|PP\\(?:END\\|_\\(?:MODAL\\|VERSION_\\(?:M\\(?:\\(?:AJ\\|IN\\)OR\\)\\|PATCH\\)\\)\\)\\|R\\(?:EA\\|G\\|ROWS\\)\\|S\\(?:CII\\|IN\\|PECT_RATIO\\(?:_REF_AXIS\\)?\\|SIGN_CO\\(?:NSISTENCY\\|RR\\)\\)\\|T\\(?:AN2?\\|TRS\\)\\|UTO\\(?:CLEAR_DEPENDENCIES\\|LEVEL\\|_\\(?:SCROLL\\|WIDTH\\)\\)\\|VG\\|XES_ORIGIN\\(?:_[XY]\\)?\\)\\|B\\(?:A\\(?:R\\|SE\\(?:NAME\\)?\\|TCH\\)\\|E\\(?:EP\\|GINTRANSACTION\\)\\|INARY\\|OTTOM\\|UTTON\\(?:S\\|_\\(?:CANCEL\\|NO\\|YES\\)\\)?\\|Y\\)\\|C\\(?:APTION\\|E\\(?:LL\\|NTER\\)\\|HANGED\\|L\\(?:ASS\\(?:NAME\\)?\\|EAR\\(?:CYCLE\\|_SELECTION\\)?\\|OSE_BUTTON\\)\\|O\\(?:L\\(?:OR\\(?:BIT\\|_SCALE\\)\\|SPAN\\)\\|M\\(?:BOBOX\\|MITTRANSACTION\\|P\\(?:ARE\\|OSE\\(?:_STRING\\)?\\)\\)\\|N\\(?:FIRM\\(?:_CANCEL\\)?\\|TOUR\\)\\|PY\\|[LS]\\)\\|\\(?:URRENT_\\(?:DAT\\(?:ETIM\\)?\\|TIM\\)\\|YCL\\(?:ENAM\\)?\\)E\\)\\|D\\(?:A\\(?:EMON\\|T\\(?:A\\(?:SET_TEXT\\|_SIZE\\)\\|[AE]\\)\\)\\|B\\(?:A\\(?:SSEMBLY\\|TTR\\)\\|LIST\\|UNIT\\|_VERSION_\\(?:IGNORE\\|M\\(?:\\(?:AJ\\|IN\\)OR\\)\\|PATCH\\)\\)\\|E\\(?:ADLY_SILENT\\|BUG\\|FAULT_LANG\\|L\\(?:AY\\|ETE\\(?:CYCLE\\)?\\|IMITER\\)\\|PENDENCIES\\|SCRIPTION_FILE\\)\\|I\\(?:AGRAM\\(?:_\\(?:[XY]POS\\)\\)?\\|RNAME\\|S\\(?:A\\(?:BLE\\(?:_DRAG\\)?\\|LLOW\\)\\|CRETE\\|PLAY\\)\\)\\|O\\(?:MAIN\\|TS\\)\\|ROP\\)\\|E\\(?:DITABLE\\|LSE\\|NABLE\\|OLN\\|R\\(?:ASE\\|ROR\\)\\|X\\(?:IT\\|P\\(?:AND\\|LICIT\\)\\|TENSION\\)\\)\\|F\\(?:A\\(?:LSE\\|TAL\\)\\|I\\(?:L\\(?:ENAME\\|TER\\)\\|RST\\(?:CYCLE\\|LEVEL\\)\\)\\|OR\\(?:MAT\\|TRAN\\)\\|R\\(?:\\(?:A\\|OM_STRING_DATETI\\)ME\\)\\)\\|G\\(?:ET\\(?:CYCLE\\|TEXT\\|_S\\(?:ELECTION\\|ORT_CRITERIA\\)\\)?\\|LOBAL\\|OCYCLE\\|UI_ELEMENT_METHOD\\|lobal_\\(?:\\(?:Poin\\|Rec\\)t\\)\\)\\|H\\(?:E\\(?:ADER\\|LP\\(?:KEY\\|TEXT\\)\\)\\|I\\(?:D\\(?:DEN\\|E_\\(?:CYCLE\\|EMPTY_FOLDER\\)\\)\\|GH\\)\\|O\\(?:RIZONTAL\\|ST\\)\\)\\|I\\(?:CON\\(?:VIEW\\)?\\|GNORE\\|MAG\\|N\\(?:DE\\(?:\\(?:N\\|XED_SE\\)T\\)\\|FO\\|IT\\(?:_HEADER\\)?\\|PUT\\|TENS_\\(?:PATH\\|REVISION\\|VERSION\\(?:_\\(?:M\\(?:\\(?:AJ\\|IN\\)OR\\)\\|PATCH\\)\\)?\\)\\|V\\(?:\\(?:ALI\\|ERTE\\)D\\)\\)\\|PADDR\\|[FN]\\)\\|J\\(?:SON\\|UST\\(?:IFY\\|\\(?:LEF\\|RIGH\\)T\\)\\)\\|KEY\\|L\\(?:A\\(?:BEL\\|ST\\(?:CYCLE\\|LEVEL\\)\\|TEX\\)\\|E\\(?:FT\\|GEND\\|NGTH\\)\\|INE\\(?:AR\\|STYLE\\)\\|O\\(?:AD\\|C\\(?:ALE\\|K\\(?:ABLE\\)?\\)\\|G\\(?:10\\|O\\(?:FF\\|N\\(?:_STATUS\\)?\\)\\|_\\(?:WINDOW\\|[XY]\\)\\)?\\)\\)\\|M\\(?:A\\(?:IN\\|P\\|R\\(?:GIN\\|KER\\)\\|T\\(?:HEMATICA\\|LAB\\|RIX\\)\\|X\\(?:CYCLE\\|_\\(?:\\(?:OCCUR\\|PENDING_FUNCTION\\)S\\)\\)\\)\\|ESSAGE\\(?:BOX\\|_\\(?:CURLERROR\\|UNAUTHORIZED\\)\\)\\|FM\\|INMAX\\|O\\(?:DIF\\(?:IED\\|Y\\)\\|TIF\\|VE\\)\\|ULTIPLE_SELECTION\\)\\|N\\(?:A\\(?:MESPACE\\|VIGATION\\)\\|E\\(?:\\(?:W\\|XT\\)CYCLE\\)\\|O\\(?:DE\\|NE\\|_\\(?:COLORBIT\\|DEPENDENCIES\\|GZ\\|LOG\\|PANEDWINDOW\\|SCROLLBARS\\)\\)\\|[PS]\\)\\|O\\(?:F\\(?:FSET\\)?\\|LDVALUE\\|MIT_\\(?:ACTIVATE\\|TTRAIL\\)\\|N_\\(?:CYCLE_\\(?:EVENT\\|SWITCH\\)\\|EOS\\|VIEW_ACTION\\)\\|P\\(?:EN\\(?:LEVELS\\|_\\(?:FILE\\|URL\\)\\)?\\|TIONAL\\)\\|RIENTATION\\|UT\\|VERLAY\\)\\|P\\(?:A\\(?:CK\\(?:AGES\\)?\\|NEDWINDOW\\|RENT\\|S\\(?:SWORD\\|TE\\)\\|T\\(?:H\\|TERN\\)\\)\\|ER\\(?:IOD\\|SISTENT\\)\\|IXMAP\\|L\\(?:ACEHOLDER\\|OT\\(?:2D_UIMODE\\)?\\)\\|O\\(?:LAR\\|RT\\(?:_REQUEST\\)?\\|S\\(?:ITION\\|T\\(?:GRES\\)?\\)\\)\\|R\\(?:EVIEW\\|I\\(?:NT\\(?:STYLE\\|_LOG\\)?\\|ORITY\\)\\|OTO\\)\\|SPLOT\\|U\\(?:BLISH\\|T\\)\\|W\\|rogressDialog\\)\\|QU\\(?:ERY\\|IT\\)\\|R\\(?:A\\(?:DIO\\|NGE\\)\\|E\\(?:A\\(?:DONLY\\|SON\\(?:_\\(?:ACTIVATE\\|C\\(?:L\\(?:EAR\\|OSE\\)\\|ONNECTION\\|YCLE_\\(?:CLEAR\\|DELETE\\|NEW\\|RENAME\\|SWITCH\\)\\)\\|D\\(?:ROP\\|UPLICATE\\)\\|F\\(?:OCUS\\(?:_OUT\\)?\\|UNCTION\\)\\|GUI_UPDATE\\|IN\\(?:\\(?:PU\\|SER\\)T\\)\\|MOVE\\|OPEN\\|PACK\\|REMOVE\\(?:_\\(?:CONNECTION\\|ELEMENT\\)\\)?\\|SELECT\\(?:_\\(?:POINT\\|RECTANGLE\\)\\)?\\|TASK\\|UNSELECT\\)\\)?\\)\\|P\\(?:L\\(?:ACE\\|Y\\)\\|ORT\\)\\|QUEST\\|S\\(?:ET\\(?:_ERROR\\)?\\|OURCE\\|PONSE\\|T\\(?:BASE\\|USERNAME\\(?:LIST\\)?\\|_\\(?:JWT_LOGON\\|LOGO\\(?:FF\\|N\\)\\|SERVICE\\)\\)\\)\\|TURN\\)\\|IGHT\\|MS\\|O\\(?:TATE_180\\|UND\\(?:10\\|5\\)?\\|W\\(?:SPAN\\)?\\)\\|UN\\)\\|S\\(?:A\\(?:\\(?:ME_YRANG\\|V\\)E\\)\\|B\\|C\\(?:AL\\(?:AR\\|E\\)\\|HEMA\\|ROLLBARS\\)\\|E\\(?:LECT_LIST\\|ND\\|PARATOR\\|RIALIZE\\(?:_FORM\\)?\\|T\\(?:TINGS\\|_\\(?:CURRENT_FORM\\|DB_TIMESTAMP\\|ERROR\\|FUNC_ARGS\\|INDEX\\|M\\(?:Q_HOST\\|SG\\)\\|RE\\(?:ASON\\|SOURCE\\)\\|THIS\\)\\)\\)\\|I\\(?:LENT\\|N\\|ZE\\)\\|KIP\\|LIDER\\|O\\(?:RT\\(?:ORDER\\)?\\|URCE2?\\)\\|QRT\\|T\\(?:A\\(?:CKING_BAR\\|RT\\)\\|D_WINDOW\\|EP\\|OP\\|R\\(?:ETCH\\|ING_\\(?:\\(?:DAT\\(?:ETIM\\)?\\|TIM\\|VALU\\)E\\)\\)\\|YLE\\(?:SHEET\\)?\\)\\|U\\(?:\\(?:BSCRIB\\|RFAC\\)E\\)\\)\\|T\\(?:A\\(?:BLESIZE\\|[GN]\\)\\|EMPLATE\\|H\\(?:IS\\|UMBNAIL\\)\\|IME\\(?:OUT\\|STAMP\\|TABLE\\)\\|O\\(?:GGLE\\|OLTIP\\|P\\|UCH\\|_STRING_\\(?:\\(?:DATE\\)?TIME\\)\\)\\|R\\(?:ANS\\(?:\\(?:I\\|PAR\\)ENT\\)\\|UE\\)\\|SEP\\)\\|U\\(?:I_UPDATE\\|N\\(?:ITS?\\|MAP\\|SET\\)\\|PDATE\\(?:_FORMS\\)?\\|RL\\|SER\\(?:_PATH\\)?\\)\\|V\\(?:A\\(?:LID\\|R\\)\\|ER\\(?:SION\\|TICAL\\)\\|ISIBLE\\|OID\\)\\|W\\(?:ARN\\|EBAPI\\|H\\(?:EEL_EVENT\\|ILE\\)\\|R\\(?:AP\\|ITE\\(?:ONLY\\|_SETTINGS\\)\\)\\)\\|X\\(?:A\\(?:NNOTATION\\|XIS2?\\)\\|ML\\(?:GROUP\\)?\\)\\|Y\\(?:1_STYLE\\|2_STYLE\\|A\\(?:NNOTATION\\|XIS[12]?\\)\\)\\)\\>"
     . font-lock-keyword-face)
   '("\\<\\(C\\(?:DATA\\|O\\(?:LOR\\|MPLEX\\)\\)\\|F\\(?:I\\(?:ELDGROUP\\|LESTREAM\\)\\|O\\(?:CUS_FUNC\\|LDER\\|RM\\)\\|UNC\\)\\|I\\(?:MAGE\\|N\\(?:DEX\\|T\\(?:EGER\\)?\\)\\)\\|L\\(?:\\(?:ATEXREPOR\\|I\\(?:NEPLO\\|S\\(?:TPLO\\)?\\)\\)T\\)\\|ME\\(?:NU\\|SSAGE_QUEUE\\)\\|NAVIGATOR\\|P\\(?:L\\(?:OT2D\\|UGIN\\)\\|ROCESS\\(?:GROUP\\)?\\|YTHON\\)\\|RE\\(?:AL\\|PORTSTREAM\\)\\|S\\(?:ET\\|OCKET\\|TR\\(?:EAM\\|ING\\|UCT\\)\\)\\|T\\(?:A\\(?:BLE\\|SK\\)\\|EXT_WINDOW\\|HERMO\\|IMER\\|RANSACTION\\)\\|U\\(?:NIPLOT\\|SERGROUPS\\)\\|XRT\\(?:3DPLOT\\|GRAPH\\)\\)\\>"
     . font-lock-type-face)
   '("\\<\\(PLOTGROUP\\|VERSION_[12]\\)\\>"
     . 'underline)
   )
  "Expressions to highlight in intens-mode")

(defconst intens-indent-width 2)

(defun intens-indent-line ()
  "Indent current line as INTENS code."
; (debug 'debug)
  (interactive)
  (beginning-of-line)
  (if (or (bobp) (looking-at "^[ \t]*END "))
      (indent-line-to 0)       ; First line is always non-indented
    (let ((not-indented t) cur-indent)
      (if (looking-at "^[ \t]*[;,)}]")  ; line we are looking at starts with a ;,)}
          (progn
            (save-excursion
              (forward-line -1)
              (while (looking-at "^[ \t]*$")
                (forward-line -1)
                )
              (if (looking-at "^[ \t]*,") ; previous line starts with a ,
                  (setq cur-indent (current-indentation))
                (progn
                  (setq cur-indent (- (current-indentation) intens-indent-width))
                  (if (< cur-indent 0) ; We can't indent past the left margin
                      (setq cur-indent 0))))))
        (save-excursion
          (while not-indented ; Iterate backwards until we find an indentation hint
            (forward-line -1)
            (if (or (looking-at "^[ \t]*END ")           ; Line starts with "END "
                    (looking-at "^[ \t]*//")             ; Line starts with "//"
                    (looking-at "^[ \t]*}[ \t]*\\($\\|//\\)") ; Line is "}" (ev. followed by comment)
                    (looking-at ".*;[ \t]*\\($\\|//\\)") ; Line ends with ; (ev. followed by comment)
                    )
                (progn
                  (setq cur-indent (current-indentation))
                  (setq not-indented nil))
              (if (looking-at "^[ \t]*,") ; previous line starts with a ,
                  (progn
                    (if (looking-at ".*[({][ \t]*\\($\\|//\\)")
                        (setq cur-indent (+ (current-indentation) (* 2 intens-indent-width)))
                      (setq cur-indent (+ (current-indentation) intens-indent-width))
                      )
                    (setq not-indented nil))
                (if (looking-at "\\(\
^[ \t]*\\<[A-Z_0-9]+\\>[ \t]*\\({[^}]+}\\|\\)[ \t]*\\($\\|//\\)\
\\|.*[({][ \t]*\\($\\|//\\)\
\\)") ; This hint indicates that we need to indent an extra level
                    (progn
                      (if (looking-at "^[ \t]*\\<VOID\\>[ \t]*\\({[^}]+}\\|\\)[ \t]*\\($\\|//\\)") ; previous line is VOID
                          (setq cur-indent (current-indentation))
                        (setq cur-indent (+ (current-indentation) intens-indent-width)) ; Do the actual indenting
                        )
                      (setq not-indented nil))
                  (if (bobp)
                      (setq not-indented nil))))))))
      (if cur-indent
          (indent-line-to cur-indent)
        (indent-line-to 0))))) ; If we didn't see an indentation hint, then allow no indentation

(defvar intens-mode-syntax-table
  (let ((intens-mode-syntax-table (make-syntax-table)))
    ;; Comment styles are same as C++
    (modify-syntax-entry ?/ ". 12b" intens-mode-syntax-table)
    (modify-syntax-entry ?\n "> b" intens-mode-syntax-table)
    intens-mode-syntax-table)
  "Syntax table for intens-mode")

(defun intens-mode ()
  "Intens-mode is major mode for editing Intens dot-des files."
  (interactive)
  (kill-all-local-variables)
  (use-local-map intens-mode-map)
  (set-syntax-table intens-mode-syntax-table)
  ;; Set up comment character
  (set (make-local-variable 'comment-start) "//")
  ;; Avoid indent with tabs
  (set (make-local-variable 'indent-tabs-mode) nil)
  ;; Set up font-lock
  (set (make-local-variable 'font-lock-defaults)
       '(intens-font-lock-keywords
         nil ;; do string/comment highlighting
         nil ;; keywords are case sensitive
         ;; this puts _ as a word constituent
         ;; simpliying our keywords significally
         ((?_ . "w"))))
  ;; Register our indentation function
  (set (make-local-variable 'indent-line-function) 'intens-indent-line)
  (setq major-mode 'intens-mode)
  (setq mode-name "INTENS")
  (run-hooks 'intens-mode-hook)
  )

(provide 'intens-mode)

 ;;; intens-mode.el ends here
