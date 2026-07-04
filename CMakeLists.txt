cmake_minimum_required(VERSION 3.24)

project(
	DynamicUnrelentingForce
	VERSION 1.0.0
	LANGUAGES CXX
)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

set(BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(SKSE_SUPPORT_XBYAK OFF CACHE BOOL "" FORCE)
set(ENABLE_SKYRIM_SE ON CACHE BOOL "" FORCE)
set(ENABLE_SKYRIM_AE ON CACHE BOOL "" FORCE)
set(ENABLE_SKYRIM_VR OFF CACHE BOOL "" FORCE)

if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/external/CommonLibSSE-NG-lite/CMakeLists.txt")
	add_subdirectory(external/CommonLibSSE-NG-lite EXCLUDE_FROM_ALL)
else()
	find_package(CommonLibSSE CONFIG REQUIRED)
endif()

add_library(${PROJECT_NAME} SHARED
	src/main.cpp
)

target_compile_definitions(${PROJECT_NAME}
	PRIVATE
		WIN32_LEAN_AND_MEAN
		NOMINMAX
)

target_compile_features(${PROJECT_NAME}
	PRIVATE
		cxx_std_23
)

target_link_libraries(${PROJECT_NAME}
	PRIVATE
		CommonLibSSE::CommonLibSSE
)

target_precompile_headers(${PROJECT_NAME}
	PRIVATE
		<RE/Skyrim.h>
		<SKSE/SKSE.h>
)

set_target_properties(${PROJECT_NAME}
	PROPERTIES
		RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/Data/SKSE/Plugins"
		LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/Data/SKSE/Plugins"
		ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib"
)

foreach(CONFIG_TYPE DEBUG RELEASE RELWITHDEBINFO MINSIZEREL)
	set_target_properties(${PROJECT_NAME}
		PROPERTIES
			RUNTIME_OUTPUT_DIRECTORY_${CONFIG_TYPE} "${CMAKE_BINARY_DIR}/Data/SKSE/Plugins"
			LIBRARY_OUTPUT_DIRECTORY_${CONFIG_TYPE} "${CMAKE_BINARY_DIR}/Data/SKSE/Plugins"
			ARCHIVE_OUTPUT_DIRECTORY_${CONFIG_TYPE} "${CMAKE_BINARY_DIR}/lib"
	)
endforeach()

add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
	COMMAND ${CMAKE_COMMAND} -E copy_if_different
		"${CMAKE_CURRENT_SOURCE_DIR}/package/SKSE/Plugins/DynamicUnrelentingForce.ini"
		"$<TARGET_FILE_DIR:${PROJECT_NAME}>/DynamicUnrelentingForce.ini"
	COMMAND ${CMAKE_COMMAND} -E make_directory
		"${CMAKE_BINARY_DIR}/Data/Source/Scripts"
	COMMAND ${CMAKE_COMMAND} -E copy_if_different
		"${CMAKE_CURRENT_SOURCE_DIR}/package/Source/Scripts/VoicePushEffectScript.psc"
		"${CMAKE_BINARY_DIR}/Data/Source/Scripts/VoicePushEffectScript.psc"
	COMMAND ${CMAKE_COMMAND} -E copy_if_different
		"${CMAKE_CURRENT_SOURCE_DIR}/package/Source/Scripts/DynamicUnrelentingForce.psc"
		"${CMAKE_BINARY_DIR}/Data/Source/Scripts/DynamicUnrelentingForce.psc"
)
