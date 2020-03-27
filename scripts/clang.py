#!/usr/bin/env python3

import argparse
import os
import subprocess
import re
import collections


def sourcefiles(root, relative):
    EXTS = ['.cc', '.h', '.cpp']
    folder = os.path.join(root, relative)
    ret = [os.path.join(folder, f) for f in os.listdir(folder)
            if os.path.splitext(f.lower())[1] in EXTS]
    ret.sort()
    return ret


def find_projects(root):
    RDIRS = re.compile(r'add_subdirectory\(([^)]+)')
    with open(os.path.join(root, 'CMakeLists.txt'), 'r') as file:
        dirs = RDIRS.findall(file.read())
        for d in dirs:
            if d.startswith('external'):
                pass
            else:
                yield d


def print_header(project_name, ch='-'):
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


def find_build_root(root):
    for r in ['build', 'build/debug-clang']:
        b = os.path.join(root, r)
        c =  os.path.join(b, 'compile_commands.json')
        if os.path.isfile(c):
            return b

    return None


def clang_tidy_lines(root):
    with open(os.path.join(root, 'clang-tidy'), 'r') as f:
        write = False
        checks = []
        for line in f:
            if write:
                yield line.rstrip()
            else:
                s = line.strip()
                if s == '':
                    pass
                elif s[0] == '#':
                    pass
                elif s == 'END_CHECKS':
                    write = True
                    l = ','.join(checks)
                    yield 'Checks: "{}"'.format(l)
                else:
                    checks.append(s)


def make_tidy(root):
    with open(os.path.join(root, '.clang-tidy'), 'w') as f:
        for l in clang_tidy_lines(root):
            print(l, file=f)


##############################################################################
##############################################################################

def handle_format(args):
    root = os.getcwd()
    for project in find_projects(root):
        print_header(project)
        files = sourcefiles(root, project)
        for f in files:
            print(os.path.basename(f), flush=True)
            if args.nop is False:
                subprocess.call(['clang-format', '-i', f])
        print()


def handle_make_tidy(args):
    root = os.getcwd()
    make_tidy(root)


def handle_tidy(args):
    root = os.getcwd()
    project_root = find_build_root(root)
    if project_root is None:
        print('unable to find build folder')
        return

    make_tidy(root)

    total_warnings = 0
    total_top = collections.Counter()

    for project in find_projects(root):
        print_header(project)
        project_warnings = 0
        project_top = collections.Counter()
        files = sourcefiles(root, project)
        for f in files:
            print(os.path.basename(f), flush=True)
            if args.nop is False:
                t = subprocess.check_output(['clang-tidy', '-p', project_root, f], text=True, encoding='utf8', stderr=subprocess.STDOUT)
                file_warnings = 0
                for l in t.split('\n'):
                    if 'warnings generated' in l:
                        pass
                    elif '-header-filter' in l:
                        pass
                    elif 'Suppressed' in l and 'non-user code' in l:
                        pass
                    else:
                        if 'warning: ' in l:
                            total_warnings += 1
                            file_warnings += 1
                            project_warnings += 1
                            total_top[f] += 1
                            project_top[f] += 1
                        print(l)
                print('{} warnings.'.format(file_warnings))
                print()

        print('{} warnings in {}.'.format(project_warnings, project))
        for file, count in project_top.most_common(3):
            print('{} at {}'.format(file, count))
        print()
        print()

    print_header('TIDY REPORT')
    print('{} warnings in total.'.format(total_warnings))
    for file, count in total_top.most_common(3):
        print('{} at {}'.format(file, count))


##############################################################################


def main():
    parser = argparse.ArgumentParser(description='do clang stuff')
    sub_parsers = parser.add_subparsers(dest='command_name', title='Commands', help='', metavar='<command>')

    sub = sub_parsers.add_parser('make', help='make .clang-tidy')
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

