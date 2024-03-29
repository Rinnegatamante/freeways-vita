cmake_minimum_required(VERSION 3.14)

if (NOT DEFINED ENV{VITASDK} AND NOT DEFINED CMAKE_TOOLCHAIN_FILE)
  message(FATAL_ERROR "Please define VITASDK env var to point to your SDK path "
		  "or set CMAKE_TOOLCHAIN_FILE.")
endif ()

set(CMAKE_TOOLCHAIN_FILE "$ENV{VITASDK}/share/vita.toolchain.cmake"
	CACHE PATH "Toolchain file")

project(freeways C CXX)

include("${VITASDK}/share/vita.cmake" REQUIRED)

# Vita application configuration
set(VITA_APP_NAME "Freeways")
set(VITA_TITLEID "FREEWAYS1")
set(VITA_VPKNAME "freeways")
set(VITA_VERSION "01.00")
set(VITA_MKSFOEX_FLAGS "-d ATTRIBUTE2=12")

# Optional: used for "send" and "dump" targets
set(PSVITAIP "192.168.0.198" CACHE STRING "PSVita IP (for FTP access)")
set(PARSECOREPATH "$ENV{PARSECOREPATH}" CACHE STRING "vita-parse-core main.py")

# Game-specific definitions
set(DATA_PATH "ux0:data/freeways/" CACHE STRING "Path to data (with trailing /)")
set(SO_PATH "${DATA_PATH}/libcrusty.so" CACHE STRING "Path to .so")

option(USE_SCELIBC_IO "Use SceLibcBridge for IO and some other std functions" OFF)
if (USE_SCELIBC_IO)
  add_definitions(-DUSE_SCELIBC_IO)
endif()

add_definitions(-DDATA_PATH="${DATA_PATH}" -DSO_PATH="${SO_PATH}")

# makes sincos, sincosf, etc. visible
add_definitions(-D_GNU_SOURCE -D__POSIX_VISIBLE=999999)

# Unique debug definition for loader's logging to not interfere with anything
if (CMAKE_BUILD_TYPE STREQUAL "Debug")
  add_definitions(-DDEBUG_SOLOADER)
endif()

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wl,-q,--allow-multiple-definition -O3 -g -ffast-math -mfloat-abi=softfp -Wno-deprecated -DDEBUG_SOLOADER")
set(CMAKE_CXX_FLAGS "${CMAKE_C_FLAGS} -std=gnu++20 -Wno-write-strings -Wno-psabi")

add_executable(${CMAKE_PROJECT_NAME}
			   source/main.c
			   source/dynlib.c
			   source/falso_jni_impl.c
			   source/patch.c
			   source/reimpl/ctype_patch.c
			   source/reimpl/env.c
			   source/reimpl/errno.c
			   source/reimpl/io.c
			   source/reimpl/ioctl.c
			   source/reimpl/log.c
			   source/reimpl/mem.c
			   source/reimpl/pthr.c
			   source/reimpl/sys.c
			   source/utils/dialog.c
			   source/utils/glutil.c
			   source/utils/init.c
			   source/utils/logger.c
			   source/utils/settings.c
			   source/utils/utils.c
			   source/audio.cpp
			   lib/falso_jni/FalsoJNI.c
			   lib/falso_jni/FalsoJNI_ImplBridge.c
			   lib/falso_jni/FalsoJNI_Logger.c
			   lib/falso_jni/ConvertUTF.c
			   lib/AFakeNative/polling/pseudo_eventfd.cpp
			   lib/AFakeNative/polling/pseudo_pipe.cpp
			   lib/AFakeNative/PseudoEpoll.cpp
			   lib/AFakeNative/AAssetManager.cpp
			   lib/AFakeNative/AFakeNative_Utils.cpp
			   lib/AFakeNative/ANativeWindow.cpp
			   lib/AFakeNative/ASensor.cpp
			   lib/AFakeNative/AConfiguration.cpp
			   lib/AFakeNative/AInput.cpp
			   lib/AFakeNative/ALooper.cpp
			   lib/AFakeNative/utils/controls.cpp
			   lib/AFakeNative/utils/sensors.cpp
			   lib/AFakeNative/ANativeActivity.cpp
			   lib/sha1/sha1.c
			   lib/fios/fios.c
			   lib/so_util/so_util.c)

