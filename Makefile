COVERAGE_INFO_FILE := coverage.info

COVER := bazel coverage --config asan --config gtest

DIFF_TEST := //diff:test

ESTD_TEST := //estd:test

EXAM_TEST := //exam:test

FLAG_TEST := //flag:test

FMTS_TEST := //fmts:test

LOGS_TEST := //logs:test

JOBS_TEST := //jobs:test

COVERAGE_IGNORE := 'external/*' '**/test/*' 'testutil/*' '**/genfiles/*' 'dbg/*'

COVERAGE_PIPE := ./scripts/merge_cov.sh $(COVERAGE_INFO_FILE)

TMP_LOGFILE := /tmp/cppkg-test.log


coverage: cover_diff cover_estd cover_exam cover_fmts cover_logs cover_jobs cover_flag

cover_diff:
	$(COVER) $(DIFF_TEST)

cover_estd:
	$(COVER) $(ESTD_TEST)

cover_exam:
	$(COVER) $(EXAM_TEST)

cover_fmts:
	$(COVER) $(FMTS_TEST)

cover_logs:
	$(COVER) $(LOGS_TEST)

cover_jobs:
	$(COVER) $(JOBS_TEST)

cover_flag:
	$(COVER) $(FLAG_TEST)


lcov: coverage
	rm -f $(TMP_LOGFILE)
	cat bazel-testlogs/diff/test/test.log >> $(TMP_LOGFILE)
	cat bazel-testlogs/estd/test/test.log >> $(TMP_LOGFILE)
	cat bazel-testlogs/exam/test/test.log >> $(TMP_LOGFILE)
	cat bazel-testlogs/fmts/test/test.log >> $(TMP_LOGFILE)
	cat bazel-testlogs/logs/test/test.log >> $(TMP_LOGFILE)
	cat bazel-testlogs/jobs/test/test.log >> $(TMP_LOGFILE)
	cat bazel-testlogs/flag/test/test.log >> $(TMP_LOGFILE)
	cat $(TMP_LOGFILE) | $(COVERAGE_PIPE)
	lcov --remove $(COVERAGE_INFO_FILE) $(COVERAGE_IGNORE) -o $(COVERAGE_INFO_FILE)
	rm -f $(TMP_LOGFILE)
	lcov --list $(COVERAGE_INFO_FILE)

lcov_diff: cover_diff
	cat bazel-testlogs/diff/test/test.log | $(COVERAGE_PIPE)
	lcov --remove $(COVERAGE_INFO_FILE) $(COVERAGE_IGNORE) -o $(COVERAGE_INFO_FILE)
	lcov --list $(COVERAGE_INFO_FILE)

lcov_estd: cover_estd
	cat bazel-testlogs/estd/test/test.log | $(COVERAGE_PIPE)
	lcov --remove $(COVERAGE_INFO_FILE) $(COVERAGE_IGNORE) -o $(COVERAGE_INFO_FILE)
	lcov --list $(COVERAGE_INFO_FILE)

lcov_exam: cover_exam
	cat bazel-testlogs/exam/test/test.log | $(COVERAGE_PIPE)
	lcov --remove $(COVERAGE_INFO_FILE) $(COVERAGE_IGNORE) -o $(COVERAGE_INFO_FILE)
	lcov --list $(COVERAGE_INFO_FILE)

lcov_fmts: cover_fmts
	cat bazel-testlogs/fmts/test/test.log | $(COVERAGE_PIPE)
	lcov --remove $(COVERAGE_INFO_FILE) $(COVERAGE_IGNORE) -o $(COVERAGE_INFO_FILE)
	lcov --list $(COVERAGE_INFO_FILE)

lcov_logs: cover_logs
	cat bazel-testlogs/logs/test/test.log | $(COVERAGE_PIPE)
	lcov --remove $(COVERAGE_INFO_FILE) $(COVERAGE_IGNORE) 'fmts/*' -o $(COVERAGE_INFO_FILE)
	lcov --list $(COVERAGE_INFO_FILE)

lcov_jobs: cover_jobs
	cat bazel-testlogs/jobs/test/test.log | $(COVERAGE_PIPE)
	lcov --remove $(COVERAGE_INFO_FILE) $(COVERAGE_IGNORE) -o $(COVERAGE_INFO_FILE)
	lcov --list $(COVERAGE_INFO_FILE)

lcov_flag: cover_flag
	rm -f $(TMP_LOGFILE)
	cat bazel-testlogs/flag/test/test.log | $(COVERAGE_PIPE)
	lcov --remove $(COVERAGE_INFO_FILE) $(COVERAGE_IGNORE) 'fmts/*' 'logs/*' -o $(COVERAGE_INFO_FILE)
	rm -f $(TMP_LOGFILE)
	lcov --list $(COVERAGE_INFO_FILE)
