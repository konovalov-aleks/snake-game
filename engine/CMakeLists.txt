cmake_minimum_required(VERSION 2.8)

include("$ENV{SBISPlatformSDK_374220}/tools/cmake_support/sbis.cmake")

sbis_create_project("engine" static_library)

sbis_add_sdk_dependency(sbis-lib300)
sbis_add_sdk_dependency(sbis-rpc300)

sbis_set_sources(
   "implementation/bonus.cpp"
   "implementation/color_generator.cpp"
   "implementation/game_room.cpp"
   "implementation/game_state.cpp"
   "implementation/snake.cpp"
   "implementation/vector2d.cpp"
)

sbis_set_headers(
   "engine/bonus.h"
   "engine/color_generator.h"
   "engine/game_room.h"
   "engine/game_state.h"
   "engine/snake.h"
   "engine/vector2d.h"
)

sbis_apply_settings()
