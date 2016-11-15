cmake_minimum_required(VERSION 2.8)

include("$ENV{SBISPlatformSDK_374220}/tools/cmake_support/sbis.cmake")

sbis_create_project("snake-game" library)

sbis_add_dependency(engine "../../engine")
sbis_add_sdk_dependency("sbis-xml300")
if(ANDROID)
    sbis_add_sdk_dependency("sbis-android-loader")
endif()
sbis_add_sdk_dependency("sbis-bl-core300")
sbis_add_sdk_dependency("sbis-bl-metadata-basic300")
sbis_add_sdk_dependency("sbis-rpc300")
sbis_add_sdk_dependency("sbis-rpc-client300")
sbis_add_sdk_dependency("sbis-communication300")
sbis_add_sdk_dependency("sbis-lib300")
sbis_add_sdk_dependency("curl")
if(ANDROID)
   sbis_add_sdk_dependency("iconv")
   sbis_add_sdk_dependency("openssl")
   sbis_use_library(log SYSTEM)
elseif(IOS)
   sbis_add_sdk_dependency(ssl)
   sbis_add_sdk_dependency(crypto)
endif()
sbis_use_library(z SYSTEM)

if(ANDROID)
   set(BRIDGES_SOURCES
       "implementation/bridges/NativeSnakeModel.cpp"
       "implementation/bridges/NativePoint.cpp"
       "implementation/bridges/NativeGame.cpp"
       "implementation/bridges/NativeWalls.cpp"
       "implementation/bridges/NativeField.cpp")
endif()

sbis_set_sources(
   "implementation/client_game_room.cpp"
   "implementation/game.cpp"
   ${BRIDGES_SOURCES}
)

sbis_set_headers(
   "implementation/bridges/NativeField.hpp"
   "implementation/bridges/NativePoint.hpp"
   "implementation/bridges/NativeGame.hpp"
   "implementation/bridges/NativeWalls.hpp"
   "implementation/bridges/NativeSnakeModel.hpp"
   "implementation/generated/snake_model.hpp"
   "implementation/generated/field.hpp"
   "implementation/generated/walls.hpp"
   "implementation/generated/game.hpp"
   "implementation/generated/point.hpp"
   "implementation/client_game_room.h"
)

sbis_include_directories_add(
   "implementation/bridges"
   "implementation/generated"
   "$ENV{DJINNI_PATH}/support-lib/jni"
)

sbis_add_pre_build_command("${CMAKE_CURRENT_LIST_DIR}/djinni/build.sh" ${SBIS_OS})

sbis_apply_settings()
