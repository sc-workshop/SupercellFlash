set(TARGET "SupercellFlashTest")

add_executable(${TARGET}
    "test/main.cpp"
)

sc_core_base_setup(${TARGET})
target_link_libraries(${TARGET} PUBLIC SupercellFlash)
set_target_properties(${TARGET} PROPERTIES
    FOLDER Supercell/CLI
)