cmake_minimum_required(VERSION 3.19)
project(AzdararBot)

if (NOT CMAKE_PREFIX_PATH)
    message(WARNING "CMAKE_PREFIX_PATH is not defined, you may need to set it "
            "(-DCMAKE_PREFIX_PATH=\"path/to/Qt/lib/cmake\" or -DCMAKE_PREFIX_PATH=/usr/include/{host}/qt{version}/ on Ubuntu)")
endif ()

# Common build configs #################################################################################################
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/install)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/install)

# Find includes in corresponding build directories
include_directories(${CMAKE_CURRENT_BINARY_DIR})
include_directories(${CMAKE_CURRENT_SOURCE_DIR})

set(CMAKE_CXX_STANDARD 17)

########################################################################################################################
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

set(QT_VERSION 5)
set(REQUIRED_QT_LIBS Core Sql Xml Network)

find_package(Qt${QT_VERSION} COMPONENTS ${REQUIRED_QT_LIBS} REQUIRED)
set(REQUIRED_LIBS_QUALIFIED Qt5::Xml Qt5::Sql Qt5::Network)

# POCO
find_package(OpenSSL REQUIRED)
find_package(Poco COMPONENTS Foundation Net NetSSL Crypto JSON REQUIRED)

########################################################################################################################
add_subdirectory(thirdparty/tgbot-cpp)
LIST(APPEND REQUIRED_LIBS_QUALIFIED TgBot)

# application executable ###############################################################################################
file(GLOB SOURCES "*.cpp" "*.h")

add_executable(${PROJECT_NAME} ${SOURCES} ${FORMS})

target_link_libraries(${PROJECT_NAME} PRIVATE ${REQUIRED_LIBS_QUALIFIED})
target_link_libraries(${PROJECT_NAME} PRIVATE ${OpenSSL_LIBRARIES})
target_link_libraries(${PROJECT_NAME} PRIVATE ${Poco_LIBRARIES})

########################################################################################################################