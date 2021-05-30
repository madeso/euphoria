#!/usr/bin/python3
"""provides compiler and plaform enum and argparse related functions"""

import buildtools.core as core

from enum import Enum


class Compiler(Enum):
    """list of compilers"""
    VS2015 = 1
    VS2017 = 2
    VS2019 = 3


class Platform(Enum):
    """list of platforms"""
    AUTO = 0
    WIN32 = 1
    X64 = 2


def add_compiler(parser):
    """add compiler argument to argparse parser"""
    parser.add_argument('compiler', help='The compiler to use.')


def add_platform(parser):
    """add platform argument to argparse parser"""
    parser.add_argument('platform', help='The platform to build for.')


def get_compiler(args) -> Compiler:
    """get the compiler from the argparse arguments"""
    compiler_name = args.compiler

    if compiler_name == 'vs2015':
        return Compiler.VS2015

    if compiler_name == 'vs2017':
        return Compiler.VS2017

    if compiler_name == 'vs2019':
        return Compiler.VS2019

    # github actions installed compiler
    if compiler_name == 'windows-2016':
        return Compiler.VS2017
    if compiler_name == 'windows-2019':
        return Compiler.VS2019

    print('Unknown compiler: ', compiler_name, flush=True)
    return Compiler.VS2019


def get_platform(args) -> Platform:
    """get the platform from the argparse arguments"""
    platform = args.platform
    if platform == 'auto':
        return Platform.AUTO

    if platform.lower() == 'win32':
        return Platform.WIN32

    if platform.lower() in ['x64', 'win64']:
        return Platform.X64

    print('Unknown platform: ', platform, flush=True)
    return Platform.AUTO


#####################################################


def get_msbuild_toolset(compiler: Compiler) -> str:
    """get the msbuild tooselt name from the compiler"""
    if compiler == Compiler.VS2015:
        return 'v140'

    if compiler == Compiler.VS2017:
        return 'v141'

    if compiler == Compiler.VS2019:
        return 'v142'

    return 'invalid_compiler'


def is_64bit(platform: Platform) -> bool:
    """get if the platform is 64 bit or not"""
    if platform == Platform.WIN32:
        return False

    if platform == Platform.X64:
        return True

    if core.is_platform_64bit():
        return True

    return False


def platform_as_string(platform: Platform):
    """returns either a 64 or a 32 bit string identification"""
    if is_64bit(platform):
        return 'x64'

    return 'win32'
