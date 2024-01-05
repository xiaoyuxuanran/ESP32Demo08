idf_component_register(
    SRCS
    "suwp_init.cpp"
    "wifi.cpp"
    "http_server.cpp"
                    
    INCLUDE_DIRS 
    "include"

    REQUIRES
    freertos
    esp_wifi
    nvs_flash
    lwip
    esp_http_server

    EMBED_FILES 
    "favicon/favicon.ico" 
    "shieru.html"
    )
