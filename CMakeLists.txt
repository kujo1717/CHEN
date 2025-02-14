#/****************************************************************************
# Copyright (c) 2013-2014 cocos2d-x.org
# Copyright (c) 2015-2017 Chukong Technologies Inc.
#
# http://www.cocos2d-x.org
#
# Permission is hereby granted, free of charge, to any person obtaining a copy
# of this software and associated documentation files (the "Software"), to deal
# in the Software without restriction, including without limitation the rights
# to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
# copies of the Software, and to permit persons to whom the Software is
# furnished to do so, subject to the following conditions:

# The above copyright notice and this permission notice shall be included in
# all copies or substantial portions of the Software.

# THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
# IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
# FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
# AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
# LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
# OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
# THE SOFTWARE.
# ****************************************************************************/

cmake_minimum_required(VERSION 3.6)

set(APP_NAME soul_knight)

project(${APP_NAME})

if(XCODE)
    if(NOT DEFINED CMAKE_XCODE_ATTRIBUTE_IPHONEOS_DEPLOYMENT_TARGET)
        SET (CMAKE_XCODE_ATTRIBUTE_IPHONEOS_DEPLOYMENT_TARGET 8.0)
    endif()
endif()

if(NOT DEFINED BUILD_ENGINE_DONE) # to test soul_knight into root project
    set(COCOS2DX_ROOT_PATH ${CMAKE_CURRENT_SOURCE_DIR}/cocos2d)
    set(CMAKE_MODULE_PATH ${COCOS2DX_ROOT_PATH}/cmake/Modules/)

    include(CocosBuildSet)
    add_subdirectory(${COCOS2DX_ROOT_PATH}/cocos ${ENGINE_BINARY_PATH}/cocos/core)
endif()

# record sources, headers, resources...
set(GAME_SOURCE)
set(GAME_HEADER)

set(GAME_RES_FOLDER
    "${CMAKE_CURRENT_SOURCE_DIR}/Resources"
    )
if(APPLE OR WINDOWS)
    cocos_mark_multi_resources(common_res_files RES_TO "Resources" FOLDERS ${GAME_RES_FOLDER})
endif()

# add cross-platforms source files and header files 
list(APPEND GAME_SOURCE
     Classes/AppDelegate.cpp


Classes/Entity/Entity.cpp
Classes/Entity/Player/Player.cpp
Classes/Entity/Player/PlayerMove.cpp
Classes/Entity/Player/PlayerAttribute.cpp
Classes/Entity/Gate/Gate.cpp
Classes/Entity/Monster/Monster.cpp
Classes/Entity/Monster/CloseMonster/CloseMonster_1.cpp
Classes/Entity/Monster/CloseMonster/CloseMonster_2.cpp
Classes/Entity/Monster/CloseMonster/CloseMonster_3.cpp
Classes/Entity/Monster/CloseMonster/CloseMonster_4.cpp
Classes/Entity/Monster/MonsterTFSM/MonsterTFSM.cpp
Classes/Entity/Monster/MonsterTFSM/State_M/MonsterStates.cpp
Classes/Entity/Monster/MonsterTFSM/State_M/State_M.cpp
Classes/Entity/Weapon/Pistol.cpp
Classes/Entity/Weapon/Sword.cpp
Classes/Entity/Weapon/Weapon.cpp
Classes/Entity/Weapon/Ammo/Ammo.cpp
Classes/Entity/Weapon/Ammo/MonsterPistolAmmo.cpp
Classes/Entity/Weapon/Ammo/PistolAmmo.cpp
Classes/Entity/Weapon/Ammo/SwordAmmo.cpp
Classes/music/music.cpp
Classes/PlayerTFSM/PlayerTFSM.cpp
Classes/PlayerTFSM/State/PlayerStates.cpp
Classes/PlayerTFSM/State/State.cpp
Classes/Scene/NormalScene.cpp
Classes/Scene/SafeScene.cpp
Classes/Scene/StartScene.cpp
Classes/Scene/BattleScene.cpp
Classes/Map/BattleMap.cpp



Classes/json/json_reader.cpp
Classes/json/json_value.cpp
Classes/json/json_writer.cpp



Classes/json/sconscript
     )
