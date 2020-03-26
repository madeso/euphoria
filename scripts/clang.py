#!/usr/bin/env python3

import argparse
import os
import subprocess
import re

def sourcefiles(relative):
    EXTS = ['.cc', '.h', '.cpp']
    folder = os.path.join(os.getcwd(), relative)
    ret = [os.path.join(folder, f) for f in os.listdir(folder)
            if os.path.splitext(f.lower())[1] in EXTS]
    ret.sort()
    return ret


def find_projects():
    RDIRS = re.compile(r'add_subdirectory\(([^)]+)')
    with open(os.path.join(os.getcwd(), 'CMakeLists.txt'), 'r') as file:
        dirs = RDIRS.findall(file.read())
        for d in dirs:
            if d.startswith('external'):
                pass
            else:
                yield d


def header(project_name, ch='-'):
    SIZE = 80
    WIDE = 1
    START = 3
    project = ' ' * WIDE + project_name + ' ' * WIDE
    start = ch*START
    left = SIZE - (len(project) + START)
    right = ch*(left) if left > 1 else ''
    print(ch * SIZE)
    print(start+project+right)
    print(ch * SIZE)


def handle_format(args):
    for project in find_projects():
        header(project)
        files = sourcefiles(project)
        for f in files:
            print(os.path.basename(f))
            # subprocess.call(['clang-format', '-i', f])
        print()


def main():
    parser = argparse.ArgumentParser(description='do clang stuff')
    sub_parsers = parser.add_subparsers(dest='command_name', title='Commands', help='', metavar='<command>')

    sub = sub_parsers.add_parser('format', help='do clang format on files')
    sub.set_defaults(func=handle_format)

    args = parser.parse_args()
    if args.command_name is not None:
        args.func(args)
    else:
        parser.print_help()


if __name__ == "__main__":
    main()

