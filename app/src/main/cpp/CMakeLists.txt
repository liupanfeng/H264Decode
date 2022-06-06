
cmake_minimum_required(VERSION 3.10.2)
project("h264decode")

#导入头文件
include_directories("${CMAKE_SOURCE_DIR}/include/")

#批量导入源文件  源文件跟CMakeLists 在同一个目录的情况
file(GLOB SOURCE *.cpp *.c)

#不同源的情况
#file(GLOB SOURCE
#        ${CMAKE_SOURCE_DIR}/include/*.cpp
#        ${CMAKE_SOURCE_DIR}/include/*.c
#        ${CMAKE_SOURCE_DIR}/*.cpp)



#导入库文件
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -L${CMAKE_SOURCE_DIR}/jniLibs/${CMAKE_ANDROID_ARCH_ABI}")

## link 三方库
add_library( ffmpeg
        SHARED
        IMPORTED )

set_target_properties( ffmpeg
        PROPERTIES IMPORTED_LOCATION
        ${CMAKE_SOURCE_DIR}/../jniLibs/${CMAKE_ANDROID_ARCH_ABI}/libffmpeg.so
        #/Users/jesson/Desktop/FFmpegDemo/app/libs/${CMAKE_ANDROID_ARCH_ABI}/libffmpeg.so
        )


add_library(
             native-lib

             SHARED

             ${SOURCE})


find_library(
              log-lib

              log )


target_link_libraries(
                       native-lib

                        ffmpeg
                        -landroid #native_window
                        -ljnigraphics #bitmap
                        -lOpenSLES #openSLES
                        z
                        m

                       ${log-lib} )