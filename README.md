# C++ pkg
[![cppkg](https://circleci.com/gh/mingkaic/cppkg.svg?style=svg)](https://app.circleci.com/pipelines/github/mingkaic/cppkg?filter=all)
[![codecov](https://codecov.io/gh/mingkaic/cppkg/branch/master/graph/badge.svg)](https://codecov.io/gh/mingkaic/cppkg)

Mimic Golang pkg libraries to simplify application development

# Dependencies

## ARRS, DIFF, ERROR, ESTD, JOBS, NUMBERS, UNITS

These libraries don't depend on anything

## FLAG

This library depends on boost

## EGRPC

This library depends on grpc and protobuf

# Conan installation
Before install package first add remote: `conan remote add mingkaic-co "https://gitlab.com/api/v4/projects/23299689/packages/conan"`
Add requirement `cppkg/<version>@mingkaic-co/stable`
