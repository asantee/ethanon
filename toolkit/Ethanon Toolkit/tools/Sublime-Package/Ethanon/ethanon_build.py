"""Build-system support for Ethanon.sublime-build.

ethanon_build: runs the engine from the project root, i.e. the nearest folder
at or above the current file that holds main.angelscript, so Cmd+B works from
any script of the project.

EthanonBuildOutputClick: a single click on an error line of the build panel
or on a script exception's callstack line, opens the file at that line
(Sublime's own result navigation needs a double click and skips callstacks).
"""

import os
import re

import sublime
import sublime_plugin
from Default.exec import ExecCommand

MAIN_SCRIPT = "main.angelscript"
OUTPUT_SYNTAX = "Ethanon Build Output.sublime-syntax"
# "    void f() (src/x.angelscript, 12)" lines of a script exception's callstack
CALLSTACK_REGEX = re.compile(r"^    .* \((.+?), (\d+)\)$")


def find_project_root(start_dir):
    current = os.path.abspath(start_dir)
    while True:
        if os.path.isfile(os.path.join(current, MAIN_SCRIPT)):
            return current
        parent = os.path.dirname(current)
        if parent == current:
            return None
        current = parent


class EthanonBuildCommand(ExecCommand):
    def run(self, cmd=None, working_dir="", kill=False,
            update_annotations_only=False, **kwargs):
        if kill or update_annotations_only:
            super().run(kill=kill,
                        update_annotations_only=update_annotations_only,
                        **kwargs)
            return

        start_dir = working_dir
        view = self.window.active_view()
        if not start_dir and view and view.file_name():
            start_dir = os.path.dirname(view.file_name())
        if not start_dir:
            sublime.error_message("Ethanon: save the file before building.")
            return

        root = find_project_root(start_dir)
        if root is None:
            sublime.error_message(
                "Ethanon: no {} found in\n{}\nor in any folder above it."
                .format(MAIN_SCRIPT, start_dir))
            return

        if cmd:
            cmd = list(cmd) + ["dir=" + root]
        super().run(cmd=cmd, working_dir=root, **kwargs)


class EthanonBuildOutputClick(sublime_plugin.EventListener):
    def on_post_text_command(self, view, command_name, args):
        if command_name != "drag_select" or (args or {}).get("by"):
            return
        settings = view.settings()
        if not settings.get("syntax", "").endswith(OUTPUT_SYNTAX):
            return
        selection = view.sel()
        if len(selection) != 1 or not selection[0].empty():
            return
        line = view.substr(view.line(selection[0].begin()))
        file_regex = settings.get("result_file_regex")
        match = (file_regex and re.match(file_regex, line)) or CALLSTACK_REGEX.match(line)
        if not match:
            return
        path, row = match.group(1), match.group(2)
        if not os.path.isabs(path):
            path = os.path.join(settings.get("result_base_dir", ""), path)
        window = view.window() or sublime.active_window()
        window.open_file("{}:{}".format(path, row), sublime.ENCODED_POSITION)
