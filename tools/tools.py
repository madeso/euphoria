#!/usr/bin/env python3

"""
various smaller tools for investigating code health and getting statistics
"""

# todo(Gustav):
# include:
#     find out who is including XXX.h and how it's included
#     generate a union graphviz file that is a union of all includes with the TU as the root

import argparse
import os
import subprocess
import re
import collections
import sys
import json
import typing
import statistics
import itertools

import compile_commands as cc
import cmake


HEADER_FILES = ['.h', '.hpp', '.hxx']
SOURCE_FILES = ['.cc', '.cpp', '.cxx', '.inl']


def list_files_in_folder(path: str, extensions: typing.Optional[typing.List[str]]) -> typing.Iterable[str]:
    if os.path.isfile(path):
        yield path
    else:
        for root, directories, files in os.walk(path):
            for file in files:
                ext = os.path.splitext(file)[1]
                if extensions is None or ext in extensions:
                    yield os.path.join(root, file)


def get_include_directories(path: str, cc: typing.Dict[str, cc.CompileCommand]) -> typing.Iterable[str]:
    c = cc[path]
    for relative_include in c.get_relative_includes():
        yield os.path.normpath(os.path.join(c.directory, relative_include))


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


def get_group(relative_file: str) -> str:
    b = relative_file.split('/')
    if len(b) == 1:
        return ''
    r = b[1] if b[0] in ['libs', 'external'] else b[0]
    if r == '..':
        return ''
    return r


def get_grouped(items):
    keyfun = lambda item: get_group(item[1])
    return itertools.groupby(sorted(items, key=keyfun), key=keyfun)


class Graphvizer:
    def __init__(self):
        self.nodes = {} # id -> name
        self.links = collections.Counter() # link with counts

    def print_result(self, group: bool, is_cluster: bool):
        print('digraph G')
        print('{')
        grouped = get_grouped(self.nodes.items()) if group else [('', self.nodes.items())]
        for group_title, items in grouped:
            has_group = group_title != ''
            indent = '    ' if has_group else ''

            if has_group:
                cluster_prefix = 'cluster_' if is_cluster else ''
                print(f'subgraph {cluster_prefix}{group_title}')
                print('{')

            for identifier, name in items:
                print(f'{indent}{identifier} [label="{name}"];')

            if has_group:
                print('}')
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


def get_line_count(path: str, discard_empty: bool) -> int:
    with open(path, 'r') as handle:
        count = 0
        for line in handle:
            if discard_empty and len(line.strip()) == 0:
                pass
            else:
                count += 1
        return count

    return -1


def get_all_indent(path: str, discard_empty: bool) -> typing.Iterable[int]:
    with open(path, 'r') as handle:
        count = 0
        for line in handle:
            if discard_empty and len(line.strip()) == 0:
                pass
            else:
                yield len(line) - len(line.lstrip())


def get_max_indent(path: str, discard_empty: bool) -> int:
    count = 0

    got_files = False
    for line_count in get_all_indent(path, discard_empty):
        count = max(count, line_count)
        got_files = True
    
    return count if got_files else -1


def contains_pragma_once(path: str) -> bool:
    with open(path, 'r') as handle:
        for line in handle:
            if '#pragma once' in line:
                return True

    return False


def file_is_in_folder(file: str, folder: str) -> bool:
    return os.path.commonprefix([folder, file]) == folder



###################################################################################################
## handlers

def handle_missing_include_guards(args):
    count = 0
    files = 0

    for patt in args.files:
        for file in list_files_in_folder(patt, ['.h']):
            files += 1
            if not contains_pragma_once(file):
                print(file)
                count += 1

    print(f'Found {count} in {files} files.')


def float_to_block_str(f) -> str:
    block_width_char = ["▏", "▎", "▍", "▌", "▋", "▊", "▉", "█"]
    size = len(block_width_char)
    count = max(int(f * size), 1)
    r = ""
    while count > 0:
        bi = len(block_width_char)-1 if count >= len(block_width_char) else count
        r += block_width_char[bi]
        count -= size
    return r


