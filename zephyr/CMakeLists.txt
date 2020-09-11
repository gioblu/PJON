if(CONFIG_PJON)

zephyr_include_directories(../src)

if(CONFIG_PJON_STRATEGY_THROUGHSERIAL)
zephyr_sources(../src/interfaces/ZEPHYR/PJON_ZEPHYR_Interface.cpp)
endif()
endif()
