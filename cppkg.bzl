load("@bazel_tools//tools/build_defs/repo:git.bzl", "git_repository")

def dependencies():
    if "com_github_nelhage_rules_boost" not in native.existing_rules():
        git_repository(
            name = "com_github_nelhage_rules_boost",
            commit = "f4d1c6d4ceedabda72bcb2530694afc93cabda61",
            remote = "https://github.com/mingkaic/rules_boost",
        )
