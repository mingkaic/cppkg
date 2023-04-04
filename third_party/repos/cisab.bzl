load("@bazel_tools//tools/build_defs/repo:git.bzl", "git_repository")

def cisab_repository():
    TAG = "v1.0.0-pre"
    print("downloading cisab {} from cppkg".format(TAG))
    git_repository(
        name = "com_github_mingkaic_cisab",
        remote = "https://github.com/mingkaic/cisab",
        tag = TAG,
    )
