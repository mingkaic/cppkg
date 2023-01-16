load("@bazel_tools//tools/build_defs/repo:git.bzl", "git_repository")

def grpc_repository():
    git_repository(
        name = "com_github_grpc_grpc",
        remote = 'https://github.com/grpc/grpc.git',
        commit = 'dc78581af30da834b7b95572f109bf6c708686e0',
    )
