#!/usr/bin/env python3
"""build script for windows for euphoria"""


import buildtools.windows as build

# todo(Gustav): add test command to windows build
def on_cmd_test(_):
    """callback for test cmd"""
    # todo(Gustav): figure out how to run all tests
    tests = os.path.join(BUILD_FOLDER, 'libs', 'core', 'Release', 'core_test.exe')
    print('Running tests {}'.format(tests))
    lines = run([tests, '-r', 'junit']).decode('utf-8')
    print('Test result: {}'.format(len(lines.splitlines())))
    # hacky way to remove all log output from the junit output
    lines = '\n'.join(line for line in lines.splitlines() if line[:1] != '[')
    save_path = os.path.join(ROOT_FOLDER, 'build', 'junit-results.xml')
    print('Saving {} lines of junit to {}'.format(len(lines), save_path), flush=True)
    with open(save_path, 'w') as junit_file:
        junit_file.write(lines)
    print('junit file written!')


def add_tests(subparsers):
    test_parser = subparsers.add_parser('test')
    test_parser.set_defaults(func=on_cmd_test)


def main():
    """main function"""
    data = build.default_data('euphoria')
    build.add_dependency_sdl2(data)
    build.add_dependency_python(data)
    build.add_dependency_assimp(data)
    build.main(data)


if __name__ == "__main__":
    main()
