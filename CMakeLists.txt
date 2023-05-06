cmake_minimum_required(VERSION 3.16)

project(cuda)
set(CMAKE_CUDA_SEPARABLE_COMPILATION ON)
SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -pthread")
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_COMPILER "clang++")


find_package(LLVM REQUIRED CONFIG)

add_executable(cuda PGSP/main.cpp PGSP/DataParser.h
        PGSP/GspEngineCpu.cpp PGSP/GspEngineCpu.h PGSP/HashEngineCpu.cpp PGSP/HashEngineCpu.h
        PGSP/HashNode.cpp PGSP/HashNode.h PGSP/HelperTypes.h PGSP/IEngine.h PGSP/profile.cpp PGSP/profile.h
        PGSP/SimpleNode.h PGSP/SPSPMEngineCpu.h PGSP/SPSPMEngineCpu.cpp PGSP/SPSPMEngineGpu.h PGSP/sycl_utiils.h
        PGSP/SyclQueue.h PGSP/utils.cpp PGSP/utils.h)
target_compile_options(cuda PUBLIC -fsycl -fsycl-targets=nvptx64-nvidia-cuda)
target_link_libraries(cuda sycl)
target_link_options(cuda PUBLIC -fsycl -fsycl-targets=nvptx64-nvidia-cuda)

install(TARGETS cuda RUNTIME DESTINATION bin)