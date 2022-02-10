load("@bazel_tools//tools/build_defs/repo:git.bzl", "git_repository")

def verum_repository():
    git_repository(
        name = "com_github_mingkaic_verum",
        remote = "https://github.com/mingkaic/verum",
        commit = "67f7ca4f50d588590daf35814038ca3ea7fe27f0",
    )
