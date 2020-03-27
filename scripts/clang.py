#!/usr/bin/env python3

"""
clang-tidy and clang-format related tools for the euphoria project
"""

import argparse
import os
import subprocess
import re
import collections
import sys


SOURCE_EXTENSIONS = ['.cc', '.h', '.cpp']

HEADER_SIZE = 80
HEADER_SPACING = 1
HEADER_START = 3

CMAKE_ADD_SUBDIRECTORY_REGEX = re.compile(r'add_subdirectory\(([^)]+)')

CLANG_TIDY_WARNING_CLASS = re.compile(r'\[(\w+(-\w+)+)\]')


def list_source_files(root, relative):
    """
    get all source files in root/relative
    """
    folder = os.path.join(root, relative)
    ret = [os.path.join(folder, f) for f in os.listdir(folder)
           if os.path.splitext(f.lower())[1] in SOURCE_EXTENSIONS]
    ret.sort()
    return ret


def find_projects(root):
    """
    Open the cmake root file in root and find all projects it references
    """
    with open(os.path.join(root, 'CMakeLists.txt'), 'r') as file:
        directories = CMAKE_ADD_SUBDIRECTORY_REGEX.findall(file.read())
        for directory in directories:
            if directory.startswith('external'):
                pass
            else:
                yield directory


def print_header(project_name, header_character='-'):
    """
    print a "pretty" header to the terminal
    """
    project = ' ' * HEADER_SPACING + project_name + ' ' * HEADER_SPACING
    start = header_character*HEADER_START
    left = HEADER_SIZE - (len(project) + HEADER_START)
    right = header_character*(left) if left > 1 else ''
    print(header_character * HEADER_SIZE)
    print(start+project+right)
    print(header_character * HEADER_SIZE)


def find_build_root(root):
    """
    Find the build folder containing the compile_commands file or None
    """
    for relative_build in ['build', 'build/debug-clang']:
        build = os.path.join(root, relative_build)
        compile_commands_json = os.path.join(build, 'compile_commands.json')
        if os.path.isfile(compile_commands_json):
            return build

    return None


def clang_tidy_lines(root):
    """
    return a iterator over the the "compiled" .clang-tidy lines
    """
    with open(os.path.join(root, 'clang-tidy'), 'r') as clang_tidy_file:
        write = False
        checks = []
        for line in clang_tidy_file:
            if write:
                yield line.rstrip()
            else:
                stripped_line = line.strip()
                if stripped_line == '':
                    pass
                elif stripped_line[0] == '#':
                    pass
                elif stripped_line == 'END_CHECKS':
                    write = True
                    checks_value = ','.join(checks)
                    yield 'Checks: "{}"'.format(checks_value)
                else:
                    checks.append(stripped_line)


def print_clang_tidy_source(root, clang_tidy_file):
    """
    print the clang-tidy "source"
    """
    for line in clang_tidy_lines(root):
        print(line, file=clang_tidy_file)


def make_clang_tidy(root):
    """
    write the .clang-tidy from the clang-tidy "source"
    """
    with open(os.path.join(root, '.clang-tidy'), 'w') as clang_tidy_file:
        print_clang_tidy_source(root, clang_tidy_file)


def call_clang_tidy(project_root, source_file):
    """
    runs clang-tidy and returns all the text output
    """
    command = ['clang-tidy', '-p', project_root, source_file]
    return subprocess.check_output(command, universal_newlines=True,
                                   encoding='utf8', stderr=subprocess.STDOUT)


def total(counter):
    """
    returns the total number of items in a counter
    """
    return sum(counter.values())


def run_clang_tidy(source_file, project_root):
    """
    runs the clang-tidy process, printing status to terminal
    """
    output = call_clang_tidy(project_root, source_file)
    warnings = collections.Counter()
    classes = collections.Counter()
    for line in output.split('\n'):
        if 'warnings generated' in line:
            pass
        elif '-header-filter' in line:
            pass
        elif 'Suppressed' in line and 'non-user code' in line:
            pass
        else:
            if 'warning: ' in line:
                warnings[source_file] += 1
                tidy_class = CLANG_TIDY_WARNING_CLASS.search(line)
                if tidy_class is not None:
                    classes[tidy_class.group(1)] += 1
            print(line)
    # print('{} warnings.'.format(total(warnings)))
    print_warning_counter(classes, source_file)
    print()
    return warnings, classes


def print_warning_counter(project_counter, project):
    """
    print warning counter to the console
    """
    print('{} warnings in {}.'.format(total(project_counter), project))
    for file, count in project_counter.most_common(10):
        print('{} at {}'.format(file, count))


##############################################################################
##############################################################################

def handle_format(args):
    """
    callback function called when running clang.py format
    """
    root = os.getcwd()
    for project in find_projects(root):
        print_header(project)
        source_files = list_source_files(root, project)
        for source_file in source_files:
            print(os.path.basename(source_file), flush=True)
            if args.nop is False:
                subprocess.call(['clang-format', '-i', source_file])
        print()


def handle_make_tidy(args):
    """
    callback function called when running clang.py make
    """
    root = os.getcwd()
    if args.nop:
        print_clang_tidy_source(root, sys.stdout)
    else:
        make_clang_tidy(root)


def handle_tidy(args):
    """
    callback function called when running clang.py tidy
    """
    root = os.getcwd()
    project_root = find_build_root(root)
    if project_root is None:
        print('unable to find build folder')
        return

    make_clang_tidy(root)

    total_counter = collections.Counter()
    total_classes = collections.Counter()

    for project in find_projects(root):
        print_header(project)
        project_counter = collections.Counter()
        source_files = list_source_files(root, project)
        for source_file in source_files:
            print(os.path.basename(source_file), flush=True)
            if args.nop is False:
                warnings, classes = run_clang_tidy(source_file, project_root)
                project_counter.update(warnings)
                total_classes.update(classes)

        print_warning_counter(project_counter, project)
        total_counter.update(project_counter)
        print()
        print()

    print_header('TIDY REPORT')
    print_warning_counter(total_counter, 'total')
    print()
    print_warning_counter(total_classes, 'classes')


##############################################################################


def main():
    """
    entry point function for running the clang.py script
    """
    parser = argparse.ArgumentParser(description='do clang stuff')
    sub_parsers = parser.add_subparsers(dest='command_name', title='Commands',
                                        help='', metavar='<command>')

    sub = sub_parsers.add_parser('make', help='make .clang-tidy')
    sub.add_argument('--nop', action='store_true', help="don't write anything")
    sub.set_defaults(func=handle_make_tidy)

    sub = sub_parsers.add_parser('tidy', help='do clang tidy on files')
    sub.add_argument('--nop', action='store_true', help="don't do anything")
    sub.set_defaults(func=handle_tidy)

    sub = sub_parsers.add_parser('format', help='do clang format on files')
    sub.add_argument('--nop', action='store_true', help="don't do anything")
    sub.set_defaults(func=handle_format)

    args = parser.parse_args()
    if args.command_name is not None:
        args.func(args)
    else:
        parser.print_help()


##############################################################################

if __name__ == "__main__":
    main()
