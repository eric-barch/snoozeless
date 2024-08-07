idf_build_get_property(target IDF_TARGET)
file(GLOB_RECURSE MAIN_SRCS "*.c" "*.cpp")
idf_component_register(
    SRCS ${MAIN_SRCS}
    INCLUDE_DIRS "."
                 "Alarm"
                 "Alarms"
                 "ApiRequest"
                 "Button"
                 "Buzzer"
                 "Countdown"
                 "CurrentTime"
                 "Device"
                 "Display"
                 "NonVolatileStorage"
                 "Session"
                 "StateMachine"
                 "StateStream"
                 "WifiConnection"
    EMBED_TXTFILES server_cert.pem
)
