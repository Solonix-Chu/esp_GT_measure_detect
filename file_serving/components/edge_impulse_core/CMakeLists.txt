set(SDK_PATH "edge-impulse-sdk")

# Add Core SDK sources
file(GLOB_RECURSE SDK_SOURCES "${SDK_PATH}/classifier/*.cpp" "${SDK_PATH}/classifier/*.c" 
                            "${SDK_PATH}/dsp/*.cpp" "${SDK_PATH}/dsp/*.c"
                            "${SDK_PATH}/third_party/*.c" "${SDK_PATH}/third_party/*.cpp"
                            "${SDK_PATH}/tensorflow/*.c" "${SDK_PATH}/tensorflow/*.cpp" "${SDK_PATH}/tensorflow/*.cc")

# Add ESP32 Porting
file(GLOB_RECURSE PORTING_SOURCES "${SDK_PATH}/porting/espressif/*.cpp" "${SDK_PATH}/porting/espressif/*.c")

# Remove CMSIS from SDK sources
list(FILTER SDK_SOURCES EXCLUDE REGEX ".*CMSIS.*")

# Remove ei_run_classifier_c.cpp to avoid dependency on model variables in shared lib
list(FILTER SDK_SOURCES EXCLUDE REGEX ".*ei_run_classifier_c\.cpp")

list(APPEND SOURCES ${SDK_SOURCES} ${PORTING_SOURCES})

idf_component_register(SRCS ${SOURCES}
                       INCLUDE_DIRS "." "edge-impulse-sdk" 
                                    "${SDK_PATH}/porting" 
                                    "${SDK_PATH}/porting/espressif" 
                                    "${SDK_PATH}/porting/espressif/ESP-NN/include"
                                    "${SDK_PATH}/porting/espressif/esp-dsp/modules/common/include"
                                    "${SDK_PATH}/porting/espressif/esp-dsp/modules/fft/include"
                       REQUIRES esp_timer driver)

target_compile_options(${COMPONENT_LIB} PRIVATE -Wno-format -Wno-missing-field-initializers -Wno-deprecated-declarations)
