idf_build_get_property(target IDF_TARGET)
file(GLOB_RECURSE MAIN_SRCS "*.c" "*.cpp")
idf_component_register(
    SRCS ${MAIN_SRCS}
    INCLUDE_DIRS "."
                 "Alarm"
                 "ApiRequest"
                 "Buzzer"
                 "CurrentTime"
                 "Device"
                 "Display"
                 "NvsManager"
                 "Session"
                 "WifiManager"
    EMBED_TXTFILES server_cert.pem
)
