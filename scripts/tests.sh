#!/usr/bin/env bash

THIS_DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )";
COV_DIR="$THIS_DIR";

CONTEXT=$(cd "$1" && pwd);

WORKDIR="$CONTEXT/tmp/cppkg_coverage";
CONVERSION_CSV="$CONTEXT/tmp/cppkg_conversion.csv";

rm -Rf "$WORKDIR";
mkdir -p "$WORKDIR";
find $WORKDIR -maxdepth 1 | grep -E -v 'tmp|\.git|bazel-' | tail -n +2 | xargs -i cp -r {} $WORKDIR;
find $WORKDIR | grep -E '\.cpp|\.hpp' | python3 $THIS_DIR/label_uniquify.py $WORKDIR > $CONTEXT;
find $WORKDIR | grep -E '\.yml' | python3 $THIS_DIR/yaml_replace.py $CONTEXT;

cd "$WORKDIR";
lcov --base-directory . --directory . --zerocounters;
set -e

echo "===== ENVIRONMENT =====";
if [ -x "$(command -v free)" ]; then
	free -m;
fi

# ===== Run Gtest =====
echo "===== TESTS =====";

source "$THIS_DIR/coverage.sh";

bzl_coverage //...

python3 "$THIS_DIR/label_replace.py" "$COV_DIR/coverage.info" $CONVERSION_CSV > "$COV_DIR/labelled_coverage.info";
send2codecov "$COV_DIR/labelled_coverage.info";
cd "$CONTEXT";

echo "";
echo "============ CPPKG TEST SUCCESSFUL ============";
