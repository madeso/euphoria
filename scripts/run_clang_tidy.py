#!/usr/bin/env python3

import re
import os
import os.path
import subprocess


def find_projects():
    r = re.compile(r'add_subdirectory\((\w+)\)')
    with open('CMakeLists.txt', 'r') as f:
        for l in f:
            m = r.search(l)
            if m is not None:
                yield m.group(1)


def run_clang_tidy(project_root, file):
    print(file, flush=True)
    args = ['clang-tidy', '-p', project_root, file]
    subprocess.run(args)


def run_clang_tidy_on_all_files(root, project_file):
    for pp in find_projects():
        folder = os.path.join(root, pp)
        files = (os.path.join(folder, file) for file in os.listdir(folder)
                if os.path.isfile(os.path.join(folder,file))
                and os.path.splitext(file)[1] in ['.cc', '.h']
                )
        print(''.join('#' for _ in range(80)))
        print('##', pp)
        for f in files:
            run_clang_tidy(project_file, f)

        print()


def find_build_root(root):
    for r in ['build', 'build/debug-clang']:
        b = os.path.join(root, r)
        c =  os.path.join(b, 'compile_commands.json')
        if os.path.isfile(c):
            return b

    return None


def main():
    root = os.getcwd()
    project_file = find_build_root(root)

    if project_file is None:
        print('unable to find build root')
        return

    import make_clang_tidy
    make_clang_tidy.make(root)

    run_clang_tidy_on_all_files(root, project_file)


if __name__ == "__main__":
    main()