def handle_list_indents(args):
    stats = {}
    files = 0
    each = args.each

    for patt in args.files:
        for file in list_files_in_folder(patt, HEADER_FILES + SOURCE_FILES):
            files += 1

            counts = list(get_all_indent(file, args.discard_empty)) if args.hist else [get_max_indent(file, args.discard_empty)]

            for count in counts:
                index = count if each <= 1 else count - (count % each)
                if index in stats:
                    stats[index].append(file)
                else:
                    stats[index] = [file]

    all_sorted = sorted(stats.items(), key=lambda item: item[0])
    print(f'Found {files} files.')
    
    total_sum = 0
    if args.hist:
        for _count,files in all_sorted:
            total_sum = max(total_sum, len(files))
    
    for count,files in all_sorted:
        c = len(files)
        count_str = f'{count}' if each <= 1 else f'{count}-{count+each-1}'
        if args.hist:
            hist_width = 50
            # chars = '█' * max(int((c * hist_width)/total_sum), 1)
            chars = float_to_block_str((c * hist_width)/total_sum)
            print(f'{count_str:<6}: {chars}')
        elif args.show and c < 3:
            print(f'{count_str}: {files}')
        else:
            print(f'{count_str}: {c}')


def handle_list(args):
    compile_commands_arg = cc.get_argument_or_none(args)
    if compile_commands_arg is None:
        return
    compile_commands = cc.load_compile_commands(compile_commands_arg)

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
    compile_commands_arg = cc.get_argument_or_none(args)
    if compile_commands_arg is None:
        return
    compile_commands = cc.load_compile_commands(compile_commands_arg)

    limit = [os.path.realpath(f) for f in args.limit or []]

    gv = Graphvizer()

    for real_file in all_translation_units(args.files):
        if real_file in compile_commands:
            include_directories = list(get_include_directories(real_file, compile_commands))
            gv_work(real_file, 'TU', include_directories, gv, limit, os.getcwd())

    gv.print_result(args.group or args.cluster, args.cluster)


def handle_list_no_project_folder(args):
    bases = [os.path.realpath(f) for f in args.files]

    compile_commands_arg = cc.get_argument_or_none(args)
    if compile_commands_arg is None:
        return
    build_root = os.path.dirname(compile_commands_arg)

    projects = set()
    projects_with_folders = set()
    files = {}
    project_folders = collections.Counter()

    for cmd in cmake.list_commands(build_root):
        if any(file_is_in_folder(cmd.file, b) for b in bases):
            if cmd.cmd.lower() in ['add_library', 'add_executable']:
                project_name = cmd.args[0]
                if cmd.args[1] not in ['INTERFACE']: # skip interface libraries
                    projects = projects | set([project_name])
                    files[project_name] = cmd.file
            if cmd.cmd.lower() == 'set_target_properties':
                # set_target_properties(core PROPERTIES FOLDER "Libraries")
                if cmd.args[1] == 'PROPERTIES' and cmd.args[2] == 'FOLDER':
                    projects_with_folders = projects_with_folders | set([cmd.args[0]])
                    project_folders.update([cmd.args[3]])

    sort_on_cmake = lambda x: x[1]
    missing = projects - projects_with_folders
    total_missing = len(missing)
    missing = sorted(((m, files[m]) for m in missing), key=sort_on_cmake)

    missing_files = 0

    for cmake_file, files in itertools.groupby(missing, sort_on_cmake):
        missing_files += 1
        print(os.path.relpath(cmake_file))
        sorted_files = sorted((f[0] for f in files))
        for f in sorted_files:
            print(f'    {f}')
        if len(sorted_files) > 1:
            print(f'    = {len(sorted_files)} projects')
        print()
    print(f'Found missing: {total_missing} projects in {missing_files} files')
    print_most_common(project_folders, 10)


def handle_missing_in_cmake(args):
    bases = [os.path.realpath(f) for f in args.files]

    compile_commands_arg = cc.get_argument_or_none(args)
    if compile_commands_arg is None:
        return
    build_root = os.path.dirname(compile_commands_arg)

    paths = set()

    for cmd in cmake.list_commands(build_root):
        if any(file_is_in_folder(cmd.file, b) for b in bases):
            if cmd.cmd.lower() == 'add_library':
                paths = paths | set(cmake.list_files_in_cmake_library(cmd))
            if cmd.cmd.lower() == 'add_executable':
                paths = paths | set(cmake.list_files_in_cmake_executable(cmd))

    count = 0
    for patt in args.files:
        for file in list_files_in_folder(patt, HEADER_FILES + SOURCE_FILES):
            resolved = os.path.abspath(file)
            if resolved not in paths:
                print(resolved)
                count += 1

    print(f'Found {count} files not referenced in cmake')


