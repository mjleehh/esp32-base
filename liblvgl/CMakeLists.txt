file(GLOB SRCS
        lvgl/lv_core/*.c
        lvgl/lv_hal/*.c
        lvgl/lv_objx/*.c
        lvgl/lv_fonts/*.c
        lvgl/lv_misc/*.c
        lvgl/lv_themes/*.c
        lvgl/lv_draw/*.c)

idf_component_register(
        NAME liblvgl
        SRCS ${SRCS}
        INCLUDE_DIRS .)

# there is a narrowing issue in the lvgl library
#
# making the warning fatal, will result in compilaiton failure
target_compile_options(${COMPONENT_TARGET} PUBLIC -Wno-error=narrowing)
