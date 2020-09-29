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

######## COVERAGES ########

TMP_COVFILE := /tmp/coverage.info
COVERAGE_INFO_FILE := bazel-out/_coverage/_coverage_report.dat
CCOVER := bazel coverage --config asan --action_env="ASAN_OPTIONS=detect_leaks=0" --config gtest --config cc_coverage
COVERAGE_CTX := tmp/cppkg_coverage
COVERAGE_CSV := tmp/cppkg_conversions.csv

.PHONY: cov_clean
cov_clean:
	rm *.info
	rm -Rf html

# coverage helper
.PHONY: cov_init
cov_init:
	rm -Rf tmp
	mkdir -p $(COVERAGE_CTX)
	find . -maxdepth 1 | grep -E -v 'tmp|.git|bazel-' | tail -n +2 | xargs -i cp -r {} $(COVERAGE_CTX)
	find $(COVERAGE_CTX) | grep -E '.cpp|.hpp' | python3 scripts/label_uniquify.py $(COVERAGE_CTX) > $(COVERAGE_CSV)
	find $(COVERAGE_CTX) | grep -E '.yml' | python3 scripts/yaml_replace.py $(COVERAGE_CSV)

.PHONY: cov_copyout
cov_copyout:
	python3 scripts/label_replace.py $(COVERAGE_CTX)/$(COVFILE) $(COVERAGE_CSV) > $(COVFILE)

.PHONY: cov_genhtml
cov_genhtml: cov_copyout
	genhtml -o html $(COVFILE)

.PHONY: clean_test_coverage
clean_test_coverage: ${COVERAGE_INFO_FILE}
	lcov --remove ${COVERAGE_INFO_FILE} '**/test/*' '**/mock/*' '**/*.pb.*' -o ${TMP_COVFILE}

.PHONY: coverage
coverage:
	$(CCOVER) //...
	@make clean_test_coverage
	lcov --extract ${TMP_COVFILE} 'diff/*' 'egrpc/*' 'error/*' 'estd/*' 'exam/*' 'flag/*' 'fmts/*' 'jobs/*' 'logs/*' 'types/*' -o coverage.info

###### INDIVIDUAL COVERAGES ######

.PHONY: cover_diff
cover_diff:
	${CCOVER} --instrumentation_filter 'diff/*' //diff:test
	@make clean_test_coverage
	lcov -a ${TMP_COVFILE} -o diff_coverage.info

.PHONY: cover_egrpc
cover_egrpc:
	${CCOVER} --instrumentation_filter 'egrpc/*' //egrpc:test
	@make clean_test_coverage
	lcov -a ${TMP_COVFILE} -o egrpc_coverage.info

.PHONY: cover_error
cover_error:
	${CCOVER} --instrumentation_filter 'error/*' //error:test
	@make clean_test_coverage
	lcov -a ${TMP_COVFILE} -o error_coverage.info

.PHONY: cover_estd
cover_estd:
	${CCOVER} --instrumentation_filter 'estd/*' //estd:test
	@make clean_test_coverage
	lcov -a ${TMP_COVFILE} -o estd_coverage.info

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
