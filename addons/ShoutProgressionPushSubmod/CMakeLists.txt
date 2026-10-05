cmake_minimum_required(VERSION 3.24)

project(
	ShoutProgressionPushSubmod
	VERSION 0.6.1
	LANGUAGES CXX
)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

list(PREPEND CMAKE_PREFIX_PATH "${CMAKE_CURRENT_SOURCE_DIR}/../../build/commonlib-installed")
find_package(directxtk CONFIG REQUIRED)
find_package(CommonLibSSE 10.0.1 CONFIG REQUIRED)

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
		"${CMAKE_CURRENT_SOURCE_DIR}/package/SKSE/Plugins/ShoutProgressionPushSubmod.ini"
		"$<TARGET_FILE_DIR:${PROJECT_NAME}>/ShoutProgressionPushSubmod.ini"
	COMMAND ${CMAKE_COMMAND} -E make_directory
		"${CMAKE_BINARY_DIR}/Data/Source/Scripts"
	COMMAND ${CMAKE_COMMAND} -E copy_if_different
		"${CMAKE_CURRENT_SOURCE_DIR}/package/Source/Scripts/VoicePushEffectScript.psc"
		"${CMAKE_BINARY_DIR}/Data/Source/Scripts/VoicePushEffectScript.psc"
	COMMAND ${CMAKE_COMMAND} -E copy_if_different
		"${CMAKE_CURRENT_SOURCE_DIR}/package/Source/Scripts/ShoutProgressionPushSubmod.psc"
		"${CMAKE_BINARY_DIR}/Data/Source/Scripts/ShoutProgressionPushSubmod.psc"
)
