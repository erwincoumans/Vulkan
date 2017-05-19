#
# VKTS Example CMake file.
#

cmake_minimum_required(VERSION 3.2)

set (VKTS_Example "VKTS_ShaderGenerator")

project (${VKTS_Example})

set(VKTS_WSI "" CACHE STRING "Enter VKTS_DISPLAY_VISUAL for alternative WSI.")

include_directories(
            ${CMAKE_CURRENT_SOURCE_DIR}/../VKTS_External/include
			${CMAKE_CURRENT_SOURCE_DIR}/../VKTS/include
)


if (${CMAKE_SYSTEM_PROCESSOR} MATCHES "arm")

	set(VKTS_ARCHITECTURE "arm")
	
else ()

	set(VKTS_ARCHITECTURE "intel")
	
endif ()

if (CMAKE_SIZEOF_VOID_P MATCHES 8)

	set(VKTS_BITS "64")
	
else ()

	set(VKTS_BITS "32")
	
endif ()

set(VKTS_RELATIVE_PATH "..")

if (${ANDROID_PLATFORM} MATCHES "android")

    include(AndroidNdkModules)
    android_ndk_import_module_cpufeatures()
    android_ndk_import_module_native_app_glue()
    
    set(VKTS_RELATIVE_PATH "../AndroidStudio")
    
    string(TOLOWER ${CMAKE_BUILD_TYPE} VKTS_BUILD_TYPE)
    
    set(VKTS_LIB .externalNativeBuild/cmake/${VKTS_BUILD_TYPE}/${ANDROID_ABI}/lib)
    
    set(VKTS_ADDITIONAL_LIBS android log vulkan cpufeatures native_app_glue)
    
elseif (${CMAKE_SYSTEM_NAME} MATCHES "Windows")

	set(VKTS_OS "Windows")
	
	set(VKTS_WINDOW "Win32")

    if (${CMAKE_CXX_COMPILER_ID} STREQUAL MSVC)
		
		set(VKTS_COMPILER "MSVC")

		set(VKTS_LIB ${VKTS_COMPILER}/lib)

        add_definitions(-D_CRT_SECURE_NO_WARNINGS)
		
	else ()
        
		set(VKTS_COMPILER "GNU")
		
		set(VKTS_LIB "build/lib")
		
    endif ()        

	set(VKTS_ADDITIONAL_LIBS vulkan-1 WinMM Xinput9_1_0 Pdh Psapi)
	
    find_path(Vulkan_INCLUDE_DIR NAMES vulkan/vulkan.h PATHS "$ENV{VULKAN_SDK}/Include")
    include_directories(AFTER ${Vulkan_INCLUDE_DIR})
	
    if (${VKTS_BITS} MATCHES "64")
    
        find_path(Vulkan_LIBRARY_DIR NAMES vulkan-1.lib HINTS "$ENV{VULKAN_SDK}/Lib")
       
    else ()
    
        find_path(Vulkan_LIBRARY_DIR NAMES vulkan-1.lib HINTS "$ENV{VULKAN_SDK}/Lib32")
            
    endif ()
    
    link_directories(${Vulkan_LIBRARY_DIR})
    
elseif (${CMAKE_SYSTEM_NAME} MATCHES "Linux")

	set(VKTS_OS "Linux")
	
	set(VKTS_COMPILER "GNU")

	set(VKTS_LIB "build/lib")

	if (${VKTS_WSI} MATCHES "VKTS_DISPLAY_VISUAL")
		add_definitions(-DVKTS_DISPLAY_VISUAL)

		set(VKTS_WINDOW "Display")

		set(VKTS_ADDITIONAL_LIBS vulkan pthread)
	elseif (${VKTS_WSI} MATCHES "VKTS_WAYLAND_VISUAL")
		add_definitions(-DVKTS_WAYLAND_VISUAL)

		set(VKTS_WINDOW "Wayland")

		set(VKTS_ADDITIONAL_LIBS vulkan pthread wayland-client wayland-cursor)
	else ()
		set(VKTS_WINDOW "Xlib")
		
		set(VKTS_ADDITIONAL_LIBS vulkan pthread xcb X11-xcb Xrandr X11)
	endif ()

endif ()

