idf_component_register(
    SRCS "connect_wifi.c" "main.c"
    INCLUDE_DIRS "."
    REQUIRES freertos esp_http_client driver
)
