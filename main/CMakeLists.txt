
   file(GLOB_RECURSE component_sources "*.c")

   idf_component_register(
    SRCS ${component_sources}
    INCLUDE_DIRS 
        "."                                          # main.c, project_top.h
        "hw_level"                                   # hw_level_top.h, hw_level_handle.h
        "hw_level/custom_esp_gpio"
        "hw_level/custom_esp_nvs"
        "hal_level"                                  # hal_level_top.h, hal_level_handle.h
        "hal_level/custom_esp_motor"
        "hal_level/custom_esp_servo"
        "hal_level/custom_esp_uart"
        "app_level"                                  # app_level_top.h
        "app_level/RCcar"
    REQUIRES 
        esp_timer
        driver
        esp_driver_ledc
        led_strip
        nvs_flash 
    )  # Managed Components
