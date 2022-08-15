idf_component_register(
    SRCS "main.c"
    INCLUDE_DIRS "."
    EMBED_FILES "esp32.png"
)

target_compile_options(${COMPONENT_LIB} PRIVATE -Wno-missing-field-initializers)