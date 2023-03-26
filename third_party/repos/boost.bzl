load("@bazel_tools//tools/build_defs/repo:git.bzl", "git_repository")

def boost_repository():
    COMMIT = "1e3a69bf2d5cd10c34b74f066054cd335d033d71"
    print("downloading boost {} from cppkg".format(COMMIT))
    git_repository(
        name = "com_github_nelhage_rules_boost",
        remote = "https://github.com/nelhage/rules_boost",
        commit = COMMIT,
    )
