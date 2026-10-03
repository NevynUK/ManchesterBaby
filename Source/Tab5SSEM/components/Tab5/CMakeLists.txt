idf_component_register(SRCS "SDCard.cpp" "Touch.cpp" "Rtc.cpp"
                    INCLUDE_DIRS "."
                    REQUIRES m5gfx driver freertos sdmmc fatfs esp_driver_i2c)
