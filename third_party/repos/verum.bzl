load("@bazel_tools//tools/build_defs/repo:git.bzl", "git_repository")

def verum_repository():
    TAG = "v1.0.0-pre"
    print("downloading verum {} from cppkg".format(TAG))
    git_repository(
        name = "com_github_mingkaic_verum",
        remote = "https://github.com/mingkaic/verum",
        commit = TAG,
    )
