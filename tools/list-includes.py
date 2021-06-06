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


def is_limited(real_file: str, limit: typing.List[str]) -> bool:
    if len(limit) == 0:
        return False

    for l in limit:
        if real_file.startswith(l):
            return False

    return True


def calculate_identifier(file: str, name: typing.Optional[str]) -> str:
    return name or file.replace('/', '_').replace('.', '_').replace('-', '_').strip('_')


def calculate_display(file: str, name: typing.Optional[str], root: str) -> str:
    return name or os.path.relpath(file, root)


class Graphvizer:
    def __init__(self):
        self.nodes = {} # id -> name
        self.links = collections.Counter() # link with counts

    def print_result(self):
        print('digraph G')
        print('{')
        for ident, name in self.nodes.items():
            print(f'{ident} [label="{name}"];')
        print()
        for code, count in self.links.items():
            print(f'{code} [label="{count}"];')
        print('}')
        print('')

    def link(self, source: str, name: typing.Optional[str], resolved: str, root: str):
        from_node = calculate_identifier(source, name)
        to_node = calculate_identifier(resolved, None)
        self.links[f'{from_node} -> {to_node}'] += 1

        # probably will calc more than once but who cares?
        self.nodes[from_node] = calculate_display(source, name, root)
        self.nodes[to_node] = calculate_display(resolved, None, root)


def gv_work(real_file: str, name: typing.Optional[str], include_directories: typing.List[str], gv: Graphvizer, limit: typing.List[str], root: str):
    if is_limited(real_file, limit):
        return

    for include in list_includes(real_file):
        resolved = resolve_include_via_include_directories_or_none(include, include_directories)
        if resolved is not None:
            gv.link(real_file, name, resolved, root)
            # todo(Gustav): fix name to be based on root
            gv_work(resolved, None, include_directories, gv, limit, root)
        else:
            gv.link(real_file, name, include, root)


def work(real_file: str, include_directories: typing.List[str], counter, print_files: bool, limit: typing.List[str]):
    if is_limited(real_file, limit):
        return

    for include in list_includes(real_file):
        resolved = resolve_include_via_include_directories_or_none(include, include_directories)
        if resolved is not None:
            if print_files:
                print(resolved)
            counter[resolved] += 1
            work(resolved, include_directories, counter, print_files, limit)
        else:
            if print_files:
                print(f'Unable to find {include}')
            counter[include] += 1


def print_most_common(counter, count):
    for file, count in counter.most_common(count):
        print(f'{file}: {count}')


def all_translation_units(files: typing.List[str]) -> typing.Iterable[str]:
    for patt in files:
        for file in list_files_in_folder(patt, ['.cc']):
            yield os.path.realpath(file)


###################################################################################################
## handlers

def handle_list(args):
    compile_commands = load_compile_commands(args.compile_commands)

    total_counter = collections.Counter()
    max_counter = collections.Counter()

    limit = [os.path.realpath(f) for f in args.limit or []]

    files = 0

    for real_file in all_translation_units(args.files):
        files += 1
        file_counter = collections.Counter()
        if real_file in compile_commands:
            include_directories = list(get_include_directories(real_file, compile_commands))
            work(real_file, include_directories, file_counter, args.print_files, limit)

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
            if count>= args.count:
                print(f'{file} included in {count}/{files}')

    print()
    print()


def handle_gv(args):
    compile_commands = load_compile_commands(args.compile_commands)

    limit = [os.path.realpath(f) for f in args.limit or []]

    gv = Graphvizer()

    for real_file in all_translation_units(args.files):
        if real_file in compile_commands:
            include_directories = list(get_include_directories(real_file, compile_commands))
            gv_work(real_file, 'TU', include_directories, gv, limit, os.getcwd())

    gv.print_result()

###################################################################################################
# main

# todo(Gustav):
# find out who is including XXX.h and how it's included
# generate a union graphviz file that is a union of all includes with the TU as the root

def main():
    parser = argparse.ArgumentParser(description='do stuff')
    sub_parsers = parser.add_subparsers(dest='command_name', title='Commands', help='', metavar='<command>')

    sub = sub_parsers.add_parser('list', help='list headers from files')
    sub.add_argument('compile_commands')
    sub.add_argument('files', nargs='+')
    sub.add_argument('--print', dest='print_files', action='store_true')
    sub.add_argument('--print-stats', dest='print_stats', action='store_true')
    sub.add_argument('--print-max', dest='print_max', action='store_true')
    sub.add_argument('--no-list', dest='print_list', action='store_false')
    sub.add_argument('--count', default=2, type=int, help="only print includes that are more or equal to <count>")
    sub.add_argument('--limit', nargs='+', help="limit search to theese files and folders")
    sub.set_defaults(func=handle_list)

    sub = sub_parsers.add_parser('gv', help='generate a graphviz of the includes')
    sub.add_argument('compile_commands')
    sub.add_argument('files', nargs='+')
    sub.add_argument('--limit', nargs='+', help="limit search to theese files and folders")
    sub.set_defaults(func=handle_gv)

    args = parser.parse_args()
    if args.command_name is not None:
        args.func(args)
    else:
        parser.print_help()


##############################################################################

if __name__ == "__main__":
    try:
        main()
    except KeyboardInterrupt:
        pass
