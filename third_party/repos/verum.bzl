load("@bazel_tools//tools/build_defs/repo:git.bzl", "git_repository")

def verum_repository():
    git_repository(
        name = "com_github_mingkaic_verum",
        remote = "https://github.com/mingkaic/verum",
        commit = "5fbeb137325f98971153db09559d5bd181e5a970",
    )
