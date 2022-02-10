load("@bazel_tools//tools/build_defs/repo:git.bzl", "git_repository")

def cisab_repository():
    git_repository(
        name = "com_github_mingkaic_cisab",
        remote = "https://github.com/mingkaic/cisab",
        commit = "658411513875dd50161abbc3591a7ae41157c2b4",
    )
