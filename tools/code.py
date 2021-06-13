#!/usr/bin/env python3

"""
various smaller tools for investigating code health and getting statistics
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


HEADER_FILES = ['.h', '.hpp', '.hxx']
SOURCE_FILES = ['.cc', '.cpp', '.cxx', '.inl']


def list_files_in_folder(path: str, extensions: typing.Optional[typing.List[str]]):
    for root, directories, files in os.walk(path):
        for file in files:
            ext = os.path.splitext(file)[1]
            if extensions is None or ext in extensions:
                yield os.path.join(root, file)


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
