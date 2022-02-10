load("//third_party:repos/boost.bzl", "boost_repository")
load("//third_party:repos/grpc.bzl", "grpc_rules_repository")
load("//third_party:repos/cisab.bzl", "cisab_repository")
load("//third_party:repos/verum.bzl", "verum_repository")

def dependencies(excludes = []):
    ignores = native.existing_rules().keys() + excludes

    if "com_github_mingkaic_cisab" not in ignores:
        cisab_repository()

    if "com_github_nelhage_rules_boost" not in ignores:
        boost_repository()

    if "rules_proto_grpc" not in ignores:
        grpc_rules_repository()


def test_dependencies(excludes = []):
    ignores = native.existing_rules().keys() + excludes

    if "com_github_mingkaic_verum" not in ignores:
        verum_repository()
