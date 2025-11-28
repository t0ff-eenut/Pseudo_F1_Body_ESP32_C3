
   file(GLOB_RECURSE component_sources "*.c")

   idf_component_register(
    SRCS ${component_sources}
    INCLUDE_DIRS 
        "."                                          # main.c, project_top.h
        "hw_level"                                   # hw_level_top.h, hw_level_handle.h
        "hw_level/custom_esp_gpio"
        "hw_level/custom_esp_nvs"
    REQUIRES 
        esp_timer
        driver
        esp_driver_ledc
        led_strip
        nvs_flash 
    )  # 관리형 컴포넌트 (Managed Components)
