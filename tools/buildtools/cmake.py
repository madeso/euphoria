#!/usr/bin/python3
"""expose cmake utilities"""

import subprocess
import typing

import buildtools.core as core


class Argument:  # pylint: disable=too-few-public-methods
    """a cmake argument"""

    def __init__(self, a_name: str, a_value: str, a_type: typing.Optional[str]):
        self.name = a_name
        self.value = a_value
        self.type = a_type

    def format_cmake_argument(self) -> str:
        """format for commandline"""
        if self.type is None:
            return '-D{}={}'.format(self.name, self.value)

        return '-D{}:{}={}'.format(self.name, self.type, self.value)


class Generator:  # pylint: disable=too-few-public-methods
    """cmake generator"""
    def __init__(self, generator: str, arch: typing.Optional[str] = None):
        self.generator = generator
        self.arch = arch


class CMake:
    """utility to call cmake commands on a project"""
    def __init__(self, build_folder: str, source_folder: str, generator: Generator):
        self.generator = generator
        self.build_folder = build_folder
        self.source_folder = source_folder
        self.arguments = []

    def add_argument_with_type(self, a_name: str, a_value: str, a_type: str):
        """add argument with a explicit type set"""
        self.arguments.append(Argument(a_name, a_value, a_type))

    def add_argument(self, name: str, value: str):
        """add argument"""
        self.arguments.append(Argument(name, value, None))

    def set_install_folder(self, folder: str):
        """set the install folder"""
        self.add_argument_with_type('CMAKE_INSTALL_PREFIX', folder, 'PATH')

    def make_static_library(self):
        """set cmake to make static (not shared) library"""
        self.add_argument('BUILD_SHARED_LIBS', '0')

    def config(self, only_print: bool = False):
        """run cmake configure step"""
        command = ['cmake']
        for arg in self.arguments:
            argument = arg.format_cmake_argument()
            print('Setting CMake argument for config', argument, flush=True)
            command.append(argument)
        command.append(self.source_folder)
        command.append('-G')
        command.append(self.generator.generator)
        if self.generator.arch is not None:
            command.append('-A')
            command.append(self.generator.arch)
        core.verify_dir_exist(self.build_folder)
        if core.is_windows():
            if only_print:
                print(command, flush=True)
            else:
                core.flush()
                subprocess.check_call(command, cwd=self.build_folder)
        else:
            print('Configuring cmake', command, flush=True)

    def build_cmd(self, install: bool):
        """run cmake build step"""
        cmd = ['cmake', '--build', '.']
        if install:
            cmd.append('--target')
            cmd.append('install')
        cmd.append('--config')
        cmd.append('Release')
        if core.is_windows():
            core.flush()
            subprocess.check_call(cmd, cwd=self.build_folder)
        else:
            print('Calling build on cmake', self.build_folder, flush=True)

    def build(self):
        """build cmake project"""
        self.build_cmd(install=False)

    def install(self):
        """install cmake project"""
        self.build_cmd(install=True)
