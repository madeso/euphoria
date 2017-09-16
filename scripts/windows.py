#!/usr/bin/python3

import os
import buildtools.visualstudio as visualstudio
import buildtools.deps as deps
import buildtools.cmake as cmake


def get_root_folder():
    return os.getcwd()


def get_build_folder():
    return os.path.join(get_root_folder(), 'build', 'euph')


def get_dependency_folder():
    return os.path.join(get_root_folder(), 'build', 'deps')


def get_proto_folder():
    return os.path.join(get_dependency_folder(), 'proto')


def get_sdl2_folder():
    return os.path.join(get_dependency_folder(), 'sdl2')


def get_freetype2_folder():
    return os.path.join(get_dependency_folder(), 'freetype')


def get_wx_folder():
    return os.path.join(get_dependency_folder(), 'wx')


def get_assimp_folder():
    return os.path.join(get_dependency_folder(), 'assimp')


def get_sdl2_build_folder():
    return os.path.join(get_sdl2_folder(), 'cmake-build')


def on_cmd_install(args):
    deps.install_dependency_assimp(get_dependency_folder(), get_assimp_folder())
    deps.install_dependency_proto(get_dependency_folder(), get_proto_folder())
    deps.install_dependency_sdl2(get_dependency_folder(), get_sdl2_folder(), get_sdl2_build_folder())
    deps.install_dependency_freetype(get_dependency_folder(), get_freetype2_folder())
    deps.install_dependency_wx(get_dependency_folder(), get_wx_folder())


def cmake_project():
    return cmake.CMake(build_folder=get_build_folder(), source_folder=get_root_folder())\
        .add_argument('SDL2_HINT_ROOT', get_sdl2_folder())\
        .add_argument('SDL2_HINT_BUILD', get_sdl2_build_folder())\
        .add_argument('wxWidgets_ROOT_DIR', get_wx_folder())\
        .add_argument('ASSIMP_ROOT_DIR', get_assimp_folder())\
        .add_argument('PROTOBUF_SRC_ROOT_FOLDER', get_proto_folder())


def on_cmd_cmake(args):
    deps.setup_freetype_dependencies(get_freetype2_folder())
    cmake_project().config()


def on_cmd_build(args):
    cmake_project().build()


def main():
    import argparse
    parser = argparse.ArgumentParser(description='Does the windows build')
    parser.set_defaults(func=None)
    subparsers = parser.add_subparsers()

    install_parser = subparsers.add_parser('install')
    install_parser.set_defaults(func=on_cmd_install)

    cmmake_parser = subparsers.add_parser('cmake')
    cmmake_parser.set_defaults(func=on_cmd_cmake)

    build_parser = subparsers.add_parser('build')
    build_parser.set_defaults(func=on_cmd_build)

    args = parser.parse_args()

    if args.func is None:
        parser.print_help()
    else:
        args.func(args)


if __name__ == "__main__":
    main()
