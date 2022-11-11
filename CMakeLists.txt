cmake_minimum_required(VERSION 3.22.1 FATAL_ERROR)

project(YinYang
  VERSION 0.1.0
  DESCRIPTION "Make your tools follow the system theme."
  HOMEPAGE_URL "https://www.parkovski.com/yinyang"
  LANGUAGES C
)

set(CMAKE_C_STANDARD 17)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_C_EXTENSIONS OFF)

if(APPLE)
  enable_language(OBJC)
endif()

add_subdirectory(src)
