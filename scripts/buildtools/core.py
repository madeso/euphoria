#!/usr/bin/python3
import os
import sys
import zipfile
import shutil


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


def flush():
    sys.stdout.flush()


def is_windows() -> bool:
    import platform
    return platform.system() == 'Windows'


def dir_exist(path: str) -> bool:
    return os.path.isdir(path)


def globals():
    vs_root = r'C:\Program Files (x86)\Microsoft Visual Studio 14.0\Common7\IDE'
    flush()

    root = os.getcwd()
    install_dist = os.path.join(root, 'install-dist')
    install = os.path.join(root, 'install')
    wx_root = os.path.join(install_dist, 'wx')
    proto_root = os.path.join(install_dist, 'proto')
    proto_root_root = os.path.join(proto_root, 'protobuf-2.6.1')
    build = os.path.join(root, 'build')


def appveyor_msbuild():
    return r'/logger:%programfiles%\AppVeyor\BuildAgent\Appveyor.MSBuildLogger.dll'


def is_platform_64bit() -> bool:
    if os.environ.get('PLATFORM', 'unknown') == 'x86':
        return False
    else:
        return True


def verify_dir_exist(path: str):
    if os.path.isdir(path):
        print('Directory exist', path)
    if os.path.isfile(path):
        print('ERROR: Not a directory, file!!!', path)
    if not os.path.exists(path):
        print('Not a directory, creating', path)
        os.makedirs(path)


def download_file(url: str, path: str):
    import urllib.request
    if not os.path.isfile(path):
        print("Downloading ", path)
        with urllib.request.urlopen(url) as response, open(path, 'wb') as out_file:
            shutil.copyfileobj(response, out_file)
    else:
        print("Already downloaded", path)


def extract_zip(proto_zip, proto_root):
    with zipfile.ZipFile(proto_zip, 'r') as z:
        z.extractall(proto_root)


def movefiles(from_directory, to_directory):
    for fidir in os.listdir(from_directory):
        to_entry = os.path.join(to_directory, fidir)
        from_entry = os.path.join(from_directory, fidir)
        shutil.move(from_entry, to_entry)


def print_files_and_folders(root, start: str = ''):
    for f in os.listdir(root):
        path = os.path.join(root, f)
        if os.path.isfile(path):
            print(start + f)
        else:
            print(start + f + '/')
            print_files_and_folders(path, start + '  ')


def rename_file(from_path: str, to_path: str):
    if os.path.isfile(from_path):
        os.rename(from_path, to_path)
    else:
        print('Missing ', from_path)


def print_dashes():
    print('-----------------------------------------------------------------------------------------------------------')


def print_file(path: str):
    if os.path.isfile(path):
        print()
        with open(path, 'r') as fin:
            print(fin.read())
    else:
        print('Could not open file', path)
