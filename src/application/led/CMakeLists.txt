# SPDX-FileCopyrightText: 2026 Nikita Prismakov
# SPDX-License-Identifier: Apache-2.0

# Heartbeat LED thread.
add_library(app_led led_thread.c)
target_include_directories(app_led PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(app_led PRIVATE app_config bsp threadx)
