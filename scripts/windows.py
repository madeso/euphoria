#!/usr/bin/python3

import os
import subprocess
import buildtools.core as core


def get_root_folder():
    return os.getcwd()


def get_build_folder():
    return os.path.join(get_root_folder(), 'build', 'euph')


def get_dependency_folder():
    return os.path.join(get_root_folder(), 'build', 'deps')


def get_proto_folder():
    return os.path.join(get_dependency_folder(), 'proto')


def on_cmd_install(args):
    core.install_dependency_proto(get_dependency_folder(), get_proto_folder(), True, core.get_vs_root(), core.get_current_platform())


def on_cmd_cmake(args):
    core.verify_dir_exist(get_build_folder())
    subprocess.check_call(['cmake', get_root_folder(), '-G', 'Visual Studio 15'], cwd=get_build_folder())


def on_cmd_build(args):
    subprocess.check_call(['cmake', '--build'], cwd=get_build_folder())


def main():
    import argparse
    parser = argparse.ArgumentParser(description='Does the windows build')
    subparsers = parser.add_subparsers()

    install_parser = subparsers.add_parser('install')
    install_parser.set_defaults(func=on_cmd_install)

    cmmake_parser = subparsers.add_parser('cmake')
    cmmake_parser.set_defaults(func=on_cmd_cmake)

    build_parser = subparsers.add_parser('build')
    build_parser.set_defaults(func=on_cmd_build)

    args = parser.parse_args()

    if args.func is None:
        pass
    else:
        args.func(args)


if __name__ == "__main__":
    main()
