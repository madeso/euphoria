#!/usr/bin/env python3

import argparse
import os

parser = argparse.ArgumentParser(description='Return file content from C++ functions')

parser.add_argument('headerfile', type=argparse.FileType('w'),
                    help='the header file')
parser.add_argument('source_file', type=argparse.FileType('w'),
                    help='the source file')
parser.add_argument('prefix',
                    help='the prefix to add to all the file functions')

parser.add_argument('input', metavar='N', nargs='+',
                    help='input files')

args = parser.parse_args()

source = args.source_file
header = args.headerfile
prefix = args.prefix
files = args.input

root = os.getcwd()

# todo: improve this
define = "GENERTED_HEADER"

header.write('#ifndef ' + define + "\n")
header.write('#define ' + define + "\n")
header.write("\n")

source.write('#include "shaders.h"\n')
source.write("\n")

def escape_cpp(line):
  line.replace('\t', '\\t').replace('\"', '\\"').replace('\t', '\\t')
  return line

for relative in args.input:
  f = os.path.join(root, relative)
  function_name = prefix + os.path.basename(f).replace('.', '_')

  print("Exported " + function_name)

  header.write("extern const char* const " + function_name + ";\n")
  source.write("const char* const " + function_name + " = \n")
  source.write('  ""\n')
  with open(f, 'r') as fi:
    for line in fi:
      source.write('  "' + escape_cpp(line.rstrip()) + '\\n"\n')
  source.write("  ;\n")
  source.write("\n")


header.write('#endif  // ' + define + "\n")
header.write("\n")

source.write("\n")