add_subdirectory(lib/libc_bridge)
add_dependencies(${CMAKE_PROJECT_NAME} SceLibcBridge)

target_include_directories(${CMAKE_PROJECT_NAME} SYSTEM
						   PUBLIC ${CMAKE_SOURCE_DIR}/lib
						   PUBLIC ${CMAKE_SOURCE_DIR}/lib/fmod/include
						   )

target_include_directories(${CMAKE_PROJECT_NAME}
						   PUBLIC ${CMAKE_SOURCE_DIR}/source
						   )

target_link_libraries(${CMAKE_PROJECT_NAME}
					  -Wl,--whole-archive pthread -Wl,--no-whole-archive
					  stdc++
					  soloud_static
					  OpenSLES
					  openal
					  sndfile
					  vorbisfile
					  ogg
					  vorbisenc
					  vorbis
					  FLAC
					  vitaGL
					  vitashark
					  SceShaccCgExt
					  taihen_stub
					  SceShaccCg_stub
					  z
					  m
					  ScePgf_stub
					  mathneon
					  kubridge_stub
					  SceHid_stub
					  SceTouch_stub
					  SceMotion_stub
					  SceAppMgr_stub
					  SceAppUtil_stub
					  SceAudio_stub
					  SceAudioIn_stub
					  SceCommonDialog_stub
					  SceSysmodule_stub
					  ${CMAKE_BINARY_DIR}/lib/libc_bridge/stubs/libSceLibcBridge_stub.a
					  SceDisplay_stub
					  SceKernelDmacMgr_stub
					  SceFios2_stub
					  SceCtrl_stub
					  SceGxm_stub
					  ScePower_stub
					  SceTouch_stub
					  SceVshBridge_stub
					  )

vita_create_self(eboot.bin ${CMAKE_PROJECT_NAME} UNSAFE NOASLR)
vita_create_vpk(${VITA_VPKNAME}.vpk ${VITA_TITLEID} eboot.bin
				VERSION ${VITA_VERSION}
				NAME ${VITA_APP_NAME}
				FILE ${CMAKE_SOURCE_DIR}/extras/livearea/icon0.png sce_sys/icon0.png
				${CMAKE_SOURCE_DIR}/extras/livearea/startup.png sce_sys/livearea/contents/startup.png
				${CMAKE_SOURCE_DIR}/extras/livearea/bg0.png sce_sys/livearea/contents/bg0.png
				${CMAKE_SOURCE_DIR}/extras/livearea/template.xml sce_sys/livearea/contents/template.xml
				)

add_custom_target(send
				  COMMAND echo destroy | nc ${PSVITAIP} 1338
				  COMMAND echo destroy | nc ${PSVITAIP} 1338
				  COMMAND curl -T eboot.bin ftp://${PSVITAIP}:1337/ux0:/app/${VITA_TITLEID}/
				  COMMAND echo launch ${VITA_TITLEID} | nc ${PSVITAIP} 1338
				  DEPENDS ${VITA_VPKNAME}.vpk-vpk
				  )

add_custom_target(send_kvdb
				  COMMAND echo destroy | nc ${PSVITAIP} 1338
				  COMMAND curl -T eboot.bin ftp://${PSVITAIP}:1337/ux0:/app/${VITA_TITLEID}/
				  COMMAND echo launch ux0:/app/${VITA_TITLEID}/eboot.bin | nc ${PSVITAIP} 1338
				  DEPENDS ${VITA_VPKNAME}.vpk-vpk
				  )

add_custom_target(dump
				  COMMAND ${CMAKE_SOURCE_DIR}/extras/scripts/get_dump.sh ${PSVITAIP}
				  COMMAND python3 ${PARSECOREPATH} coredump ${CMAKE_PROJECT_NAME}
				  )

add_custom_target(reboot
				  COMMAND echo reboot | nc ${PSVITAIP} 1338
				  )
