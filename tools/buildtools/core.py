#!/usr/bin/python3
"""core functions and classes"""
import os
import sys
import zipfile
import shutil
import platform
import urllib.request


class TextReplacer:
    """multi replace calls on a single text"""
    def __init__(self):
        self.replacements = []

    def add(self, old: str, new: str):
        """add a replacement command"""
        self.replacements.append((old, new))
        return self

    def replace(self, text: str):
        """perform all replacement"""
        for replacement in self.replacements:
            old = replacement[0]
            new = replacement[1]
            text = text.replace(old, new)
        return text


def flush():
    """flushes stdout"""
    sys.stdout.flush()


def is_windows() -> bool:
    """is the script runnning on a windows system?"""
    return platform.system() == 'Windows'


def dir_exist(path: str) -> bool:
    """does the directory exist"""
    return os.path.isdir(path)


def is_platform_64bit() -> bool:
    """check if the script is running on 64bit or not"""
    if os.environ.get('PLATFORM', 'unknown') == 'x86':
        return False
    return True


def verify_dir_exist(path: str):
    """make sure directory exists"""
    if os.path.isdir(path):
        print('Directory exist', path, flush=True)
    if os.path.isfile(path):
        print('ERROR: Not a directory, file!!!', path, flush=True)
    if not os.path.exists(path):
        print('Not a directory, creating', path, flush=True)
        os.makedirs(path)


def download_file(url: str, path: str):
    """download file if not already downloaded"""
    if not os.path.isfile(path):
        print("Downloading ", path, flush=True)
        with urllib.request.urlopen(url) as response, open(path, 'wb') as out_file:
            shutil.copyfileobj(response, out_file)
    else:
        print("Already downloaded", path, flush=True)


def extract_zip(zip_file, target_folder):
    """extract a zip file to folder"""
    with zipfile.ZipFile(zip_file, 'r') as handle:
        handle.extractall(target_folder)


def move_files(from_directory, to_directory):
    """moves all file from one directory to another"""
    for fidir in os.listdir(from_directory):
        to_entry = os.path.join(to_directory, fidir)
        from_entry = os.path.join(from_directory, fidir)
        shutil.move(from_entry, to_entry)


def print_files_and_folders(root, start: str = ''):
    """print files and folder recursivly"""
    for file in os.listdir(root):
        path = os.path.join(root, file)
        if os.path.isfile(path):
            print(start + file, flush=True)
        else:
            print(start + file + '/', flush=True)
            print_files_and_folders(path, start + '  ')


def rename_file(from_path: str, to_path: str):
    """renames a file"""
    if os.path.isfile(from_path):
        os.rename(from_path, to_path)
    else:
        print('Missing ', from_path, flush=True)


def print_dashes():
    """print dashes"""
    print('-' * 100, flush=True)


def print_file(path: str):
    """print file"""
    if os.path.isfile(path):
        print()
        with open(path, 'r') as fin:
            print(fin.read(), flush=True)
    else:
        print('Could not open file', path, flush=True)
