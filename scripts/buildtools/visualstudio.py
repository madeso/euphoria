# !/usr/bin/python3
"""hacky utilitity functions for visual studio projects"""

import os
import re
import subprocess
import buildtools.core as core
import buildtools.args as args
import buildtools.cmake as cmake
import typing


VSWHERE_EXE = os.path.join(os.path.dirname(os.path.abspath(__file__)), 'vswhere.exe')

# Project("{8BC9CEB8-8B4A-11D0-8D11-00A0C91BC942}") = "richtext",
# "wx_richtext.vcxproj", "{7FB0902D-8579-5DCE-B883-DAF66A885005}"
SOLUTION_PROJECT_REGEX = re.compile(r'Project\("[^"]+"\) = "[^"]+", "([^"]+)"')

# <PreprocessorDefinitions>
# WIN32;_LIB;_CRT_SECURE_NO_DEPRECATE=1;_CRT_NON_CONFORMING_SWPRINTFS=1;
# _SCL_SECURE_NO_WARNINGS=1;__WXMSW__;NDEBUG;_UNICODE;WXBUILDING;
# %(PreprocessorDefinitions)
# </PreprocessorDefinitions>
PROJECT_PREPROC_REGEX = re.compile(r'([ ]*<PreprocessorDefinitions>)([^<]*' +
                                   r'</PreprocessorDefinitions>)')

MT_DEBUG_REGEX = re.compile(r'([ ]*)<RuntimeLibrary>MultiThreadedDebugDLL')
MT_RELEASE_REGEX = re.compile(r'([ ]*)<RuntimeLibrary>MultiThreadedDLL')

###############################################################################


def determine_version_for_vswhere(_: args.Compiler) -> str:
    """gets the argument to vswhere for the current compiler"""
    # determine path based on compiler
    return '16.0'


def get_path_to_devenv(compiler: args.Compiler):
    """gets the path to devenv for the current compiler"""
    # warn if default value?
    devenv = r'C:\Program Files (x86)\Microsoft Visual Studio 14.0\Common7\IDE\devenv.exe'
    if core.is_windows():
        vswhere = VSWHERE_EXE
        cmd = [vswhere, '-version', determine_version_for_vswhere(compiler),
               '-property', 'productPath']
        devenv = subprocess.check_output(cmd).decode("utf-8").strip()
        print('devenv is ', devenv, flush=True)

    return devenv


def visual_studio_generator(compiler: args.Compiler, platform: args.Platform) -> cmake.Generator:
    """gets the visual studio cmake generator argument for the compiler and platform"""
    if compiler == args.Compiler.VS2015:
        if args.is_64bit(platform):
            return cmake.Generator('Visual Studio 14 2015 Win64')
        return cmake.Generator('Visual Studio 14 2015')
    if compiler == args.Compiler.VS2017:
        if args.is_64bit(platform):
            return cmake.Generator('Visual Studio 15 Win64')
        return cmake.Generator('Visual Studio 15')
    if compiler == args.Compiler.VS2019:
        arch = 'x64' if args.is_64bit(platform) else 'Win32'
        return cmake.Generator('Visual Studio 16 2019', arch)
    raise Exception('Invalid compiler')


def list_projects_in_solution(path: str) -> typing.List[str]:
    """list all projects in a solution file"""
    project_list = []
    solution_folder = os.path.dirname(path)
    with open(path) as solution_file:
        for line in solution_file:
            match = SOLUTION_PROJECT_REGEX.match(line)
            if match:
                subfolder = match.group(1)
                if not core.is_windows():
                    subfolder = subfolder.replace('\\', '/')
                print(subfolder, flush=True)
                project_list.append(os.path.join(solution_folder, subfolder))
    return project_list


def add_definition_to_project(path: str, new_define: str):
    """add preprocessor definition to visual studio project file"""
    lines = []
    with open(path) as project:
        for line in project:
            match = PROJECT_PREPROC_REGEX.match(line)
            if match:
                start_tag = match.group(1)
                existing_definitions_and_end_tag = match.group(2)
                lines.append('{0}{1};{2}'.format(start_tag, new_define,
                                                 existing_definitions_and_end_tag))
            else:
                lines.append(line.rstrip())
    with open(path, mode='w') as project:
        for line in lines:
            project.write(line + '\n')


def change_project_line_to_static_link(line, path):
    """transform a single line in a project file to static link"""
    match_debug = MT_DEBUG_REGEX.match(line)
    match_release = MT_RELEASE_REGEX.match(line)

    if match_debug:
        print('in {project} changed to static debug'.format(project=path), flush=True)
        indent = match_debug.group(1)
        return '{0}<RuntimeLibrary>MultiThreadedDebug</RuntimeLibrary>'.format(indent)

    if match_release:
        print('in {project} changed to static release'.format(project=path), flush=True)
        indent = match_release.group(1)
        return '{0}<RuntimeLibrary>MultiThreaded</RuntimeLibrary>'.format(indent)

    return line.rstrip()


