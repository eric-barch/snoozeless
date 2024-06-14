set(requires "")
idf_build_get_property(target IDF_TARGET)

if(${target} STREQUAL "linux")
    list(APPEND requires esp_stubs esp_event esp-tls esp_http_client protocol_examples_common nvs_flash)
endif()

# Collect source files using GLOB.
file(GLOB MAIN_SRCS "*.c")
file(GLOB NVS_SRCS "nvs_utils/*.c")
file(GLOB CONSOLE_SRCS "console_utils/*.c")

# Combine source files.
list(APPEND MAIN_SRCS ${NVS_SRCS} ${CONSOLE_SRCS})

idf_component_register(SRCS ${MAIN_SRCS}
                    INCLUDE_DIRS "."
                                 "nvs_utils"
                                 "console_utils"
                    REQUIRES ${requires}
                    EMBED_TXTFILES server_cert.pem)
