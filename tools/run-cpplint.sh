#!/bin/sh
find libs/ apps/ -type f \( -iname \*.h -o -iname \*.cc \) | xargs cpplint
