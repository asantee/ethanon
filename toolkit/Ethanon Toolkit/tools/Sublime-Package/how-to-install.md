Ethanon plug-in for Sublime
===========================

This plug-in brings Ethanon support to [Sublime Text] 4:

* **Ethanon** syntax for **.angelscript** files: AngelScript as the engine uses it (`funcdef`, `mixin`/`abstract`/`shared`
  classes, `get`/`set` accessors, lambdas, `!is`, `cast<>`, handles, `#include` / `#if TESTING`), the engine API (global
  functions, types, enum values) highlighted as library names, and classes, functions, methods, enums, namespaces and
  funcdefs in **Goto Symbol** (Cmd+R), **Goto Symbol in Project** (Cmd+Shift+R) and **Goto Definition** (F12).
* **ENML** syntax for the engine's key/value files: **.enml** (`app.enml`, `items/*.enml`), **.character**, **.layer**,
  **.element** and **.elements**. It follows the engine's parser (`gs2d/src/Enml/Enml.cpp`), so anything the parser would
  reject shows up as an error: invalid names, a missing `{` or `=`, an empty value, a backslash other than `\;` or `\\`.
  Section names (`default`, `item0`, ...) are listed by Cmd+R.
* Comment toggling (Cmd+/, Cmd+Alt+/) and auto-indent for both.
* Build shortcuts.

  [Sublime Text]: http://www.sublimetext.com/

How to install
==============

There are two ways to install Sublime plug-in for Ethanon:

Direct from source
------------------

- Open Sublime Text, go to **Preferences** -> **Browse Packages**
- Copy the directory "/Ethanon/" to "sublime-dir/Packages/"
- Restart Sublime

OR

From .sublime-package
---------------------

- Zip all files in "Sublime-Package/Ethanon/"
- Rename the zip file to Ethanon.sublime-package  
  Note: Sublime plugin source files must be in the package root
- Move Ethanon.sublime-package to "sublime-dir/Installed Packages/"
- Restart Sublime Text

Upgrading from an older copy: delete `Ethanon.tmLanguage` from the installed folder. It was replaced by
`Ethanon.sublime-syntax`, and two syntaxes with the same name conflict.

Setting up build option
-----------------------

- Open the *.angelscript file from your project with Sublime Text
- Go to **Tools** -> **Build system** and pick **Ethanon**
- Use **Cmd + B** to Build and **Cmd + Shift + B** to Run

Maintenance
===========

- The engine API lists in `Ethanon.sublime-syntax` are generated. After binding something new to AngelScript, run
  `perl update-api-lists.pl` in this folder; it reads every `Register*` call under `toolkit/Source/src/engine` and
  `src/addons`.
- `syntax_test_enml.enml` and `syntax_test_ethanon.angelscript` are Sublime syntax tests: open one and run
  **Tools** -> **Build** (Cmd+B picks "Syntax Tests"). Their first line names the syntax as
  `Packages/Ethanon/...`; if the package lives elsewhere (for example `Packages/User/Ethanon`), adjust that line in the
  installed copy.
