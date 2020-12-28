load("@bazel_tools//tools/build_defs/repo:git.bzl", "git_repository")

def gtest_repository(name):
    git_repository(
        name = name,
        remote = "https://github.com/google/googletest",
        commit = "ca4b7c9ff4d8a5c37ac51795b03ffe934958aeff",
    )

#_URL = "https://github.com/google/googletest/archive/ec44c6c1675c25b9827aacd08c02433cccde7780.tar.gz"
#
#_STRIP_PREFIX = "googletest-ec44c6c1675c25b9827aacd08c02433cccde7780"
#
#_BUILD_CONTENT = """licenses(["notice"])
#
#cc_library(
#    name = "gtest",
#    srcs = [
#        "googletest/src/gtest-all.cc",
#        "googlemock/src/gmock-all.cc",
#    ],
#    hdrs = glob([
#        "**/*.h",
#        "googletest/src/*.cc",
#        "googlemock/src/*.cc",
#    ]),
#    includes = [
#        "googlemock",
#        "googletest",
#        "googletest/include",
#        "googlemock/include",
#    ],
#    linkopts = ["-pthread"],
#    visibility = ["//visibility:public"],
#)
#
#cc_library(
#    name = "gtest_main",
#    srcs = ["googlemock/src/gmock_main.cc"],
#    linkopts = ["-pthread"],
#    visibility = ["//visibility:public"],
#    deps = [":gtest"],
#)
#"""
#
#def _impl(ctx):
#    ctx.download_and_extract(
#        stripPrefix = _STRIP_PREFIX,
#        url = _URL,
#    )
#
#    ctx.file(
#        "BUILD.bazel",
#        content = _BUILD_CONTENT,
#        executable = False,
#    )
#
#    ctx.file(
#        "WORKSPACE",
#        content = "",
#        executable = False,
#    )
#
#gtest_repository = repository_rule(implementation = _impl)
