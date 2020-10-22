cmake_minimum_required (VERSION 2.8)

PROJECT(XSPlayer)

SET(THIRD_PARTY_DIR ${CMAKE_CURRENT_SOURCE_DIR}/ThirdParty)

ADD_SUBDIRECTORY(Core)
ADD_SUBDIRECTORY(UI)
ADD_SUBDIRECTORY(IO)
ADD_SUBDIRECTORY(Decode)
ADD_SUBDIRECTORY(Render)
ADD_SUBDIRECTORY(Request)
ADD_SUBDIRECTORY(Script)


INCLUDE_DIRECTORIES(
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${THIRD_PARTY_DIR}
    ${THIRD_PARTY_DIR}/duilib
    ${THIRD_PARTY_DIR}/ffmpeg/include
    ${THIRD_PARTY_DIR}/SDL2/include
    ${THIRD_PARTY_DIR}/sqlite/include
    ${THIRD_PARTY_DIR}/python/include
    ${THIRD_PARTY_DIR}/rapidjson
)

LINK_DIRECTORIES(
    ${THIRD_PARTY_DIR}/duilib/lib
    ${THIRD_PARTY_DIR}/ffmpeg/lib
    ${THIRD_PARTY_DIR}/SDL2/lib
    ${THIRD_PARTY_DIR}/sqlite/lib
    ${THIRD_PARTY_DIR}/python/libs
)

SOURCE_GROUP("Core", FILES ${CORE_SRC})
SOURCE_GROUP("IO", FILES ${IO_SRC})
SOURCE_GROUP("UI" FILES ${UI_SOURCE})
SOURCE_GROUP("Decode" FILES ${DECODE_SRC})
SOURCE_GROUP("Render" FILES ${RENDER_SRC})
SOURCE_GROUP("Request" FILES ${REQUEST_SRC})
SOURCE_GROUP("Script" FILES ${SCRIPT_SRC})

ADD_EXECUTABLE(
            XSPlayer main.cpp Contanst.h Contanst.cpp Utils.h Utils.cpp
            ${CORE_SRC}
            ${IO_SRC}
            ${UI_SOURCE}
            ${DECODE_SRC}
            ${RENDER_SRC}
            ${REQUEST_SRC}
            ${SCRIPT_SRC}
            )

TARGET_LINK_LIBRARIES(
    XSPlayer
    duilib
    avcodec
    avdevice
    avfilter
    avformat
    avutil
    swresample
    swscale
    SDL2
    sqlite3
    python3
)

if(MSVC)

	# needed for some MSVC installations
	set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} /SAFESEH:NO")
	set(CMAKE_SHARED_LINKER_FLAGS "${CMAKE_SHARED_LINKER_FLAGS} /SAFESEH:NO")
	set(CMAKE_MODULE_LINKER_FLAGS "${CMAKE_MODULE_LINKER_FLAGS} /SAFESEH:NO")

    add_definitions(-D _CRT_SECURE_NO_WARNINGS)
   # add_definitions(-D _UNICODE)
   # add_definitions(-D UNICODE)

endif(MSVC)

SET_TARGET_PROPERTIES(XSPlayer PROPERTIES LINK_FLAGS "/SUBSYSTEM:WINDOWS")
