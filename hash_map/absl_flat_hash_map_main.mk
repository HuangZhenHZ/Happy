SRCS = hash_map/absl_flat_hash_map_main.cpp
LDFLAGS = libabsl/libabsl_hash.a libabsl/libabsl_city.a libabsl/libabsl_low_level_hash.a libabsl/libabsl_raw_hash_set.a libabsl/libabsl_raw_logging_internal.a
TARGET = hash_map/absl_flat_hash_map_main
include template.mk
