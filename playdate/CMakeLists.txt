cmake_minimum_required(VERSION 3.14)
set(CMAKE_C_STANDARD 11)

set(ENVSDK $ENV{PLAYDATE_SDK_PATH})

if (NOT ${ENVSDK} STREQUAL "")
	# Convert path from Windows
	file(TO_CMAKE_PATH ${ENVSDK} SDK)
else()
	execute_process(
			COMMAND bash -c "egrep '^\\s*SDKRoot' $HOME/.Playdate/config"
			COMMAND head -n 1
			COMMAND cut -c9-
			OUTPUT_VARIABLE SDK
			OUTPUT_STRIP_TRAILING_WHITESPACE
	)
endif()

if (NOT EXISTS ${SDK})
	message(FATAL_ERROR "SDK Path not found; set ENV value PLAYDATE_SDK_PATH")
	return()
endif()

set(CMAKE_CONFIGURATION_TYPES "Debug;Release")
set(CMAKE_XCODE_GENERATE_SCHEME TRUE)

# Game Name Customization
set(PLAYDATE_GAME_NAME template)
set(PLAYDATE_GAME_DEVICE template_DEVICE)

project(${PLAYDATE_GAME_NAME} C ASM)

file(GLOB SRC_FILES 
	src/main.c
    game/src/util.c
    game/src/vector.c
    game/src/matrix.c
    game/src/context.c
    game/src/polygon.c
    game/src/font.c
    game/src/fastRandom.c
    game/src/entity.c
    game/src/data.c
    game/src/renderer.c
    game/src/game.c
    game/src/menu.c
    game/src/tests.c
    game/quickjs/cutils.c
    game/quickjs/libbf.c
    game/quickjs/libregexp.c
    game/quickjs/libunicode.c
    game/quickjs/quickjs-libc.c
    game/quickjs/quickjs.c
)

set(CMAKE_C_FLAGS "-DCONFIG_VERSION=\"2021-03-27\" -DCONFIG_BIGNUM -Denviron=NULL")

if (TOOLCHAIN STREQUAL "armgcc")
	add_executable(${PLAYDATE_GAME_DEVICE}
		${SRC_FILES}
	)
	target_include_directories(${PLAYDATE_GAME_DEVICE} 
		PRIVATE
		./game/include
	)
else()
	add_library(${PLAYDATE_GAME_NAME} SHARED
	    ${SRC_FILES}
	)
	target_include_directories(${PLAYDATE_GAME_NAME} 
		PRIVATE
		./game/include
	)
endif()

include(${SDK}/C_API/buildsupport/playdate_game.cmake)

