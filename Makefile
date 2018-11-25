COVERAGE_INFO_FILE := coverage.info

COVER := bazel coverage --config asan --config gtest

FLAG_TEST := //flag:test

FMTS_TEST := //fmts:test

LOGS_TEST := //logs:test

COVERAGE_IGNORE := 'external/*' '**/test/*' 'testutil/*' '**/genfiles/*' 'dbg/*'

COVERAGE_PIPE := ./scripts/merge_cov.sh $(COVERAGE_INFO_FILE)

TMP_LOGFILE := /tmp/cppkg-test.log


coverage: cover_fmts cover_logs cover_flag

cover_fmts:
	$(COVER) $(FMTS_TEST)

cover_logs:
	$(COVER) $(LOGS_TEST)

cover_flag:
	$(COVER) $(FLAG_TEST)


lcov: coverage
	rm -f $(TMP_LOGFILE)
	cat bazel-testlogs/fmts/test/test.log >> $(TMP_LOGFILE)
	cat bazel-testlogs/logs/test/test.log >> $(TMP_LOGFILE)
	cat bazel-testlogs/flag/test/test.log >> $(TMP_LOGFILE)
	cat $(TMP_LOGFILE) | $(COVERAGE_PIPE)
	lcov --remove $(COVERAGE_INFO_FILE) $(COVERAGE_IGNORE) -o $(COVERAGE_INFO_FILE)
	rm -f $(TMP_LOGFILE)
	lcov --list $(COVERAGE_INFO_FILE)

lcov_fmts: cover_fmts
	cat bazel-testlogs/fmts/test/test.log | $(COVERAGE_PIPE)
	lcov --remove $(COVERAGE_INFO_FILE) $(COVERAGE_IGNORE) -o $(COVERAGE_INFO_FILE)
	lcov --list $(COVERAGE_INFO_FILE)

lcov_logs: cover_logs
	cat bazel-testlogs/logs/test/test.log | $(COVERAGE_PIPE)
	lcov --remove $(COVERAGE_INFO_FILE) $(COVERAGE_IGNORE) 'fmts/*' -o $(COVERAGE_INFO_FILE)
	lcov --list $(COVERAGE_INFO_FILE)

lcov_flag: cover_flag
	rm -f $(TMP_LOGFILE)
	cat bazel-testlogs/flag/test/test.log | $(COVERAGE_PIPE)
	lcov --remove $(COVERAGE_INFO_FILE) $(COVERAGE_IGNORE) 'fmts/*' 'logs/*' -o $(COVERAGE_INFO_FILE)
	rm -f $(TMP_LOGFILE)
	lcov --list $(COVERAGE_INFO_FILE)
