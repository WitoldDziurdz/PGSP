cmake_minimum_required(VERSION 3.16)

project(cuda)
set(CMAKE_CUDA_SEPARABLE_COMPILATION ON)
SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -pthread")
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_COMPILER "clang++")


find_package(LLVM REQUIRED CONFIG)

add_executable(cuda PGSP/main.cpp PGSP/src/core/data_parser.h
        PGSP/src/cpu/GspEngineCpu.cpp PGSP/src/cpu/GspEngineCpu.h PGSP/src/cpu/HashEngineCpu.cpp PGSP/src/cpu/HashEngineCpu.h
        PGSP/src/cpu/HashNode.cpp PGSP/src/cpu/HashNode.h PGSP/src/core/helper_types.h PGSP/src/IEngine.h PGSP/src/core/profile.cpp PGSP/src/core/profile.h
        PGSP/src/cpu/SimpleNode.h PGSP/src/cpu/SPSPMEngineCpu.h PGSP/src/cpu/SPSPMEngineCpu.cpp PGSP/src/gpu/SPSPMEngineGpu.h PGSP/src/core/sycl_utiils.h
        PGSP/src/core/utils.cpp PGSP/src/core/utils.h)
target_compile_options(cuda PUBLIC -fsycl -fsycl-targets=nvptx64-nvidia-cuda)
target_link_libraries(cuda sycl)
target_link_options(cuda PUBLIC -fsycl -fsycl-targets=nvptx64-nvidia-cuda)

install(TARGETS cuda RUNTIME DESTINATION bin)

set(DATA_FILES_DIR ${CMAKE_SOURCE_DIR}/PGSP/input)
set(OUTPUT_DATA_FILES_DIR ${CMAKE_BINARY_DIR}/)

file(COPY ${DATA_FILES_DIR} DESTINATION ${OUTPUT_DATA_FILES_DIR})