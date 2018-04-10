#!/usr/bin/python3

import subprocess
import typing

import buildtools.core as core


class Argument:
    def __init__(self, name: str, value: str, type: typing.Optional[str]):
        self.name = name
        self.value = value
        self.type = type


class CMake:
    def __init__(self, build_folder: str, source_folder: str, generator: str):
        self.generator = generator
        self.build_folder = build_folder
        self.source_folder = source_folder
        self.arguments = []

    def add_argument_with_type(self, name: str, value: str, type: str) -> 'CMake':
        self.arguments.append(Argument(name, value, type))
        return self

    def add_argument(self, name: str, value: str) -> 'CMake':
        self.arguments.append( Argument(name, value, None) )
        return self

    def set_install_folder(self, folder: str) -> 'CMake':
        self.add_argument_with_type('CMAKE_INSTALL_PREFIX', folder, 'PATH')
        return self

    def make_static_library(self) -> 'CMake':
        self.add_argument('BUILD_SHARED_LIBS', '0')
        return self

    def config(self):
        command = ['cmake']
        for arg in self.arguments:
            argument = '-D{}={}'.format(arg.name, arg.value) if arg.type is None else '-D{}:{}={}'.format(arg.name, arg.type, arg.value)
            print('Setting CMake argument for config', argument)
            command.append(argument)
        command.append(self.source_folder)
        command.append('-G')
        command.append(self.generator)
        core.verify_dir_exist(self.build_folder)
        if core.is_windows():
            core.flush()
            subprocess.check_call(command, cwd=self.build_folder)
        else:
            print('Configuring cmake', command)

    def build_cmd(self, install: bool):
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
            print('Calling build on cmake', self.build_folder)

    def build(self):
        self.build_cmd(False)

    def install(self):
        self.build_cmd(True)

