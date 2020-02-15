cmake_minimum_required(VERSION 3.5)
project(mobilenetv2)
set(TARGET_NAME "mobilenetv2")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -pie -fPIE -fPIC -O3")
#set(SOURCE_FILES src/anchor_generator.cpp  src/config.cpp  src/detect.cpp  src/tools.cpp
#        src/anchor_generator.h src/config.h src/tools.h)

include_directories("/media/naruto/CHASESKY/software/opencv-3.4.0/include")
link_directories("/media/naruto/CHASESKY/software/opencv-3.4.0/lib")
set(OpenCV_LIBS opencv_imgproc opencv_core opencv_highgui opencv_calib3d opencv_video opencv_videoio opencv_objdetect opencv_ml opencv_features2d opencv_photo opencv_flann opencv_imgcodecs)
#find_package(OpenCV REQUIRED)

include_directories("${CMAKE_CURRENT_LIST_DIR}/include")#//tvm 三个主要的头文件 dlpack,dmlc,tvm
link_directories("${CMAKE_CURRENT_LIST_DIR}/lib")#//用于获取libtvm_runtime.so

add_executable(mobilenetv2 server_deploy.cpp)
set(EXECUTABLE_OUTPUT_PATH ${CMAKE_CURRENT_LIST_DIR})
#target_link_libraries(${TARGET_NAME} /home/naruto/anaconda3c/lib/libtiff.so.5)
target_link_libraries(${TARGET_NAME} ${OpenCV_LIBS} tvm_runtime dl pthread )
