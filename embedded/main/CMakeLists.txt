idf_build_get_property(target IDF_TARGET)

if(${target} STREQUAL "linux")
    list(APPEND requires esp_stubs esp_event esp-tls esp_http_client protocol_examples_common nvs_flash)
endif()

file(GLOB_RECURSE MAIN_SRCS "*.c")

idf_component_register(
    SRCS ${MAIN_SRCS}
    INCLUDE_DIRS "."
                 "controllers"
                 "services"
                 "state"
                 "utilities"
    EMBED_TXTFILES server_cert.pem
)
