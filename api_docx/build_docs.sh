#!/bin/bash

set -e

DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" >/dev/null 2>&1 && pwd )"
echo "DIR = $DIR"

ROOT="$(dirname "$DIR")"
echo "ROOT = $ROOT"



echo "### doxygen version"
doxygen -v

doxygen $ROOT/api_docx/doxygen.conf
