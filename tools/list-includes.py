#!/usr/bin/env python3

"""
list include count in translation units

purpose: for fun and for what goes into a pch

"""

import argparse
import os
import subprocess
import re
import collections
import sys
import json
import typing
import statistics


class CompileCommand:
    def __init__(self, directory: str, command: str):
        self.directory = directory
        self.command = command

    def get_relative_includes(self) -> typing.Iterable[str]:
        # shitty comamndline parser... beware
        commands = self.command.split(' ')
        for c in commands:
            if c.startswith('-I'):
                yield c[2:]


def load_compile_commands(path: str) -> typing.Dict[str, CompileCommand]:
    with open(path, 'r') as handle:
        store = json.load(handle)

        r = {}
        for entry in store:
            r[entry['file']] = CompileCommand(entry['directory'], entry['command'])

        return r


def get_include_directories(path: str, cc: typing.Dict[str, CompileCommand]) -> typing.Iterable[str]:
    c = cc[path]
    for relative_include in c.get_relative_includes():
        yield os.path.normpath(os.path.join(c.directory, relative_include))


def list_files_in_folder(path: str, extensions: typing.Optional[typing.List[str]]) -> typing.Iterable[str]:
    if os.path.isfile(path):
        yield path
    else:
        for root, directories, files in os.walk(path):
            for file in files:
                ext = os.path.splitext(file)[1]
                if extensions is None or ext in extensions:
                    yield os.path.join(root, file)


def list_includes(path: str) -> typing.Iterable[str]:
    with open(path, 'r') as handle:
        for line in handle:
            l = line.strip()
            # beware... shitty c++ parser
            if l.startswith('#include'):
                yield l.split(' ')[1].strip().strip('"').strip('<').strip('>')


def resolve_include_via_include_directories_or_none(include: str, include_directories: typing.List[str]) -> typing.Optional[str]:
    for directory in include_directories:
        joined = os.path.join(directory, include)
        if os.path.isfile(joined):
            return joined
    return None


def work(real_file: str, include_directories: typing.List[str], counter, print_files: bool):
    for include in list_includes(real_file):
        resolved = resolve_include_via_include_directories_or_none(include, include_directories)
        if resolved is not None:
            if print_files:
                print(resolved)
            counter[resolved] += 1
            work(resolved, include_directories, counter, print_files)
        else:
            if print_files:
                print(f'Unable to find {include}')
            counter[include] += 1


def print_most_common(counter, count):
    for file, count in counter.most_common(count):
        print(f'{file}: {count}')

# todo(Gustav):
# find out who is including XXX.h and how it's included
# generate a union graphviz file that is a union of all includes with the TU as the root

def main():
    """
    entry point function for running the script
    """
    parser = argparse.ArgumentParser(description='do stuff')
    parser.add_argument('compile_commands')
    parser.add_argument('files', nargs='+')
    parser.add_argument('--print', dest='print_files', action='store_true')
    parser.add_argument('--print-stats', dest='print_stats', action='store_true')
    parser.add_argument('--print-max', dest='print_max', action='store_true')
    parser.add_argument('--no-list', dest='print_list', action='store_false')
    args = parser.parse_args()

    compile_commands = load_compile_commands(args.compile_commands)

    total_counter = collections.Counter()
    max_counter = collections.Counter()

    files = 0

    for patt in args.files:
        for file in list_files_in_folder(patt, ['.cc']):
            files += 1
            real_file = os.path.realpath(file)
            file_counter = collections.Counter()
            if real_file in compile_commands:
                include_directories = list(get_include_directories(real_file, compile_commands))
                work(real_file, include_directories, file_counter, args.print_files)

            if args.print_stats:
                print_most_common(file_counter, 10)
            total_counter.update(list(file_counter))
            max_counter = max_counter | file_counter

    if args.print_max:
        print()
        print()
        print('10 top number of includes for a translation unit')
        print_most_common(max_counter, 10)

    if args.print_list:
        print()
        print()
        print('Number of includes per translation unit')
        for file, count in sorted(total_counter.items(), key=lambda elem: elem[1], reverse=True):
            if count> 1:
                print(f'{file} included in {count}/{files}')

    print()
    print()


##############################################################################

if __name__ == "__main__":
    try:
        main()
    except KeyboardInterrupt:
        pass
