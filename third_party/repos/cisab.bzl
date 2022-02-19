load("@bazel_tools//tools/build_defs/repo:git.bzl", "git_repository")

def cisab_repository():
    git_repository(
        name = "com_github_mingkaic_cisab",
        remote = "https://github.com/mingkaic/cisab",
        commit = "b65c5e18e6bc1ca1bdcbb064ab681a15b16b961f",
    )
