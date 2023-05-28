cmake_minimum_required(VERSION 3.5)
set(PROJECT_NAME Qt)
project(${PROJECT_NAME})

set(CMAKE_CXX_STANDARD 17)

set(QT_DIR "C:/dev/Qt/QtRoot")


file(GLOB_RECURSE all_files_folders LIST_DIRECTORIES true
        ${QT_DIR}
        )

SET(PRJ_DIRECTORIES "")
FOREACH(file_or_folder ${all_files_folders})
    IF(IS_DIRECTORY ${file_or_folder})
        LIST(APPEND PRJ_DIRECTORIES ${file_or_folder})
    ENDIF()
ENDFOREACH()

file(GLOB_RECURSE PRJ_SOURS_FILES
        ${QT_DIR}/*.cpp
        )

file(GLOB_RECURSE PRJ_HEADER_FILES
        ${QT_DIR}/*.h
        )


add_compile_definitions(MINGW)


add_executable(${PROJECT_NAME}
        "${PRJ_SOURS_FILES}"
        "${PRJ_HEADER_FILES}"
        main.cpp
        )

include_directories(${PRJ_DIRECTORIES})

