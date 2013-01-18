Ethanon plug-in for Sublime
===========================

This plug-in brings Ethanon syntax highlighting on **.angelscript** files and Build shortcuts to [Sublime Text].

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

Setting up build option
-----------------------

- Open the *.angelscript file from your project with Sublime Text
- Go to **Tools** -> **Build system** and pick **Ethanon**
- Use **Cmd + B** to Build and **Cmd + Shift + B** to Run
