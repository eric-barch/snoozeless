set(requires "")
idf_build_get_property(target IDF_TARGET)

if(${target} STREQUAL "linux")
    list(APPEND requires esp_stubs esp_event esp-tls esp_http_client protocol_examples_common nvs_flash)
endif()

# Collect source files using GLOB.
file(GLOB MAIN_SRCS "*.c")
file(GLOB NVS_UTILS_SRCS "nvs_utils/*.c")
file(GLOB CONSOLE_UTILS_SRCS "console_utils/*.c")
file(GLOB WIFI_UTILS_SRCS "wifi_utils/*.c")
file(GLOB HTTP_UTILS_SRCS "http_utils/*.c")
file(GLOB HTTP_REQUESTS_SRCS "http_requests/*.c")
file(GLOB CREDENTIALS_SRCS "credentials/*.c")

# Combine source files.
list(
    APPEND MAIN_SRCS 
        ${NVS_UTILS_SRCS} 
        ${CONSOLE_UTILS_SRCS} 
        ${WIFI_UTILS_SRCS} 
        ${HTTP_UTILS_SRCS} 
        ${HTTP_REQUESTS_SRCS}
        ${CREDENTIALS_SRCS}
)

idf_component_register(
    SRCS ${MAIN_SRCS}
    INCLUDE_DIRS "."
                 "nvs_utils"
                 "console_utils"
                 "wifi_utils"
                 "http_utils"
                 "http_requests"
                 "credentials"
    REQUIRES ${requires}
    EMBED_TXTFILES server_cert.pem
)
