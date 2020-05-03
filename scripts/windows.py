#!/usr/bin/env python3
"""build script for windows for euphoria"""

import os
import subprocess
import argparse
import buildtools.deps as btdeps
import buildtools.cmake as btcmake
import buildtools.args as btargs
import buildtools.visualstudio as btstudio


###############################################################################


ROOT_FOLDER = os.getcwd()
BUILD_FOLDER = os.path.join(ROOT_FOLDER, 'build', 'euph')
DEPENDENCY_FOLDER = os.path.join(ROOT_FOLDER, 'build', 'deps')
SDL2_FOLDER = os.path.join(DEPENDENCY_FOLDER, 'sdl2')
SDL2_BUILD_FOLDER = os.path.join(SDL2_FOLDER, 'cmake-build')
FREETYPE2_FOLDER = os.path.join(DEPENDENCY_FOLDER, 'freetype')
ASSIMP_FOLDER = os.path.join(DEPENDENCY_FOLDER, 'assimp')
ASSIMP_INSTALL_FOLDER = os.path.join(ASSIMP_FOLDER, 'cmake-install')


###############################################################################


def generate_cmake_project(generator):
    """generate the euphoira project"""
    project = btcmake.CMake(BUILD_FOLDER, ROOT_FOLDER, generator)
    project.add_argument('SDL2_HINT_ROOT', SDL2_FOLDER)
    project.add_argument('SDL2_HINT_BUILD', SDL2_BUILD_FOLDER)
    project.add_argument('ASSIMP_ROOT_DIR', ASSIMP_INSTALL_FOLDER)
    if 'PYTHON' in os.environ:
        python_exe = os.environ['PYTHON']+'\\python.exe'
        project.add_argument('PYTHON_EXECUTABLE:FILEPATH', python_exe)
    return project


def run_install(compiler, platform, generator):
    """install euphoria dependencies"""
    btdeps.install_dependency_assimp(DEPENDENCY_FOLDER, ASSIMP_FOLDER,
                                     ASSIMP_INSTALL_FOLDER, generator)
    btdeps.install_dependency_sdl2(DEPENDENCY_FOLDER, SDL2_FOLDER,
                                   SDL2_BUILD_FOLDER, generator)
    btdeps.install_dependency_freetype(DEPENDENCY_FOLDER, FREETYPE2_FOLDER,
                                       compiler, platform)


def run_cmake(platform, generator):
    """configure the euphoria cmake project"""
    btdeps.setup_freetype_dependencies(FREETYPE2_FOLDER, platform)
    generate_cmake_project(generator).config()


def run(args) -> str:
    """run a terminal and return the output or print error"""
    try:
        return subprocess.check_output(args, stderr=subprocess.STDOUT).decode('utf-8')
    except subprocess.CalledProcessError as error:
        print('Failed to run {} error: {}'.format(args, error.output))
        return ''


###############################################################################


def on_cmd_install(arg):
    """callback for install command"""
    compiler = btargs.get_compiler(arg)
    platform = btargs.get_platform(arg)
    generator = btstudio.visual_studio_generator(compiler, platform)

    run_install(compiler, platform, generator)


def on_cmd_cmake(arg):
    """callback for cmake command"""
    compiler = btargs.get_compiler(arg)
    platform = btargs.get_platform(arg)
    generator = btstudio.visual_studio_generator(compiler, platform)

    run_cmake(platform, generator)


def on_cmd_dev(arg):
    """callback for dev command"""
    compiler = btargs.get_compiler(arg)
    platform = btargs.get_platform(arg)
    generator = btstudio.visual_studio_generator(compiler, platform)

    run_install(compiler, platform, generator)
    run_cmake(platform, generator)


def on_cmd_build(arg):
    """callback for build cmd"""
    compiler = btargs.get_compiler(arg)
    platform = btargs.get_platform(arg)
    generator = btstudio.visual_studio_generator(compiler, platform)

    generate_cmake_project(generator).build()


def on_cmd_test(_):
    """callback for test cmd"""
    tests = os.path.join(BUILD_FOLDER, 'tests', 'Release', 'tests.exe')
    print('Running tests {}'.format(tests))
    lines = run([tests, '-r', 'junit'])
    print('Test result:')
    for l in lines.splitlines():
        print(l)
    print('######################################')
    # hacky way to remove all log output from the junit output
    lines = '\n'.join(line for line in lines.splitlines() if line[:1] != '[')
    save_path = os.path.join(ROOT_FOLDER, 'build', 'junit-results.xml')
    for l in lines:
        print(l)
    print('######################################')
    print('Saving junit to', save_path, flush=True)
    with open(save_path, 'w') as junit_file:
        junit_file.write(lines)
    print('junit file written!')


###############################################################################


def add_options(parser):
    """add compiler and platform options"""
    btargs.add_compiler(parser)
    btargs.add_platform(parser)


###############################################################################


def main():
    """entry point for script"""
    parser = argparse.ArgumentParser(description='Does the windows build')
    parser.set_defaults(func=None)
    subparsers = parser.add_subparsers()

    install_parser = subparsers.add_parser('install')
    install_parser.set_defaults(func=on_cmd_install)
    add_options(install_parser)

    cmmake_parser = subparsers.add_parser('cmake')
    cmmake_parser.set_defaults(func=on_cmd_cmake)
    add_options(cmmake_parser)

    # dev is install + cmake
    dev_parser = subparsers.add_parser('dev')
    dev_parser.set_defaults(func=on_cmd_dev)
    add_options(dev_parser)

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
