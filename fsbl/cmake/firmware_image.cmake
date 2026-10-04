# SPDX-FileCopyrightText: 2026 Nikita Prismakov
# SPDX-License-Identifier: Apache-2.0

# ---------------------------------------------------------------------------
# Link-time and post-build steps of a firmware executable.
# ---------------------------------------------------------------------------

# Make ${target} relink when its linker script ${script} is edited.
function(stm32n6_track_linker_script target script)
    set_property(TARGET ${target} APPEND PROPERTY LINK_DEPENDS ${script})
endfunction()

# After every link of ${target}: write the raw image <target>.bin beside the
# .elf (the SigningTool takes it as input) and print the section sizes.
function(stm32n6_emit_image target)
    add_custom_command(TARGET ${target} POST_BUILD
        COMMAND ${CMAKE_OBJCOPY} -O binary
        "$<TARGET_FILE:${target}>"
        "$<TARGET_FILE_DIR:${target}>/${target}.bin"
        COMMAND ${CMAKE_SIZE} -B "$<TARGET_FILE:${target}>")
endfunction()
