load("@bazel_tools//tools/build_defs/repo:git.bzl", "git_repository")

def cisab_repository():
    git_repository(
        name = "com_github_mingkaic_cisab",
        remote = "https://github.com/mingkaic/cisab",
        commit = "2eb0d648e066e57efd5b1032d8274fd3a5189e84",
    )
