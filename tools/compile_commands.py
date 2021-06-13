#!/usr/bin/env python3

"""
compile comamnds functions

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


class CompileCommand:
    def __init__(self, directory: str, command: str):
        self.directory = directory
        self.command = command

    def get_relative_includes(self) -> typing.Iterable[str]:
        # shitty comamndline parser... beware
        commands = self.command.split(' ')
        for c in commands:
            if c.startswith('-I'):
                yield c[2:]


def load_compile_commands(path: str) -> typing.Dict[str, CompileCommand]:
    with open(path, 'r') as handle:
        store = json.load(handle)

        r = {}
        for entry in store:
            r[entry['file']] = CompileCommand(entry['directory'], entry['command'])

        return r


