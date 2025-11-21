
   file(GLOB_RECURSE component_sources "*.c")

   idf_component_register(
    SRCS ${component_sources}
    INCLUDE_DIRS "custom_esp_gpio" "." 
    REQUIRES driver nvs_flash esp_timer esp_driver_ledc # 내장 ESP-IDF 컴포넌트
    led_strip)  # 관리형 컴포넌트 (Managed Components)