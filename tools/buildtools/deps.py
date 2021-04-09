# !/usr/bin/python3
"""contains scripts to install dependencies"""

import os

import buildtools.cmake as cmake
import buildtools.core as core
import buildtools.args as args
import buildtools.visualstudio as visualstudio


def install_dependency_sdl2(deps, root, build, generator: cmake.Generator):
    """download and build sdl2"""
    core.print_dashes()
    print('Installing dependency sdl2', flush=True)
    url = "https://www.libsdl.org/release/SDL2-2.0.8.zip"
    zip_file = os.path.join(deps, 'sdl2.zip')
    if not core.dir_exist(root):
        core.verify_dir_exist(root)
        core.verify_dir_exist(deps)
        print('downloading sdl2', flush=True)
        core.download_file(url, zip_file)
        core.extract_zip(zip_file, root)
        core.move_files(os.path.join(root, 'SDL2-2.0.8'), root)
        project = cmake.CMake(build_folder=build, source_folder=root, generator=generator)
        # project.make_static_library()
        # this is defined by the standard library so don't add it
        # generates '__ftol2_sse already defined' errors
        project.add_argument('LIBC', 'ON')
        project.add_argument('SDL_STATIC', 'ON')
        project.add_argument('SDL_SHARED', 'OFF')
        project.config()
        project.build()
    else:
        print('SDL2 build exist, not building again...', flush=True)


def setup_freetype_dependencies(root: str, platform: args.Platform):
    """set enviroment variables for the cmake find scripts for freetype"""
    obj_folder = os.path.join(root, 'objs')

    # is x64 the right sub folder?
    build_folder = os.path.join(obj_folder, 'vc2010', args.platform_as_string(platform))
    os.environ["FREETYPE_DIR"] = root
    os.environ["GTKMM_BASEPATH"] = build_folder


def install_dependency_freetype(deps: str, root: str, compiler: args.Compiler,
                                platform: args.Platform):
    """download and build freetype"""
    core.print_dashes()
    print('Installing dependency freetype2', flush=True)
    url = 'http://download.savannah.gnu.org/releases/freetype/ft28.zip'
    zip_file = os.path.join(deps, 'ft.zip')
    if not core.dir_exist(root):
        core.verify_dir_exist(root)
        core.verify_dir_exist(deps)
        print('downloading freetype2', flush=True)
        core.download_file(url, zip_file)
        core.extract_zip(zip_file, root)
        core.move_files(os.path.join(root, 'freetype-2.8'), root)
        sln = os.path.join(root, 'builds', 'windows', 'vc2010', 'freetype.sln')
        visualstudio.upgrade_sln(sln, compiler)
        #  visualstudio.change_all_projects_to_static(sln)
        visualstudio.msbuild(sln, compiler, platform, ['freetype'])

        vc2010 = os.path.join(root, 'objs', 'vc2010')
        build_folder = os.path.join(vc2010, args.platform_as_string(platform))

        old_freetype = os.path.join(build_folder, 'freetype28.lib')
        new_freetype = os.path.join(build_folder, 'freetype.lib')
        core.rename_file(old_freetype, new_freetype)
    else:
        print('Freetype build exist, not building again...', flush=True)


def install_dependency_assimp(deps: str, root: str, install: str, generator: cmake.Generator):
    """download and build assimp"""
    core.print_dashes()
    print('Installing dependency assimp', flush=True)
    url = "https://github.com/assimp/assimp/archive/v5.0.1.zip"
    zip_file = os.path.join(deps, 'assimp.zip')
    if not core.dir_exist(root):
        core.verify_dir_exist(root)
        core.verify_dir_exist(deps)
        print('downloading assimp', flush=True)
        core.download_file(url, zip_file)
        print('extracting assimp', flush=True)
        core.extract_zip(zip_file, root)
        build = os.path.join(root, 'cmake-build')
        core.move_files(os.path.join(root, 'assimp-5.0.1'), root)
        project = cmake.CMake(build_folder=build, source_folder=root, generator=generator)
        project.add_argument('ASSIMP_BUILD_X3D_IMPORTER', '0')
        #  project.make_static_library()
        print('Installing cmake to', install, flush=True)
        core.flush()
        project.set_install_folder(install)
        core.verify_dir_exist(install)
        project.config()
        project.build()
        print('Installing assimp', flush=True)
        project.install()
    else:
        print('Assimp build exist, not building again...', flush=True)
