#!/usr/bin/env python3

"""
cmake functions

"""

import argparse
import os
import subprocess
import re
import collections
import sys
import json
import typing
import statistics
import itertools


class CmakeJson:
    def __init__(self, file: str, line: int, cmd: str, args: typing.List[str]):
        self.file = file
        self.line = line
        self.cmd = cmd
        self.args = args


def list_commands(build_root) -> typing.Iterable[CmakeJson]:
    cmake = ['cmake', '--trace-expand', '--trace-format=json-v1', build_root]
    output = subprocess.check_output(cmake, stderr=subprocess.STDOUT, text=True)
    lines = output.split('\n')

    for line in lines:
        try:
            command = json.loads(line)
            if 'cmd' in command:
                yield CmakeJson(command['file'], command['line'], command['cmd'], command['args'])
            else:
                pass
        except json.decoder.JSONDecodeError:
            pass


