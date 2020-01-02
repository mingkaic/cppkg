COVERAGE_INFO_FILE := bazel-out/_coverage/_coverage_report.dat

COVERAGE_IGNORE := 'external/*' '**/test/*' 'testutil/*' '**/genfiles/*' 'dbg/*'

CCOVER := bazel coverage --config asan --action_env="ASAN_OPTIONS=detect_leaks=0" --config gtest --config cc_coverage

DIFF_TEST := //diff:test

ESTD_TEST := //estd:test

EXAM_TEST := //exam:test

FLAG_TEST := //flag:test

FMTS_TEST := //fmts:test

LOGS_TEST := //logs:test

JOBS_TEST := //jobs:test


cov_clean: coverage.info
	lcov --remove coverage.info $(COVERAGE_IGNORE) -o coverage.info
	lcov --list coverage.info

cov_genhtml: coverage.info
	genhtml -o html coverage.info


coverage:
	$(CCOVER) $(DIFF_TEST) $(ESTD_TEST) $(EXAM_TEST) $(FMTS_TEST) $(LOGS_TEST) $(JOBS_TEST) $(FLAG_TEST)
	lcov --remove $(COVERAGE_INFO_FILE) -o coverage.info

cover_diff:
	$(CCOVER) $(DIFF_TEST)
	lcov --remove $(COVERAGE_INFO_FILE) -o coverage.info

cover_estd:
	$(CCOVER) $(ESTD_TEST)
	lcov --remove $(COVERAGE_INFO_FILE) 'logs/*' -o coverage.info

cover_exam:
	$(CCOVER) $(EXAM_TEST)
	lcov --remove $(COVERAGE_INFO_FILE) 'logs/*' 'fmts/*' -o coverage.info

cover_fmts:
	$(CCOVER) $(FMTS_TEST)
	lcov --remove $(COVERAGE_INFO_FILE) -o coverage.info

cover_logs:
	$(CCOVER) $(LOGS_TEST)
	lcov --remove $(COVERAGE_INFO_FILE) 'fmts/*' -o coverage.info

cover_jobs:
	$(CCOVER) $(JOBS_TEST)
	lcov --remove $(COVERAGE_INFO_FILE) 'logs/*' -o coverage.info

cover_flag:
	$(CCOVER) $(FLAG_TEST)
	lcov --remove $(COVERAGE_INFO_FILE) 'logs/*' -o coverage.info


lcov: coverage cov_clean

lcov_diff: cover_diff cov_clean

lcov_estd: cover_estd cov_clean

lcov_exam: cover_exam cov_clean

lcov_fmts: cover_fmts cov_clean

lcov_logs: cover_logs cov_clean

lcov_jobs: cover_jobs cov_clean

lcov_flag: cover_flag cov_clean


.PHONY: cov_clean cov_genhtml
.PHONY: coverage cover_diff cover_estd cover_exam cover_fmts cover_logs cover_jobs cover_flag
.PHONY: lcov lcov_diff lcov_estd lcov_exam lcov_fmts lcov_logs lcov_jobs lcov_flag
