workspace(name = "com_github_mingkaic_cppkg")

# local dependencies

load("//:cppkg.bzl", "dependencies")
dependencies()

load("@com_github_nelhage_rules_boost//:boost/boost.bzl", "boost_deps")
boost_deps()

# test dependencies

load("//:gtest.bzl", "gtest_repository")
gtest_repository(name = "gtest")
