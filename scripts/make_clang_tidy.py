#!/usr/bin/env python3

import os
import os.path


def lines(root):
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


def make(root):
    with open(os.path.join(root, '.clang-tidy'), 'w') as f:
        for l in lines(root):
            print(l, file=f)


def main():
    root = os.getcwd()
    make(root)

if __name__ == "__main__":
    main()

