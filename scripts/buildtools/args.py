#!/usr/bin/python3

import buildtools.core as core

import argparse
from enum import Enum


class Compiler(Enum):
    VS2015 = 1
    VS2017 = 2


class Platform(Enum):
    AUTO = 0
    WIN32 = 1
    X64 = 2


def add_compiler(parser):
    pass


def add_platform(parser):
    pass


def get_compiler(args) -> Compiler:
    pass


def get_platform(args) -> Platform:
    pass


#####################################################


def get_msbuild_toolset(c: Compiler) -> str:
    if c == Compiler.VS2015:
        return '140'
    if c == Compiler.VS2017:
        return '141'
    return 'invalid_compiler'


def is_64bit(p: Platform) -> bool:
    if p == Platform.WIN32:
        return False
    elif p == Platform.X64:
        return True
    else:
        if core.is_platform_64bit():
            return True
        else:
            return False


def platform_as_string(p: Platform):
    if is_64bit(p):
        return 'x64'
    else:
        return 'win32'