def handle_line_count(args):
    stats = {}
    files = 0
    each = args.each

    for patt in args.files:
        for file in list_files_in_folder(patt, HEADER_FILES + SOURCE_FILES):
            files += 1

            count = get_line_count(file, args.discard_empty)

            index = count if each <= 1 else count - (count % each)
            if index in stats:
                stats[index].append(file)
            else:
                stats[index] = [file]

    print(f'Found {files} files.')
    for count,files in sorted(stats.items(), key=lambda item: item[0]):
        c = len(files)
        count_str = f'{count}' if each <= 1 else f'{count}-{count+each-1}'
        if args.show and c < 3:
            print(f'{count_str}: {files}')
        else:
            print(f'{count_str}: {c}')


def handle_check_files(args):
    files = 0
    errors = 0

    for patt in args.files:
        for file in list_files_in_folder(patt, HEADER_FILES + SOURCE_FILES):
            files += 1
            if '-' in file:
                errors +=1
                print(f'file name mismatch: {file}')

    print(f'Found {errors} errors in {files} files.')
    
    sys.exit(-1 if errors > 0 else 0)



###################################################################################################


def main():
    """
    entry point function for running the script
    """
    parser = argparse.ArgumentParser(description='investigate code health and random statistics')
    sub_parsers = parser.add_subparsers(dest='command_name', title='Commands', help='', metavar='<command>')

    sub = sub_parsers.add_parser('line-count', help='list line counts')
    sub.add_argument('files', nargs='+', help='files or folders to look in')
    sub.add_argument('--each', type=int, default=1)
    sub.add_argument('--show', action='store_true')
    sub.add_argument('--include-empty', dest='discard_empty', action='store_false')
    sub.set_defaults(func=handle_line_count)

    sub = sub_parsers.add_parser('include-list', help='list headers from files')
    cc.add_argument(sub)
    sub.add_argument('files', nargs='+')
    sub.add_argument('--print', dest='print_files', action='store_true')
    sub.add_argument('--print-stats', dest='print_stats', action='store_true')
    sub.add_argument('--print-max', dest='print_max', action='store_true')
    sub.add_argument('--no-list', dest='print_list', action='store_false')
    sub.add_argument('--count', default=2, type=int, help="only print includes that are more or equal to <count>")
    sub.add_argument('--limit', nargs='+', help="limit search to theese files and folders")
    sub.set_defaults(func=handle_list)

    sub = sub_parsers.add_parser('include-gv', help='generate a graphviz of the includes')
    cc.add_argument(sub)
    sub.add_argument('files', nargs='+')
    sub.add_argument('--limit', nargs='+', help="limit search to theese files and folders")
    sub.add_argument('--group', action='store_true', help="group output")
    sub.add_argument('--cluster', action='store_true', help="group output into clusters")
    sub.set_defaults(func=handle_gv)

    sub = sub_parsers.add_parser('list-indents', help='list the files with the maximum indents')
    sub.add_argument('files', nargs='+')
    sub.add_argument('--each', type=int, default=1, help='group counts')
    sub.add_argument('--show', action='store_true', help='include files in list')
    sub.add_argument('--hist', action='store_true', help='show simple histogram')
    sub.add_argument('--include-empty', dest='discard_empty', action='store_false')
    sub.set_defaults(func=handle_list_indents)

    sub = sub_parsers.add_parser('missing-pragma-once', help='find headers with missing include guards')
    sub.add_argument('files', nargs='+')
    sub.set_defaults(func=handle_missing_include_guards)

    sub = sub_parsers.add_parser('missing-in-cmake', help='find files that existis on disk but missing in cmake')
    sub.add_argument('files', nargs='+')
    cc.add_argument(sub)
    sub.set_defaults(func=handle_missing_in_cmake)

    sub = sub_parsers.add_parser('list-no-project-folders', help='find projects that have not set the solution folder')
    sub.add_argument('files', nargs='+')
    cc.add_argument(sub)
    sub.set_defaults(func=handle_list_no_project_folder)

    sub = sub_parsers.add_parser('check-files', help="find files that doesn't match the name style")
    sub.add_argument('files', nargs='+')
    cc.add_argument(sub)
    sub.set_defaults(func=handle_check_files)

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
