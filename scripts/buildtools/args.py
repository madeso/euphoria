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
    parser.add_argument('compiler', help='The compiler to use.')


def add_platform(parser):
    parser.add_argument('platform', help='The platform to build for.')


def get_compiler(args) -> Compiler:
    c = args.compiler
    if c == 'vs2015':
        return Compiler.VS2015
    elif c == 'vs2017':
        return Compiler.VS2017
    else:
        print('Unknown compiler: ', c)
        return Compiler.VS2017


def get_platform(args) -> Platform:
    p = args.platform
    if p == 'auto':
        return Platform.AUTO
    elif p == 'win32':
        return Platform.WIN32
    elif p == 'x64' or p == 'win64':
        return Platform.X64
    else:
        print('Unknown platform: ', p)
        return Platform.AUTO


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
