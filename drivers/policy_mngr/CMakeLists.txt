# SPDX-License-Identifier: Apache-2.0

zephyr_library()

zephyr_library_sources_ifdef(CONFIG_POLICY_MNGR_CODASIP_0_02 policy_mngr_codasip_0_02.c)
