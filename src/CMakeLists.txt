cmake_minimum_required(VERSION 3.10)
project(metno_gif)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Add the path to your local OpenCV installation
set(OpenCV_DIR /usr/lib/cmake/opencv4)

find_package(OpenCV REQUIRED)

# Add the path to your local NetCDF-CXX4 installation
set(NETCDF_CXX4_INCLUDE_DIR /usr/include)
set(NETCDF_CXX4_LIBRARY  /usr/lib/libnetcdf_c++4.so)

include_directories(${OpenCV_INCLUDE_DIRS} ${NETCDF_CXX4_INCLUDE_DIR})

add_executable(metno_gif create_images.cpp)

target_link_libraries(metno_gif ${OpenCV_LIBS} ${NETCDF_CXX4_LIBRARY})

# Set the installation path
set(CMAKE_INSTALL_PREFIX /usr/local)

# Install the executable to the specified path
install(TARGETS metno_gif
    RUNTIME DESTINATION bin
)
