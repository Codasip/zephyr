# Copyright (c) 2024 Codasip s.r.o.
# SPDX-License-Identifier: Apache-2.0

# add_subdirectory(common)
add_subdirectory(${SOC_SERIES})

zephyr_include_directories(common)