list(APPEND GAME_HEADER
     Classes/AppDelegate.h


Classes/Entity/Entity.h
Classes/Entity/Player/Player.h
Classes/Entity/Player/PlayerMove.h
Classes/Entity/Player/PlayerAttribute.h
Classes/Entity/Gate/Gate.h
Classes/Entity/Monster/Monster.h
Classes/Entity/Monster/CloseMonster/CloseMonster_1.h
Classes/Entity/Monster/CloseMonster/CloseMonster_2.h
Classes/Entity/Monster/CloseMonster/CloseMonster_3.h
Classes/Entity/Monster/CloseMonster/CloseMonster_4.h
Classes/Entity/Monster/MonsterTFSM/MonsterTFSM.h
Classes/Entity/Monster/MonsterTFSM/State_M/MonsterStates.h
Classes/Entity/Monster/MonsterTFSM/State_M/State_M.h
Classes/Entity/Weapon/Pistol.h
Classes/Entity/Weapon/Sword.h
Classes/Entity/Weapon/Weapon.h
Classes/Entity/Weapon/Ammo/Ammo.h
Classes/Entity/Weapon/Ammo/MonsterPistolAmmo.h
Classes/Entity/Weapon/Ammo/PistolAmmo.h
Classes/Entity/Weapon/Ammo/SwordAmmo.h
Classes/music/music.h
Classes/PlayerTFSM/PlayerTFSM.h
Classes/PlayerTFSM/State/PlayerStates.h
Classes/PlayerTFSM/State/State.h
Classes/Scene/NormalScene.h
Classes/Scene/SafeScene.h
Classes/Scene/StartScene.h
Classes/Scene/BattleScene.h
Classes/Map/BattleMap.h


Classes/json/autolink.h
Classes/json/config.h
Classes/json/features.h
Classes/json/forwards.h
Classes/json/json.h
Classes/json/json_batchallocator.h
Classes/json/reader.h
Classes/json/value.h
Classes/json/writer.h
Classes/json/json_internalarray.inl
Classes/json/json_internalmap.inl
Classes/json/json_valueiterator.inl
     )

if(ANDROID)
    # change APP_NAME to the share library name for Android, it's value depend on AndroidManifest.xml
    set(APP_NAME MyGame)
    list(APPEND GAME_SOURCE
         proj.android/app/jni/hellocpp/main.cpp
         )
elseif(LINUX)
    list(APPEND GAME_SOURCE
         proj.linux/main.cpp
         )
elseif(WINDOWS)
    list(APPEND GAME_HEADER
         proj.win32/main.h
         proj.win32/resource.h
         )
    list(APPEND GAME_SOURCE
         proj.win32/main.cpp
         proj.win32/game.rc
         ${common_res_files}
         )
elseif(APPLE)
    if(IOS)
        list(APPEND GAME_HEADER
             proj.ios_mac/ios/AppController.h
             proj.ios_mac/ios/RootViewController.h
             )
        set(APP_UI_RES
            proj.ios_mac/ios/LaunchScreen.storyboard
            proj.ios_mac/ios/LaunchScreenBackground.png
            proj.ios_mac/ios/Images.xcassets
            )
        list(APPEND GAME_SOURCE
             proj.ios_mac/ios/main.m
             proj.ios_mac/ios/AppController.mm
             proj.ios_mac/ios/RootViewController.mm
             proj.ios_mac/ios/Prefix.pch
             ${APP_UI_RES}
             )
    elseif(MACOSX)
        set(APP_UI_RES
            proj.ios_mac/mac/Icon.icns
            proj.ios_mac/mac/Info.plist
            )
        list(APPEND GAME_SOURCE
             proj.ios_mac/mac/main.cpp
             proj.ios_mac/mac/Prefix.pch
             ${APP_UI_RES}
             )
    endif()
    list(APPEND GAME_SOURCE ${common_res_files})
endif()

# mark app complie info and libs info
set(all_code_files
    ${GAME_HEADER}
    ${GAME_SOURCE}
    )
if(NOT ANDROID)
    add_executable(${APP_NAME} ${all_code_files})
else()
    add_library(${APP_NAME} SHARED ${all_code_files})
    add_subdirectory(${COCOS2DX_ROOT_PATH}/cocos/platform/android ${ENGINE_BINARY_PATH}/cocos/platform)
    target_link_libraries(${APP_NAME} -Wl,--whole-archive cpp_android_spec -Wl,--no-whole-archive)
endif()

target_link_libraries(${APP_NAME} cocos2d)
target_include_directories(${APP_NAME}
        PRIVATE Classes
        PRIVATE ${COCOS2DX_ROOT_PATH}/cocos/audio/include/
)

# mark app resources
setup_cocos_app_config(${APP_NAME})
if(APPLE)
    set_target_properties(${APP_NAME} PROPERTIES RESOURCE "${APP_UI_RES}")

    if(MACOSX)
        set_xcode_property(${APP_NAME} INFOPLIST_FILE "${CMAKE_CURRENT_SOURCE_DIR}/proj.ios_mac/mac/Info.plist")
    elseif(IOS)
        set_xcode_property(${APP_NAME} INFOPLIST_FILE "${CMAKE_CURRENT_SOURCE_DIR}/proj.ios_mac/ios/Info.plist")
        set_xcode_property(${APP_NAME} ASSETCATALOG_COMPILER_APPICON_NAME "AppIcon")
    endif()

    # For code-signing, set the DEVELOPMENT_TEAM:
    #set_xcode_property(${APP_NAME} DEVELOPMENT_TEAM "GRLXXXX2K9")
elseif(WINDOWS)
    cocos_copy_target_dll(${APP_NAME})
endif()

if(LINUX OR WINDOWS)
    cocos_get_resource_path(APP_RES_DIR ${APP_NAME})
    cocos_copy_target_res(${APP_NAME} LINK_TO ${APP_RES_DIR} FOLDERS ${GAME_RES_FOLDER})
endif()
