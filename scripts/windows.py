#!/usr/bin/python3

import os
import subprocess


def on_cmd_install(args):
    root = os.getcwd()
    print('The root is ', root)


def on_cmd_cmake(args):
    root = os.getcwd()
    print('The root is ', root)
    subprocess.call(['cmake', 'c:\projects\source', '-G', 'Visual Studio 15'])


def on_cmd_build(args):
    root = os.getcwd()
    print('The root is ', root)


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
