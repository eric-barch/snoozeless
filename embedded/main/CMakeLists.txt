set(requires "")
idf_build_get_property(target IDF_TARGET)

if(${target} STREQUAL "linux")
    list(APPEND requires esp_stubs esp_event esp-tls esp_http_client protocol_examples_common nvs_flash)
endif()
idf_component_register(SRCS "main.c"
                    INCLUDE_DIRS "."
                    REQUIRES ${requires}
                    EMBED_TXTFILES server_cert.pem)
