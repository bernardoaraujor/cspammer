cmake_minimum_required(VERSION 3.11)

cmake_policy(VERSION ${CMAKE_MAJOR_VERSION}.${CMAKE_MINOR_VERSION})

project(cspammer DESCRIPTION "An IOTA Spammer Client")

enable_language(C)
enable_testing()

# fetch iota.c
include(FetchContent)
FetchContent_Declare(
        iota.c
        GIT_REPOSITORY http://github.com/iotaledger/iota.c.git
        GIT_TAG 7743c91a02a32001a1158bdf4cee3580ce973991
)

if(${CMAKE_VERSION} VERSION_LESS 3.14)
    macro(FetchContent_MakeAvailable NAME)
        FetchContent_GetProperties(${NAME})
        if(NOT ${NAME}_POPULATED)
            FetchContent_Populate(${NAME})
            add_subdirectory(${${NAME}_SOURCE_DIR} ${${NAME}_BINARY_DIR})
        endif()
    endmacro()
endif()

message(STATUS "Fetching iota.c")
FetchContent_MakeAvailable(iota.c)

# external libs module
include(ExternalProject)

# external libs
include(cmake/argtable3.cmake)

# libs in the sandbox
link_directories("${CMAKE_INSTALL_PREFIX}/lib")



# your source files
add_executable(${CMAKE_PROJECT_NAME}
        "main.c"
        "cspammer.c"
        "cspammer_logger.c"
        )

target_include_directories(${CMAKE_PROJECT_NAME} PRIVATE
        "${PROJECT_SOURCE_DIR}"
        "${CMAKE_INSTALL_PREFIX}/include"
        "${CMAKE_INSTALL_PREFIX}/include/cjson"
        "${iota.c_SOURCE_DIR}"
        )

target_link_libraries(${CMAKE_PROJECT_NAME} PUBLIC
        cclient
        cjson # iota.c
        http_parser # iota.c
        mbedtls # iota.c
        mbedcrypto # iota.c
        mbedx509 # iota.c
        common
        logger # iota_common
        keccak # iota_common
        argtable3
        )

configure_file(${CMAKE_CURRENT_SOURCE_DIR}/pem.txt
        ${CMAKE_CURRENT_BINARY_DIR} COPYONLY
        )