def change_project_to_static_link(path: str):
    """change a single project to static crt"""
    lines = []
    with open(path) as project:
        for line in project:
            new_line = change_project_line_to_static_link(line, path)
            lines.append(new_line)
    with open(path, mode='w') as project:
        for line in lines:
            project.write(line + '\n')


def change_all_projects_to_static(sln: str):
    """change all projects in a sln to usse static crt"""
    projects = list_projects_in_solution(sln)
    for project in projects:
        change_project_to_static_link(project)


def add_definition_to_solution(sln: str, definition: str):
    """add a preproc to a solution"""
    projects = list_projects_in_solution(sln)
    for project in projects:
        add_definition_to_project(project, definition)


def make_single_project_64(project_path: str, rep: core.TextReplacer):
    """make a single project 64 bit"""
    if not os.path.isfile(project_path):
        print('missing ' + project_path, flush=True)
        return
    lines = []
    with open(project_path) as project:
        for line in project:
            new_line = rep.replace(line.rstrip())
            lines.append(new_line)
    with open(project_path, 'w') as project:
        for line in lines:
            project.write(line + '\n')


def make_projects_64(sln: str):
    """make all projects in a solution 64 bit"""
    projects = list_projects_in_solution(sln)
    rep = core.TextReplacer()
    rep.add('Win32', 'x64')
    rep.add('<DebugInformationFormat>EditAndContinue</DebugInformationFormat>',
            '<DebugInformationFormat>ProgramDatabase</DebugInformationFormat>')
    rep.add('<TargetMachine>MachineX86</TargetMachine>',
            '<TargetMachine>MachineX64</TargetMachine>')
    # protobuf specific hack since cmake looks in x64 folder
    rep.add('<OutDir>Release\\</OutDir>', '<OutDir>x64\\Release\\</OutDir>')
    rep.add('<OutDir>Debug\\</OutDir>', '<OutDir>x64\\Debug\\</OutDir>')
    for project in projects:
        make_single_project_64(project, rep)


def make_solution_64(sln: str):
    """make only solution file 64 bit"""
    rep = core.TextReplacer()
    rep.add('Win32', 'x64')

    lines = []
    with open(sln) as slnlines:
        for line in slnlines:
            new_line = rep.replace(line.rstrip())
            lines.append(new_line)

    with open(sln, 'w') as solution_file:
        for line in lines:
            solution_file.write(line + '\n')


def convert_sln_to_64(sln: str):
    """make both solution and all its projects 64 bit"""
    make_solution_64(sln)
    make_projects_64(sln)


def upgrade_sln(proto_sln: str, compiler: args.Compiler):
    """upgrade a old solution to the current compiler"""
    devenv = get_path_to_devenv(compiler)
    print('Upgrading ', proto_sln, flush=True)
    if core.is_windows():
        subprocess.check_call([devenv, proto_sln, '/upgrade'])
    print('Upgrade done!', flush=True)


def find_msbuild(compiler: args.Compiler):
    """find path to msbuild"""
    # warn if default value?
    msbuild_path = 'msbuild'
    if core.is_windows():
        vswhere = VSWHERE_EXE
        cmd = [vswhere, '-version', determine_version_for_vswhere(compiler),
               '-requires', 'Microsoft.Component.MSBuild',
               '-find', 'MSBuild\\**\\Bin\\MSBuild.exe']
        msbuild_path = subprocess.check_output(cmd).decode("utf-8").strip()
        print('msbuild from vswhere is ', msbuild_path, flush=True)

    return msbuild_path


def msbuild(sln: str, compiler: args.Compiler, platform: args.Platform,
            libraries: typing.Optional[typing.List[str]]):
    """run the msbuild command"""
    msbuild_cmd = [find_msbuild(compiler)]
    print('Msbuild is ', msbuild_cmd[0], flush=True)
    if libraries is not None:
        msbuild_cmd.append('/t:' + ';'.join(libraries))
    msbuild_cmd.append('/p:Configuration=Release')
    # https://blogs.msdn.microsoft.com/vcblog/2016/02/24/stuck-on-an-older-toolset-version-move-to-visual-studio-2015-without-upgrading-your-toolset/
    # https://stackoverflow.com/questions/33380128/visual-studio-2015-command-line-retarget-solution
    msbuild_cmd.append('/p:PlatformToolset=' + args.get_msbuild_toolset(compiler))
    msbuild_cmd.append('/p:Platform=' + args.platform_as_string(platform))
    msbuild_cmd.append(sln)
    if core.is_windows():
        subprocess.check_call(msbuild_cmd)
    else:
        print(msbuild_cmd, flush=True)