link_directories(
        ${CMAKE_CURRENT_SOURCE_DIR}/${VKTS_RELATIVE_PATH}/VKTS_PKG_Core/${VKTS_LIB}
		${CMAKE_CURRENT_SOURCE_DIR}/${VKTS_RELATIVE_PATH}/VKTS_PKG_Entity/${VKTS_LIB}
		${CMAKE_CURRENT_SOURCE_DIR}/${VKTS_RELATIVE_PATH}/VKTS_PKG_Gui/${VKTS_LIB}
		${CMAKE_CURRENT_SOURCE_DIR}/${VKTS_RELATIVE_PATH}/VKTS_PKG_Image/${VKTS_LIB}
		${CMAKE_CURRENT_SOURCE_DIR}/${VKTS_RELATIVE_PATH}/VKTS_PKG_Interactive/${VKTS_LIB}
		${CMAKE_CURRENT_SOURCE_DIR}/${VKTS_RELATIVE_PATH}/VKTS_PKG_Math/${VKTS_LIB}
		${CMAKE_CURRENT_SOURCE_DIR}/${VKTS_RELATIVE_PATH}/VKTS_PKG_Runtime/${VKTS_LIB}
		${CMAKE_CURRENT_SOURCE_DIR}/${VKTS_RELATIVE_PATH}/VKTS_PKG_Scenegraph/${VKTS_LIB}
		${CMAKE_CURRENT_SOURCE_DIR}/${VKTS_RELATIVE_PATH}/VKTS_PKG_VulkanWrapper/${VKTS_LIB}
		${CMAKE_CURRENT_SOURCE_DIR}/${VKTS_RELATIVE_PATH}/VKTS_PKG_VulkanComposition/${VKTS_LIB}
		${CMAKE_CURRENT_SOURCE_DIR}/${VKTS_RELATIVE_PATH}/VKTS_PKG_VulkanScenegraph/${VKTS_LIB}
		${CMAKE_CURRENT_SOURCE_DIR}/${VKTS_RELATIVE_PATH}/VKTS_PKG_VulkanGui/${VKTS_LIB}
		${CMAKE_CURRENT_SOURCE_DIR}/${VKTS_RELATIVE_PATH}/VKTS_PKG_Window/${VKTS_LIB}
		${CMAKE_CURRENT_SOURCE_DIR}/${VKTS_RELATIVE_PATH}/VKTS_PKG_VulkanWindow/${VKTS_LIB}
)

file(GLOB_RECURSE CPP_FILES ${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp)

if (${ANDROID_PLATFORM} MATCHES "android")
    add_library(${VKTS_Example} SHARED ${CPP_FILES})
else()
    add_executable(${VKTS_Example} ${CPP_FILES})
endif ()

set_property(TARGET ${VKTS_Example} PROPERTY RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/../VKTS_Binaries)
set_property(TARGET ${VKTS_Example} PROPERTY RUNTIME_OUTPUT_DIRECTORY_DEBUG ${CMAKE_CURRENT_SOURCE_DIR}/../VKTS_Binaries)
set_property(TARGET ${VKTS_Example} PROPERTY RUNTIME_OUTPUT_DIRECTORY_RELEASE ${CMAKE_CURRENT_SOURCE_DIR}/../VKTS_Binaries)
set_property(TARGET ${VKTS_Example} PROPERTY RUNTIME_OUTPUT_DIRECTORY_MINSIZEREL ${CMAKE_CURRENT_SOURCE_DIR}/../VKTS_Binaries)
set_property(TARGET ${VKTS_Example} PROPERTY RUNTIME_OUTPUT_DIRECTORY_RELWITHDEBINFO ${CMAKE_CURRENT_SOURCE_DIR}/../VKTS_Binaries)

set_property(TARGET ${VKTS_Example} PROPERTY CXX_STANDARD 11)
set_property(TARGET ${VKTS_Example} PROPERTY CXX_STANDARD_REQUIRED ON)

target_link_libraries(${VKTS_Example}
	VKTS_PKG_VulkanGui
	VKTS_PKG_Gui
	VKTS_PKG_VulkanScenegraph
	VKTS_PKG_Scenegraph
	VKTS_PKG_VulkanComposition
	VKTS_PKG_VulkanWindow
	VKTS_PKG_VulkanWrapper
	VKTS_PKG_Window
	VKTS_PKG_Interactive
	VKTS_PKG_Entity
	VKTS_PKG_Image
	VKTS_PKG_Math
	VKTS_PKG_Runtime
	VKTS_PKG_Core
${VKTS_ADDITIONAL_LIBS})

