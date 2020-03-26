#!/usr/bin/env python3


def lines():
    with open('clang-tidy', 'r') as f:
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


def main():
    with open('.clang-tidy', 'w') as f:
        for l in lines():
            print(l, file=f)


if __name__ == "__main__":
    main()

