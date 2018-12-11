workspace(name = "com_github_mingkaic_cppkg")

# local dependencies

load("//:cppkg.bzl", "dependencies")

dependencies()

# test dependencies

load("//:gtest.bzl", "gtest_repositoru")

gtest_repository(name = "gtest")

# boost dependencies

load("@com_github_nelhage_rules_boost//:boost/boost.bzl", "boost_deps")

boost_deps()
