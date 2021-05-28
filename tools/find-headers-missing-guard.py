#!/usr/bin/env python3

"""
find header files not containing pragma once
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


def list_files_in_folder(path: str, extensions: typing.Optional[typing.List[str]]):
    for root, directories, files in os.walk(path):
        for file in files:
            ext = os.path.splitext(file)[1]
            if extensions is None or ext in extensions:
                yield os.path.join(root, file)


def contains_pragma_once(path: str) -> bool:
    with open(path, 'r') as handle:
        for line in handle:
            if '#pragma once' in line:
                return True

    return False


def main():
    """
    entry point function for running the script
    """
    parser = argparse.ArgumentParser(description='do stuff')
    parser.add_argument('files', nargs='+')
    args = parser.parse_args()

    count = 0
    files = 0

    for patt in args.files:
        for file in list_files_in_folder(patt, ['.h']):
            files += 1
            if not contains_pragma_once(file):
                print(file)
                count += 1

    print(f'Found {count} in {files} files.')


##############################################################################

if __name__ == "__main__":
    try:
        main()
    except KeyboardInterrupt:
        pass
