#!/usr/bin/python
import os
import sys
import re
import typing
import zipfile
import subprocess

class TextReplacer:
    def __init__(self):
        self.res = []

    def add(self, reg, rep):
        self.res.append((reg, rep))
        return self

    def replace(self, text):
        for r in self.res:
            reg = r[0]
            rep = r[1]
            text = text.replace(reg, rep)
        return text


def get_vs_root():
    if os.name == 'nt':
        vs = subprocess.check_output(
            ['reg', 'QUERY', r"HKLM\SOFTWARE\Microsoft\VisualStudio\14.0", '/v', 'InstallDir', '/reg:32'])
        print("This is the vs solution path...", vs)
        sys.stdout.flush()
    vs_root = r'C:\Program Files (x86)\Microsoft Visual Studio 14.0\Common7\IDE'
    return vs_root


def globals():
    vs_root = r'C:\Program Files (x86)\Microsoft Visual Studio 14.0\Common7\IDE'
    sys.stdout.flush()

    root = os.getcwd()
    install_dist = os.path.join(root, 'install-dist')
    install = os.path.join(root, 'install')
    wx_root = os.path.join(install_dist, 'wx')
    proto_root = os.path.join(install_dist, 'proto')
    proto_root_root = os.path.join(proto_root, 'protobuf-2.6.1')
    build = os.path.join(root, 'build')


def appveyor_msbuild():
    return r' /logger:"C:\Program Files\AppVeyor\BuildAgent\Appveyor.MSBuildLogger.dll"'


def get_current_platform() -> str:
    platform = 'x64'
    if os.environ.get('PLATFORM', 'unknown') == 'x86':
        platform = 'Win32'
    return platform


def verify_dir_exist(path: str):
    if not os.path.isdir(path):
        os.makedirs(path)


def download_file(url: str, path: str):
    import urllib
    if not os.path.isfile(path):
        print("Downloading ", path)
        urllib.request.urlretrieve(url, path)
    else:
        print("Already downloaded", path)


def list_projects_in_solution(path: str) -> typing.List[str]:
    ret = []
    dir = os.path.dirname(path)
    pl = re.compile(r'Project\("[^"]+"\) = "[^"]+", "([^"]+)"')
    with open(path) as sln:
        for line in sln:
            # Project("{8BC9CEB8-8B4A-11D0-8D11-00A0C91BC942}") = "richtext", "wx_richtext.vcxproj", "{7FB0902D-8579-5DCE-B883-DAF66A885005}"
            m = pl.match(line)
            if m:
                ret.append(os.path.join(dir, m.group(1)))
    return ret


def add_definition_to_project(path: str, define: str):
    # <PreprocessorDefinitions>WIN32;_LIB;_CRT_SECURE_NO_DEPRECATE=1;_CRT_NON_CONFORMING_SWPRINTFS=1;_SCL_SECURE_NO_WARNINGS=1;__WXMSW__;NDEBUG;_UNICODE;WXBUILDING;%(PreprocessorDefinitions)</PreprocessorDefinitions>
    pp = re.compile(r'([ ]*<PreprocessorDefinitions>)([^<]*</PreprocessorDefinitions>)')
    lines = []
    with open(path) as project:
        for line in project:
            m = pp.match(line)
            if m:
                lines.append('{0}{1};{2}'.format(m.group(1), define, m.group(2)))
            else:
                lines.append(line.rstrip())
    with open(path, mode='w') as project:
        for line in lines:
            project.write(line + '\n')



# change from:
# <RuntimeLibrary>MultiThreadedDebugDLL</RuntimeLibrary> to <RuntimeLibrary>MultiThreadedDebug</RuntimeLibrary>
# <RuntimeLibrary>MultiThreadedDLL</RuntimeLibrary> to <RuntimeLibrary>MultiThreaded</RuntimeLibrary>
def change_to_static_link(path: str):
    mtdebug = re.compile(r'([ ]*)<RuntimeLibrary>MultiThreadedDebugDLL')
    mtrelease = re.compile(r'([ ]*)<RuntimeLibrary>MultiThreadedDLL')
    lines = []
    with open(path) as project:
        for line in project:
            mdebug = mtdebug.match(line)
            mrelease = mtrelease.match(line)
            if mdebug:
                print('in {project} changed to static debug'.format(project=path))
                lines.append(
                    '{spaces}<RuntimeLibrary>MultiThreadedDebug</RuntimeLibrary>'.format(spaces=mdebug.group(1)))
            elif mrelease:
                print('in {project} changed to static release'.format(project=path))
                lines.append('{spaces}<RuntimeLibrary>MultiThreaded</RuntimeLibrary>'.format(spaces=mrelease.group(1)))
            else:
                lines.append(line.rstrip())
    with open(path, mode='w') as project:
        for line in lines:
            project.write(line + '\n')


