load("@bazel_tools//tools/build_defs/repo:git.bzl", "git_repository")

def grpc_repository():
    TAG = "v1.26.0"
    print("downloading grpc {} from cppkg".format(TAG))
    git_repository(
        name = "com_github_grpc_grpc",
        remote = 'https://github.com/grpc/grpc.git',
        tag = TAG,
    )
