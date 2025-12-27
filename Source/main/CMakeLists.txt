set(COMPONENT_SRCS "bluebox.c")
set(COMPONENT_ADD_INCLUDEDIRS .)

set(COMPONENT_REQUIRES "audio_pipeline" "audio_hal" "esp_peripherals"    "audio_stream"
                       "esp_eth" "esp_netif" "spiffs" "nvs_flash"
                       "esp_wifi" "esp_http_client" "esp_http_server"
                       "bluetooth_service" "esp_peripherals" "input_key_service")

register_component()

# Fill spiffs file system with defaqukt content. Ereases existing content
spiffs_create_partition_image(storage ../resources FLASH_IN_PROJECT)