def change_all_projects_to_static(sln: str):
    projects = list_projects_in_solution(sln)
    for p in projects:
        change_to_static_link(p)


def add_definition_to_solution(sln: str, definition: str):
    projects = list_projects_in_solution(sln)
    for p in projects:
        add_definition_to_project(p, definition)


def make_single_project_64(project_path: str, rep: TextReplacer):
    if not os.path.isfile(project_path):
        print('missing ' + project_path)
        return
    lines = []
    with open(project_path) as project:
        for line in project:
            nl = rep.replace(line.rstrip())
            lines.append(nl)
    with open(project_path, 'w') as project:
        for line in lines:
            project.write(line + '\n')


def make_projects_64(sln: str):
    projects = list_projects_in_solution(sln)
    rep = TextReplacer()
    rep.add('Win32', 'x64')
    rep.add('<DebugInformationFormat>EditAndContinue</DebugInformationFormat>',
            '<DebugInformationFormat>ProgramDatabase</DebugInformationFormat>')
    rep.add('<TargetMachine>MachineX86</TargetMachine>', '<TargetMachine>MachineX64</TargetMachine>')
    # protobuf specific hack since cmake looks in x64 folder
    rep.add('<OutDir>Release\</OutDir>', '<OutDir>x64\Release\</OutDir>')
    rep.add('<OutDir>Debug\</OutDir>', '<OutDir>x64\Debug\</OutDir>')
    for project in projects:
        make_single_project_64(project, rep)


def make_solution_64(sln: str):
    rep = TextReplacer()
    rep.add('Win32', 'x64')

    lines = []
    with open(sln) as slnlines:
        for line in slnlines:
            nl = rep.replace(line.rstrip())
            lines.append(nl)

    with open(sln, 'w') as f:
        for line in lines:
            f.write(line + '\n')


def convert_sln_to_64(sln: str):
    make_solution_64(sln)
    make_projects_64(sln)


def install_dependency_wx(install_dist: str, wx_root: str, build: bool, platform: str):
    wx_url = "https://github.com/wxWidgets/wxWidgets/releases/download/v3.1.0/wxWidgets-3.1.0.zip"
    wx_zip = os.path.join(install_dist, "wx.zip")
    wx_sln = os.path.join(wx_root, 'build', 'msw', 'wx_vc14.sln')
    verify_dir_exist(install_dist)
    verify_dir_exist(wx_root)
    print("downloading wx...")
    download_file(wx_url, os.path.join(install_dist, wx_zip))
    print("extracting wx")
    with zipfile.ZipFile(wx_zip, 'r') as z:
        z.extractall(wx_root)
    print("changing wx to static")
    change_all_projects_to_static(wx_sln)

    print("building wxwidgets")
    print("----------------------------------")

    if build:
        sys.stdout.flush()
        wx_msbuild_cmd = ['msbuild', '/p:Configuration=Release', '/p:Platform='+platform, appveyor_msbuild(), wx_sln]
        subprocess.check_call(wx_msbuild_cmd)


def install_dependency_proto(install_dist: str, proto_root: str, build: bool, vs_root: str, platform: str):
    proto_url = "https://github.com/google/protobuf/releases/download/v2.6.1/protobuf-2.6.1.zip"
    proto_root_root = os.path.join(proto_root, 'protobuf-2.6.1')
    proto_zip = os.path.join(install_dist, 'proto.zip')
    proto_sln = os.path.join(proto_root_root, 'vsprojects', 'protobuf.sln')
    verify_dir_exist(install_dist)
    verify_dir_exist(proto_root)
    print("downloding proto...")
    download_file(proto_url, os.path.join(install_dist, proto_zip))
    print("extracting proto")
    with zipfile.ZipFile(proto_zip, 'r') as z:
        z.extractall(proto_root)
    print("upgrading protobuf")
    print("-----------------------------------")
    devenv = os.path.join(vs_root, 'devenv.exe')
    if build:
        sys.stdout.flush()
        subprocess.check_call([devenv, proto_sln, '/upgrade'])
    add_definition_to_solution(proto_sln, '_SILENCE_STDEXT_HASH_DEPRECATION_WARNINGS')

    print("changing proto to static")
    change_all_projects_to_static(proto_sln)

    if platform == 'x64':
        print('64 bit build, hacking proto to 64 bit')
        convert_sln_to_64(proto_sln)

    print("building protobuf")
    print("-----------------------------------")
    if build:
        sys.stdout.flush()
        proto_msbuild_cmd = ['msbuild', '/t:libprotobuf;protoc', '/p:Configuration=Release', '/p:Platform='+platform, appveyor_msbuild(), proto_sln]
        subprocess.check_call(proto_msbuild_cmd)

