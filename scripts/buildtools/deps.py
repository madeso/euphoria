# !/usr/bin/python3

import sys
import os
import subprocess
import shutil

import buildtools.cmake as cmake
import buildtools.core as core
import buildtools.args as args
import buildtools.visualstudio as visualstudio


def install_dependency_wx(install_dist: str, wx_root: str, compiler: args.Compiler, platform: args.Platform):
    core.print_dashes()
    print('Installing dependency wxWidgets')
    wx_url = "https://github.com/wxWidgets/wxWidgets/releases/download/v3.1.0/wxWidgets-3.1.0.zip"
    wx_zip = os.path.join(install_dist, "wx.zip")
    wx_sln = os.path.join(wx_root, 'build', 'msw', 'wx_vc14.sln')
    if not core.dir_exist(wx_root):
        core.verify_dir_exist(install_dist)
        core.verify_dir_exist(wx_root)
        # hrm, this looks weird but seems to be working
        # wx_zip is based on install_dist, and is later joined by install_dist again...
        core.download_file(wx_url, os.path.join(install_dist, wx_zip))
        core.extract_zip(wx_zip, wx_root)

        print('Upgrading wx sln')
        core.print_dashes()
        visualstudio.upgrade_sln(wx_sln, compiler)

        #  print('Changing wx to static')
        #  core.print_dashes()
        #  visualstudio.change_all_projects_to_static(wx_sln)

        print("Building wxwidgets")
        core.print_dashes()
        visualstudio.msbuild(wx_sln, compiler, platform, None)
    else:
        print('wxWidgets build exist, not building again...')


def install_dependency_proto(install_dist: str, proto_root: str, compiler: args.Compiler, platform: args.Platform):
    core.print_dashes()
    print('Installing dependency protobuf')
    proto_url = "https://github.com/google/protobuf/releases/download/v2.6.1/protobuf-2.6.1.zip"
    proto_zip = os.path.join(install_dist, 'proto.zip')
    proto_sln = os.path.join(proto_root, 'vsprojects', 'protobuf.sln')
    if not core.dir_exist(proto_root):
        core.verify_dir_exist(install_dist)
        core.verify_dir_exist(proto_root)
        print("downloding proto...")
        core.download_file(proto_url, os.path.join(install_dist, proto_zip))
        print("extracting proto")
        core.extract_zip(proto_zip, proto_root)
        print('moving proto files from subfolder to root')
        proto_root_root = os.path.join(proto_root, 'protobuf-2.6.1')
        core.movefiles(proto_root_root, proto_root)
        print("upgrading protobuf")
        core.print_dashes()

        visualstudio.upgrade_sln(proto_sln, compiler)
        visualstudio.add_definition_to_solution(proto_sln, '_SILENCE_STDEXT_HASH_DEPRECATION_WARNINGS')

        #  print("changing proto to static")
        #  visualstudio.change_all_projects_to_static(proto_sln)

        if args.is_64bit(platform):
            print('64 bit build, hacking proto to 64 bit')
            visualstudio.convert_sln_to_64(proto_sln)

        print("building protobuf")
        core.print_dashes()
        visualstudio.msbuild(proto_sln, compiler, platform, ['libprotobuf', 'protoc'])
        proto_msbuild_cmd = ['msbuild', '/t:libprotobuf;protoc', '/p:Configuration=Release', '/p:Platform='+ args.platform_as_string(platform), proto_sln]
        if core.is_windows():
            core.flush()
            subprocess.check_call(proto_msbuild_cmd)
    else:
        print('Protobuf build exist, not building again...')


def install_dependency_sdl2(deps, root, build, generator: str):
    core.print_dashes()
    print('Installing dependency sdl2')
    url = "https://www.libsdl.org/release/SDL2-2.0.5.zip"
    zip = os.path.join(deps, 'sdl2.zip')
    if not core.dir_exist(root):
        core.verify_dir_exist(root)
        core.verify_dir_exist(deps)
        print('downloading sdl2')
        core.download_file(url, zip)
        core.extract_zip(zip, root)
        core.movefiles(os.path.join(root, 'SDL2-2.0.5'), root)
        project = cmake.CMake(build_folder=build, source_folder=root, generator=generator)
        #  project.make_static_library()
        project.config()
        project.build()
    else:
        print('SDL2 build exist, not building again...')


def setup_freetype_dependencies(root: str, platform: args.Platform):
    obj_folder = os.path.join(root, 'objs')

    # is x64 the right sub folder?
    build_folder = os.path.join(obj_folder, 'vc2010', args.platform_as_string(platform))
    os.environ["FREETYPE_DIR"] = root
    os.environ["GTKMM_BASEPATH"] = build_folder


def install_dependency_freetype(deps: str, root: str, compiler: args.Compiler, platform: args.Platform):
    core.print_dashes()
    print('Installing dependency freetype2')
    url = 'http://download.savannah.gnu.org/releases/freetype/ft28.zip'
    zip = os.path.join(deps, 'ft.zip')
    if not core.dir_exist(root):
        core.verify_dir_exist(root)
        core.verify_dir_exist(deps)
        print('downloading freetype2')
        core.download_file(url, zip)
        core.extract_zip(zip, root)
        core.movefiles(os.path.join(root, 'freetype-2.8'), root)
        sln = os.path.join(root, 'builds', 'windows', 'vc2010', 'freetype.sln')
        visualstudio.upgrade_sln(sln, compiler)
        #  visualstudio.change_all_projects_to_static(sln)
        visualstudio.msbuild(sln, compiler, platform, ['freetype'])

        build_folder = os.path.join(root, 'objs', 'vc2010', args.platform_as_string(platform))
        core.rename_file(os.path.join(build_folder, 'freetype28.lib'), os.path.join(build_folder, 'freetype.lib'))
    else:
        print('Freetype build exist, not building again...')


def install_dependency_assimp(deps: str, root: str, install: str, generator: str):
    core.print_dashes()
    print('Installing dependency assimp')
    url = "https://github.com/assimp/assimp/archive/v4.0.1.zip"
    zip = os.path.join(deps, 'assimp.zip')
    if not core.dir_exist(root):
        core.verify_dir_exist(root)
        core.verify_dir_exist(deps)
        print('downloading assimp')
        core.download_file(url, zip)
        core.extract_zip(zip, root)
        build = os.path.join(root, 'cmake-build')
        core.movefiles(os.path.join(root, 'assimp-4.0.1'), root)
        project = cmake.CMake(build_folder=build, source_folder=root, generator=generator)
        project.add_argument('ASSIMP_BUILD_X3D_IMPORTER', '0')
        #  project.make_static_library()
        print('Installing cmake to', install)
        core.flush()
        project.set_install_folder(install)
        core.verify_dir_exist(install)
        project.config()
        project.build()
        print('Installing assimp')
        project.install()
    else:
        print('Assimp build exist, not building again...')

