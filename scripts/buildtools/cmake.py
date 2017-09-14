#!/usr/bin/python3

import subprocess
import typing

import buildtools.core as core
import buildtools.visualstudio as visualstudio


class CMake:
    def __init__(self, build_folder: str, source_folder: str, generator: typing.Optional[str] = None):
        self.generator = generator if generator is not None else visualstudio.visual_studio_generator()
        self.build_folder = build_folder
        self.source_folder = source_folder
        self.arguments = []

    def add_argument(self, name: str, value: str) -> 'CMake':
        self.arguments.append( {name: name, value: value} )
        return self

    def config(self):
        command = ['cmake']
        for arg in self.arguments:
            command.append('-D{}={}'.format(arg.name, arg.value))
        command.append(self.source_folder)
        command.append('-G')
        command.append(self.generator)
        core.verify_dir_exist(self.build_folder)
        if core.is_windows():
            core.flush()
            subprocess.check_call(command, cwd=self.build_folder)
        else:
            print('Configuring cmake', command)

    def build(self):
        if core.is_windows():
            core.flush()
            subprocess.check_call(['cmake', '--build', '.'], cwd=self.build_folder)
        else:
            print('Calling build on cmake', self.build_folder)

