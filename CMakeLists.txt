CMAKE_MINIMUM_REQUIRED( VERSION 2.6 )

PROJECT(PixelEffect)

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
#set(CMAKE_CXX_EXTENSIONS OFF)

if (UNIX)
    set(GLEW_INCLUDE_DIR "/usr/local/Cellar/glew/2.1.0/include/GL")
    set(GLEW_LIBRARY     "/usr/local/Cellar/glew/2.1.0/lib/libGLEW.2.1.0.dylib")
    
    set(GLFW_INCLUDE_DIR "/usr/local/Cellar/glfw/3.2.1/lib/include")
    set(GLFW_LIBRARIES   "/usr/local/Cellar/glfw/3.2.1/lib/libglfw.3.2.dylib")

#    add_subdirectory( "/usr/local/Cellar/glfw/lib" )
endif()

find_package(OpenGL   REQUIRED)
find_package(GLEW     REQUIRED)
#find_package(GLFW     REQUIRED)

if (UNIX)
    find_package(PkgConfig REQUIRED)  # find GLFW library
    pkg_search_module(GLFW REQUIRED glfw3)
endif()

MESSAGE(STATUS "Building ${PROJECT_NAME} ...")

SET(PROJECT_MAJOR_VERSION 1)
SET(PROJECT_MINOR_VERSION 0)
SET(PROJECT_VERSION ${PROJECT_MAJOR_VERSION}.${PROJECT_MINOR_VERSION})

SET(INCLUDE_DIR ${CMAKE_SOURCE_DIR}/include)
SET(SOURCE_DIR PixelEffect)
SET(LIB_DIR ${CMAKE_SOURCE_DIR}/lib/x86/Debug)

link_directories(${LIB_DIR})

FILE(GLOB HEADERS
	"${SOURCE_DIR}/PixelEffect.Framework/*.h"
	"${SOURCE_DIR}/PixelEffect.Shared/*.h"
	"${SOURCE_DIR}/PixelEffect.UI/*.h")

FILE(GLOB HEADERS_WINDOWS_LIB "${SOURCE_DIR}/PixelEffect.Windows/*.h")

FILE(GLOB HEADERS_WINDOWS_APP "WindowsApp/*.h")

FILE(MAKE_DIRECTORY includes)

FILE(COPY ${HEADERS} DESTINATION includes)	

if (WIN32)
    FILE(COPY ${HEADERS_WINDOWS_LIB} DESTINATION includes)
endif()

if (UNIX)
    FILE(COPY ${HEADERS_WINDOWS_LIB} DESTINATION includes)
endif()

INCLUDE_DIRECTORIES( 
	${INCLUDE_DIR} 
	${CMAKE_CURRENT_BINARY_DIR}/includes)

FILE(GLOB SOURCES
	"${SOURCE_DIR}/PixelEffect.Framework/*.cpp"
	"${SOURCE_DIR}/PixelEffect.Shared/*.cpp"
	"${SOURCE_DIR}/PixelEffect.UI/*.cpp"
	"${INCLUDE_DIR}/XMLPP/*.hpp")

FILE(GLOB SOURCES_WINDOWS_LIB "${SOURCE_DIR}/PixelEffect.Windows/*.cpp")

FILE(GLOB SOURCES_WINDOWS_APP "WindowsApp/*.cpp")

FILE(MAKE_DIRECTORY sources)

FILE(COPY ${SOURCES} DESTINATION sources)	

if (WIN32)
    FILE(COPY ${SOURCES_WINDOWS_LIB} DESTINATION sources)
endif()

if (UNIX)
    FILE(COPY ${SOURCES_WINDOWS_LIB} DESTINATION sources)
endif()

FILE(GLOB SOURCES_FILES	"${CMAKE_CURRENT_BINARY_DIR}/sources/*.cpp")

if (WIN32)
    add_definitions(-DDEBUG -DOPENGL -DMAJOR_COLUMN_ORDER -DWINDOWS -DAPI_EXPORT -D_CRT_SECURE_NO_WARNINGS)
endif()

if (UNIX)
    add_definitions(-DDEBUG -DOPENGL -DMAJOR_COLUMN_ORDER -DUNIX -D_CRT_SECURE_NO_WARNINGS)
endif()

MESSAGE(STATUS "Compiling ..." )

if (WIN32)
	ADD_LIBRARY(${PROJECT_NAME}WindowsLib STATIC ${SOURCES_FILES})

	set_target_properties(${PROJECT_NAME}WindowsLib PROPERTIES ENABLE_EXPORTS ON)

	ADD_EXECUTABLE(${PROJECT_NAME}WindowsApp ${SOURCES_WINDOWS_APP})

	set_target_properties(${PROJECT_NAME}WindowsApp PROPERTIES ENABLE_EXPORTS ON)

	target_link_libraries(${PROJECT_NAME}WindowsApp
		${OPENGL_LIBRARY}
		glfw3.lib
		glew32s.lib
		${PROJECT_NAME}WindowsLib)

endif()



if (UNIX)
	ADD_LIBRARY(${PROJECT_NAME}LinuxLib STATIC ${SOURCES_FILES})

	set_target_properties(${PROJECT_NAME}LinuxLib PROPERTIES ENABLE_EXPORTS ON)

	ADD_EXECUTABLE(PixelEffectLinuxApp ${SOURCES_WINDOWS_APP})

	set_target_properties(${PROJECT_NAME}LinuxApp PROPERTIES ENABLE_EXPORTS ON)

	target_link_libraries(${PROJECT_NAME}LinuxApp
		${OPENGL_LIBRARY}
#		${GLFW_LIBRARIES}
"/usr/local/Cellar/glfw/3.2.1/lib/libglfw.3.2.dylib"
		${GLEW_LIBRARIES}
		${PROJECT_NAME}LinuxLib)
endif()

MESSAGE(STATUS "COMPLETED !" )
