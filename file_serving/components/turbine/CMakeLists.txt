# Add TFLite and Model Parameters
file(GLOB_RECURSE MODEL_SOURCES "tflite-model/*.cpp" "tflite-model/*.c" "model-parameters/*.cpp" "model-parameters/*.c")

list(APPEND SOURCES ${MODEL_SOURCES} "turbine_wrapper.cpp")

idf_component_register(SRCS ${SOURCES}
                       INCLUDE_DIRS "include"
                       PRIV_INCLUDE_DIRS "." "model-parameters" "tflite-model"
                       REQUIRES esp_timer driver edge_impulse_core)

target_compile_options(${COMPONENT_LIB} PRIVATE -Wno-format -Wno-missing-field-initializers -Wno-unused-variable -Wno-deprecated-declarations)