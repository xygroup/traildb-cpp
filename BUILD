cc_library(
name = "traildb-cpp",
hdrs = [
"include/ByteManipulations.h",
"include/TrailDBConstructor.h",
"include/TrailDBcpp.h",
"include/TrailDBException.h",
],
deps = [
"//traildb",
],
include_prefix = "include",
visibility = ["//visibility:public"],
)

cc_test(
name = "test",
srcs = [
"tests/test.cpp"
],
size = "small",
deps = [
":traildb-cpp",
"//archive",
"@gtest//:gtest_main",
],
copts = [
"-DBAZEL=1",
"-std=c++14",
"-Iexternal/gtest/include",
"-Iexternal",
],
)