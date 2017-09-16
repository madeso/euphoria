#!/usr/bin/python3

import subprocess
import typing

import buildtools.core as core
import buildtools.visualstudio as visualstudio


class Argument:
    def __init__(self, name: str, value: str, type: typing.Optional[str]):
        self.name = name
        self.value = value
        self.type = type


class CMake:
    def __init__(self, build_folder: str, source_folder: str, generator: typing.Optional[str] = None):
        self.generator = generator if generator is not None else visualstudio.visual_studio_generator()
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

    def config(self):
        command = ['cmake']
        for arg in self.arguments:
            if arg.type is None:
                command.append('-D{}={}'.format(arg.name, arg.value))
            else:
                command.append('-D{}:{}={}'.format(arg.name, arg.type, arg.value))
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
        if core.is_windows():
            core.flush()
            subprocess.check_call(cmd, cwd=self.build_folder)
        else:
            print('Calling build on cmake', self.build_folder)

    def build(self):
        self.build_cmd(False)

    def install(self):
        self.build_cmd(True)

