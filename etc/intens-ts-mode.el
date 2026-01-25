;;; -*- lexical-binding: t; -*-
;;; SPDX-FileCopyrightText: 2025 SEMAFOR Informatik & Energie AG, Basel
;;;
;;; SPDX-License-Identifier: Apache-2.0

;; ====================================
;; Intens Tree-Sitter
;; ====================================
(require 'treesit)

(setq intens-lib-dir (concat (file-name-directory (or load-file-name (buffer-file-name))) "../lib"))
(setq intens-lib64-dir (concat (file-name-directory (or load-file-name (buffer-file-name))) "../lib64"))
(add-to-list 'treesit-extra-load-path intens-lib-dir)
(add-to-list 'treesit-extra-load-path intens-lib64-dir)

(define-derived-mode intens-ts-mode prog-mode ""
  "Major mode for editing INTENS with tree-sitter."
  (interactive)
  ; :syntax-table sgml-mode-syntax-table

  (setq-local font-lock-defaults nil)
  (when (treesit-ready-p 'intens)
    (treesit-parser-create 'intens)
    (intens-ts-setup))
  (setq mode-name "INTENS-TS")
  (eglot-ensure)
)

(defvar intens-ts-font-lock-rules
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
    :feature comment
    ((gen_comment) @font-lock-comment-face)

    :language intens
    :override t
    :feature string
    ((gen_quote_string) @font-lock-string-face)
  )
)

(if (ignore-errors (load "g-intens-ts"))
  (set 'intens-ts-font-lock-rules intens-ts-font-lock-rules-loaded))

(defun intens-ts-setup ()
  "Setup treesit for intens-ts-mode."
  ;; Our tree-sitter setup goes here.

  ;; This handles font locking -- more on that below.
  ;; This handles indentation -- again, more on that below.
  ; (setq-local treesit-simple-indent-rules intens-ts-indent-rules)

  (setq-local treesit-font-lock-feature-list
            '((comment builtin-function preprocessor)
              (keyword type operator variable number)
              (string)
              (delimiter bracket)))


  (setq-local treesit-font-lock-settings
        (apply #'treesit-font-lock-rules
             intens-ts-font-lock-rules))

  ;; ... everything else we talk about go here also ...

  ;; End with this
  (treesit-major-mode-setup)
  (font-lock-update)
  )

(with-eval-after-load 'eglot
(add-to-list 'eglot-server-programs
             '((intens-ts-mode) . ("intens-language-server" "--stdio")))
)

(setq project-vc-extra-root-markers '(".lspconfig"))
(setq project-vc-extra-root-markers '(".intensproj"))

;; Automatically use intens-ts-mode for .des, .des.in and .inc
;not yet; (add-to-list 'auto-mode-alist '("\\.des\\'" . intens-ts-mode))
;not yet; (add-to-list 'auto-mode-alist '("\\.des.in\\'" . intens-ts-mode))
(add-to-list 'auto-mode-alist '("\\.inc\\'" . intens-ts-mode))
(provide 'intens-ts-mode)
