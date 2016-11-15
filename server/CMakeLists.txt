cmake_minimum_required(VERSION 2.8)

include("$ENV{SBISPlatformSDK_374220}/tools/cmake_support/sbis.cmake")

sbis_create_project("game-server" library)

sbis_add_sdk_dependency(sbis-lib300)
sbis_add_dependency(engine "../engine")

sbis_set_sources(
   "implementation/server_game_room.cpp"
)

sbis_set_headers(
   "implementation/server_game_room.h"
)

sbis_copy_binary_to("www/Модули/Game")

sbis_apply_settings()
