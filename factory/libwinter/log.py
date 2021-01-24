# Copyright (c) 2021 Alethea Katherine Flowers.
# Published under the standard MIT License.
# Full text available at: https://opensource.org/licenses/MIT

"""Logging helpers and such."""

import atexit
import datetime
from pathlib import Path
import sys
import traceback

from libwinter import tui

DEBUG_COLOR = (0.7, 0.7, 0.7)
INFO_COLOR = (0.8, 0.8, 1.0)
WARNING_COLOR = (1.0, 1.0, 0.4)
ERROR_COLOR = (1.0, 0.4, 0.4)
SUCCESS_COLOR = (0.5, 1.0, 0.5)


_log_file_path = Path("logs", f"{datetime.datetime.now().isoformat()}.log")
_log_file_path.parent.mkdir(parents=True, exist_ok=True)
_log_file = _log_file_path.open("w")


def _finish_up():
    _log_file.flush()
    _log_file.close()


atexit.register(_finish_up)


def _print_term(color, *args, sep=" ", end="\n", **kwargs):
    args = sep.join(f"{arg}" for arg in args)
    print(tui.rgb(color), args, tui.reset, sep="", end=end, **kwargs)


def _print_file(*args, sep=" ", end="\n", **kwargs):
    print(*args, sep=sep, end=end, **kwargs, file=_log_file)


def debug(*args, **kwargs):
    # _print_term(DEBUG_COLOR, *args, **kwargs)
    _print_file("[debug]   : ", *args, **kwargs)


def info(*args, **kwargs):
    _print_term(INFO_COLOR, *args, **kwargs)
    _print_file("[info]    : ", *args, **kwargs)


def warning(*args, **kwargs):
    _print_term(WARNING_COLOR, *args, **kwargs)
    _print_file("[warning] : ", *args, **kwargs)


def error(text, *args, exc=None, quit=True, **kwargs):
    if exc is not None:
        _print_term(ERROR_COLOR, f"{text}: {exc}", *args, **kwargs)
        _print_file(f"[error]   : {text}: {exc}", *args, **kwargs)
        traceback.print_exc(file=_log_file)
    else:
        _print_term(ERROR_COLOR, text, *args, **kwargs)
        _print_file(f"[error]   : {text}", *args, **kwargs)

    if quit:
        sys.exit(-1)


def success(*args, **kwargs):
    _print_term(SUCCESS_COLOR, *args, **kwargs)
    _print_file("[success] : ", *args, **kwargs)


def section(name, depth=1, **kwargs):
    marker = "►" * depth
    output = f"{tui.bold}{tui.underline}{marker} {name}\n"
    _print_term((0.5, 0.8, 1.0), output)
    marker = "#" * depth
    _print_file(f"\n{depth} {name}\n")
