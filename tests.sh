#!/usr/bin/env bash

THIS_DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )";
COV_DIR="$THIS_DIR";

lcov --base-directory . --directory . --zerocounters;
set -e

# ===== Run Gtest =====
echo "===== TESTS =====";

source "$THIS_DIR/coverage.sh";

bzl_coverage //...

lcov --remove "$COV_DIR/coverage.info" 'external/*' '**/test/*' \
'testutil/*' '**/genfiles/*' 'dbg/*' -o "$COV_DIR/coverage.info";
send2codecov "$COV_DIR/coverage.info";

echo "";
echo "============ CPPKG TEST SUCCESSFUL ============";
