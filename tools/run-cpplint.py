#!/usr/bin/env python3

import sys
import subprocess
import os
import argparse
import itertools


def list_files_in_dir(dir):
    for dp, dn, filenames in os.walk(dir):
        for f in filenames:
            valid_ext = os.path.splitext(f)[1] in ['.cc', '.h']
            if valid_ext and not f.startswith('pch.'):
                yield os.path.join(dp, f)


def list_all_files(root):
    return itertools.chain.from_iterable(list_files_in_dir(os.path.join(root, d)) for d in ['apps', 'libs'])


def handle_ls(_):
    files = list_all_files(os.getcwd())
    for f in files:
        print(f)


def print_err(path, stdout):
    print('*' * 80)
    print(path)
    print(stdout)
    print('*' * 80)
    print()


def run_file(path):
    ret = subprocess.run(['cpplint', path], stdout=subprocess.PIPE, stderr=subprocess.STDOUT, encoding="utf-8", universal_newlines=True)
    if ret.returncode != 0:
        print_err(path, ret.stdout)
        return (path, ret.stdout)
    else:
        print(path)
    return None


def handle_run(_):
    files = list_all_files(os.getcwd())
    errors = []
    for f in files:
        e = run_file(f)
        if e is not None:
            errors.append(e)

    if len(errors) > 0:
        for path, err in errors:
            print_err(path, err)
        sys.exit(f"cpplint checks failed with {len(errors)} errors")
    else:
        sys.exit(0)

# find libs/ apps/ -type f \( -iname \*.h -o -iname \*.cc \) | xargs cpplint

def main():
    parser = argparse.ArgumentParser(description='Runs cpplint on all sources')
    parser.set_defaults(func=None)
    subparsers = parser.add_subparsers()

    ls_parser = subparsers.add_parser('ls', help='list all files')
    ls_parser.set_defaults(func=handle_ls)

    run_parser = subparsers.add_parser('run', help='run all files')
    run_parser.set_defaults(func=handle_run)

    arg = parser.parse_args()
    if arg.func is None:
        parser.print_help()
    else:
        arg.func(arg)


if __name__ == "__main__":
    main()


