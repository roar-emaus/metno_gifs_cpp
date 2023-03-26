cmake_minimum_required(VERSION 3.10)
project(create_images)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Add the path to your local OpenCV installation
set(OpenCV_DIR /home/roar/projects/metno_gifs/c++/libs/opencv/build/local-install/lib/cmake/opencv4)

find_package(OpenCV REQUIRED)

# Add the path to your local NetCDF-CXX4 installation
set(NETCDF_CXX4_INCLUDE_DIR /home/roar/projects/metno_gifs/c++/libs/netcdf-cxx4-4.3.1/build/local-install/include)
set(NETCDF_CXX4_LIBRARY  /home/roar/projects/metno_gifs/c++/libs/netcdf-cxx4-4.3.1/build/local-install/lib /home/roar/projects/metno_gifs/c++/libs/netcdf-cxx4-4.3.1/build/local-install/lib/libnetcdf-cxx4.so)

include_directories(${OpenCV_INCLUDE_DIRS} ${NETCDF_CXX4_INCLUDE_DIR})

add_executable(create_images create_images.cpp)

target_link_libraries(create_images ${OpenCV_LIBS} ${NETCDF_CXX4_LIBRARY})

