workspace(name = "com_github_mingkaic_cppkg")

# test dependencies

load("//:gtest.bzl", "get_gtest")

get_gtest(name = "com_google_googletest")

# local dependencies

load("//:cppkg.bzl", "dependencies")

dependencies()

# boost dependencies

load("@com_github_nelhage_rules_boost//:boost/boost.bzl", "boost_deps")

boost_deps()
