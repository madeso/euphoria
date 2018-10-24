#!/usr/bin/env python3

import os
import requests
import subprocess
import buildtools.deps as deps
import buildtools.cmake as cmake
import buildtools.args as args
import buildtools.visualstudio as visualstudio


def get_root_folder():
    return os.getcwd()


def get_build_folder():
    return os.path.join(get_root_folder(), 'build', 'euph')


def get_dependency_folder():
    return os.path.join(get_root_folder(), 'build', 'deps')


def get_sdl2_folder():
    return os.path.join(get_dependency_folder(), 'sdl2')


def get_freetype2_folder():
    return os.path.join(get_dependency_folder(), 'freetype')


def get_assimp_folder():
    return os.path.join(get_dependency_folder(), 'assimp')

def get_assimp_install_folder():
    return os.path.join(get_assimp_folder(), 'cmake-install')


def get_sdl2_build_folder():
    return os.path.join(get_sdl2_folder(), 'cmake-build')


def cmake_project(generator: str):
    r = cmake.CMake(build_folder=get_build_folder(), source_folder=get_root_folder(), generator=generator) \
        .add_argument('SDL2_HINT_ROOT', get_sdl2_folder())\
        .add_argument('SDL2_HINT_BUILD', get_sdl2_build_folder())\
        .add_argument('ASSIMP_ROOT_DIR', get_assimp_install_folder())
    if 'PYTHON' in os.environ:
        r.add_argument('PYTHON_EXECUTABLE:FILEPATH', os.environ['PYTHON']+'\\python.exe')
    return r


def on_cmd_install(arg):
    compiler = args.get_compiler(arg)
    platform = args.get_platform(arg)
    generator = visualstudio.visual_studio_generator(compiler, platform)
    deps.install_dependency_assimp(get_dependency_folder(), get_assimp_folder(), get_assimp_install_folder(), generator)
    deps.install_dependency_sdl2(get_dependency_folder(), get_sdl2_folder(), get_sdl2_build_folder(), generator)
    deps.install_dependency_freetype(get_dependency_folder(), get_freetype2_folder(), compiler, platform)


def on_cmd_cmake(arg):
    compiler = args.get_compiler(arg)
    platform = args.get_platform(arg)
    generator = visualstudio.visual_studio_generator(compiler, platform)
    deps.setup_freetype_dependencies(get_freetype2_folder(), platform)
    cmake_project(generator).config()


def on_cmd_build(arg):
    compiler = args.get_compiler(arg)
    platform = args.get_platform(arg)
    generator = visualstudio.visual_studio_generator(compiler, platform)
    cmake_project(generator).build()


def run(args) -> str:
    try:
        return subprocess.check_output(args)
    except subprocess.CalledProcessError as e:
        return e.output


def on_cmd_test(args):
    tests = os.path.join(get_build_folder(), 'tests', 'Release', 'tests.exe')
    lines = run([tests, '-r', 'junit'])
    print('Test result', lines)
    url = 'https://ci.appveyor.com/api/testresults/junit/' + os.environ['APPVEYOR_JOB_ID']
    print('Uploading to appveyour', url)
    r = requests.post(url, files={'catch.json': lines})
    print(r.text)


def add_options(parser):
    args.add_compiler(parser)
    args.add_platform(parser)


def main():
    import argparse
    parser = argparse.ArgumentParser(description='Does the windows build')
    parser.set_defaults(func=None)
    subparsers = parser.add_subparsers()

    install_parser = subparsers.add_parser('install')
    install_parser.set_defaults(func=on_cmd_install)
    add_options(install_parser)

    cmmake_parser = subparsers.add_parser('cmake')
    cmmake_parser.set_defaults(func=on_cmd_cmake)
    add_options(cmmake_parser)

    build_parser = subparsers.add_parser('build')
    build_parser.set_defaults(func=on_cmd_build)
    add_options(build_parser)

    test_parser = subparsers.add_parser('test')
    test_parser.set_defaults(func=on_cmd_test)

    arg = parser.parse_args()
    if arg.func is None:
        parser.print_help()
    else:
        arg.func(arg)


if __name__ == "__main__":
    main()
