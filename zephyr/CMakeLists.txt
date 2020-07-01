if(CONFIG_PJON)

zephyr_include_directories(../src)
add_compile_definitions(ZEPHYR)
zephyr_compile_definitions(ZEPHYR)


endif()
