
(add-to-list 'load-path "../../etc")
(require 'intens-mode)

;; enable python for block eval
(org-babel-do-load-languages
 'org-babel-load-languages
 '((python . t)))

;;  Org executes code blocks without prompting the user for confirmation
;;  and uses css
(setq org-confirm-babel-evaluate nil
      org-html-htmlize-output-type 'css)
