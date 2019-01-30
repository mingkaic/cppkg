load("@bazel_tools//tools/build_defs/repo:git.bzl", "git_repository")

def dependencies():
    if "com_github_nelhage_rules_boost" not in native.existing_rules():
        git_repository(
            name = "com_github_nelhage_rules_boost",
            commit = "8a8853fd755496288995a603ce9aa2685709cd39",
            remote = "https://github.com/nelhage/rules_boost",
        )
