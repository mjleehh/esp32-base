idf_component_register(
        SRC_DIRS src
        INCLUDE_DIRS include
        REQUIRES liblvgl)

target_compile_definitions(${COMPONENT_TARGET} PRIVATE "-D TAG=\"${COMPONENT_NAME}\"")
