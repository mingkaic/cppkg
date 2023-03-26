load("@bazel_tools//tools/build_defs/repo:git.bzl", "git_repository")

def verum_repository():
    COMMIT = "35d31528fcdd3cfdb6b69d1e46a948dd176f64a6"
    print("downloading verum {} from cppkg".format(COMMIT))
    git_repository(
        name = "com_github_mingkaic_verum",
        remote = "https://github.com/mingkaic/verum",
        commit = COMMIT,
    )
