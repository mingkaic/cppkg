workspace(name = "com_github_mingkaic_cppkg")

# === development ===

load("//third_party:all.bzl", "test_dependencies")
test_dependencies()

load("@com_github_mingkaic_verum//third_party:all.bzl", verum_deps="dependencies")
verum_deps()

load("@bazel_tools//tools/build_defs/repo:git.bzl", "git_repository")
git_repository(
    name = "com_grail_bazel_compdb",
	remote = "https://github.com/grailbio/bazel-compilation-database",
	tag = "0.4.5",
)

# === load all dependencies ===

load("//third_party:all.bzl", "dependencies")
dependencies()

# load boost dependencies
load("@com_github_nelhage_rules_boost//:boost/boost.bzl", "boost_deps")
boost_deps()

# load grpc dependencies
load("@com_github_grpc_grpc//bazel:grpc_deps.bzl", "grpc_deps")
grpc_deps()
load("@com_github_grpc_grpc//bazel:grpc_extra_deps.bzl", "grpc_extra_deps")
grpc_extra_deps()
