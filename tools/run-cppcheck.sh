#!/bin/sh
cppcheck --enable=all --suppressions-list=tools/cppcheck-suppressions --inconclusive --error-exitcode=0 libs/ apps/ 2> cppcheck-result.txt

# remove colors: https://stackoverflow.com/questions/17998978/removing-colors-from-output
cat cppcheck-result.txt | sed 's/\x1B\[[0-9;]\{1,\}[A-Za-z]//g'  > cppcheck-result-fix.txt
rm cppcheck-result.txt
mv cppcheck-result-fix.txt cppcheck-result.txt
