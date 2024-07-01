idf_build_get_property(target IDF_TARGET)
file(GLOB_RECURSE MAIN_SRCS "*.c")
idf_component_register(
    SRCS ${MAIN_SRCS}
    INCLUDE_DIRS "."
    EMBED_TXTFILES server_cert.pem
)
