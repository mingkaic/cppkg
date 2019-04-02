load("@bazel_tools//tools/build_defs/repo:git.bzl", "git_repository")

def dependencies():
    if "com_github_nelhage_rules_boost" not in native.existing_rules():
        git_repository(
            name = "com_github_nelhage_rules_boost",
            commit = "98fa3377d01e8ad66616583fab5d18cf25e46e51",
            remote = "https://github.com/nelhage/rules_boost",
        )
