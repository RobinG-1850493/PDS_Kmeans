cmake_minimum_required(VERSION 3.10)
project(Serial_Kmeans)

find_package (Eigen3 3.3 REQUIRED)

add_executable(kmeans kmeans.cpp)

target_link_libraries (kmeans Eigen3::Eigen)