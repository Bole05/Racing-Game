# This file will be configured to contain variables for CPack. These variables
# should be set in the CMake list file of the project before CPack module is
# included. The list of available CPACK_xxx variables and their associated
# documentation may be obtained using
#  cpack --help-variable-list
#
# Some variables are common to all generators (e.g. CPACK_PACKAGE_NAME)
# and some are specific to a generator
# (e.g. CPACK_NSIS_EXTRA_INSTALL_COMMANDS). The generator specific variables
# usually begin with CPACK_<GENNAME>_xxxx.


set(CPACK_BUILD_SOURCE_DIRS "C:/Users/bolew/Documents/GitHub/Racing-Game/vcpkg_installed/x86-windows/vcpkg/blds/sdl3-image/src/ease-3.2.4-f22def61fe.clean;C:/Users/bolew/Documents/GitHub/Racing-Game/vcpkg_installed/x86-windows/vcpkg/blds/sdl3-image/x86-windows-dbg")
set(CPACK_CMAKE_GENERATOR "Ninja")
set(CPACK_COMPONENTS_ALL "DEVEL;Unspecified;devel;library")
set(CPACK_COMPONENT_UNSPECIFIED_HIDDEN "TRUE")
set(CPACK_COMPONENT_UNSPECIFIED_REQUIRED "TRUE")
set(CPACK_DEFAULT_PACKAGE_DESCRIPTION_FILE "C:/Users/bolew/AppData/Local/vcpkg/downloads/tools/cmake-3.30.1-windows/cmake-3.30.1-windows-i386/share/cmake-3.30/Templates/CPack.GenericDescription.txt")
set(CPACK_DEFAULT_PACKAGE_DESCRIPTION_SUMMARY "SDL3_image built using CMake")
set(CPACK_GENERATOR "ZIP")
set(CPACK_INNOSETUP_ARCHITECTURE "x86")
set(CPACK_INSTALL_CMAKE_PROJECTS "C:/Users/bolew/Documents/GitHub/Racing-Game/vcpkg_installed/x86-windows/vcpkg/blds/sdl3-image/x86-windows-dbg;SDL3_image;ALL;/")
set(CPACK_INSTALL_PREFIX "C:/Users/bolew/Documents/GitHub/Racing-Game/vcpkg_installed/x86-windows/vcpkg/pkgs/sdl3-image_x86-windows/debug")
set(CPACK_MODULE_PATH "C:/Users/bolew/Documents/GitHub/Racing-Game/vcpkg_installed/x86-windows/vcpkg/blds/sdl3-image/src/ease-3.2.4-f22def61fe.clean/cmake")
set(CPACK_NSIS_DISPLAY_NAME "SDL3_image 3.2.4")
set(CPACK_NSIS_INSTALLER_ICON_CODE "")
set(CPACK_NSIS_INSTALLER_MUI_ICON_CODE "")
set(CPACK_NSIS_INSTALL_ROOT "$PROGRAMFILES")
set(CPACK_NSIS_PACKAGE_NAME "SDL3_image 3.2.4")
set(CPACK_NSIS_UNINSTALL_NAME "Uninstall")
set(CPACK_OUTPUT_CONFIG_FILE "C:/Users/bolew/Documents/GitHub/Racing-Game/vcpkg_installed/x86-windows/vcpkg/blds/sdl3-image/x86-windows-dbg/CPackConfig.cmake")
set(CPACK_PACKAGE_DEFAULT_LOCATION "/")
set(CPACK_PACKAGE_DESCRIPTION_FILE "C:/Users/bolew/AppData/Local/vcpkg/downloads/tools/cmake-3.30.1-windows/cmake-3.30.1-windows-i386/share/cmake-3.30/Templates/CPack.GenericDescription.txt")
set(CPACK_PACKAGE_DESCRIPTION_SUMMARY "SDL3_image built using CMake")
set(CPACK_PACKAGE_DIRECTORY "C:/Users/bolew/Documents/GitHub/Racing-Game/vcpkg_installed/x86-windows/vcpkg/blds/sdl3-image/x86-windows-dbg/dist")
set(CPACK_PACKAGE_FILE_NAME "SDL3_image-3.2.4-win32")
set(CPACK_PACKAGE_INSTALL_DIRECTORY "SDL3_image 3.2.4")
set(CPACK_PACKAGE_INSTALL_REGISTRY_KEY "SDL3_image 3.2.4")
set(CPACK_PACKAGE_NAME "SDL3_image")
set(CPACK_PACKAGE_RELOCATABLE "true")
set(CPACK_PACKAGE_VENDOR "Humanity")
set(CPACK_PACKAGE_VERSION "3.2.4")
set(CPACK_PACKAGE_VERSION_MAJOR "3")
set(CPACK_PACKAGE_VERSION_MINOR "2")
set(CPACK_PACKAGE_VERSION_PATCH "4")
set(CPACK_PROJECT_CONFIG_FILE "C:/Users/bolew/Documents/GitHub/Racing-Game/vcpkg_installed/x86-windows/vcpkg/blds/sdl3-image/x86-windows-dbg/CPackProjectConfig.cmake")
set(CPACK_RESOURCE_FILE_LICENSE "C:/Users/bolew/AppData/Local/vcpkg/downloads/tools/cmake-3.30.1-windows/cmake-3.30.1-windows-i386/share/cmake-3.30/Templates/CPack.GenericLicense.txt")
set(CPACK_RESOURCE_FILE_README "C:/Users/bolew/AppData/Local/vcpkg/downloads/tools/cmake-3.30.1-windows/cmake-3.30.1-windows-i386/share/cmake-3.30/Templates/CPack.GenericDescription.txt")
set(CPACK_RESOURCE_FILE_WELCOME "C:/Users/bolew/AppData/Local/vcpkg/downloads/tools/cmake-3.30.1-windows/cmake-3.30.1-windows-i386/share/cmake-3.30/Templates/CPack.GenericWelcome.txt")
set(CPACK_SET_DESTDIR "OFF")
set(CPACK_SOURCE_7Z "ON")
set(CPACK_SOURCE_GENERATOR "7Z;ZIP")
set(CPACK_SOURCE_OUTPUT_CONFIG_FILE "C:/Users/bolew/Documents/GitHub/Racing-Game/vcpkg_installed/x86-windows/vcpkg/blds/sdl3-image/x86-windows-dbg/CPackSourceConfig.cmake")
set(CPACK_SOURCE_PACKAGE_FILE_NAME "SDL3-3.2.4-src")
set(CPACK_SOURCE_ZIP "ON")
set(CPACK_SYSTEM_NAME "win32")
set(CPACK_THREADS "1")
set(CPACK_TOPLEVEL_TAG "win32")
set(CPACK_WIX_SIZEOF_VOID_P "4")

if(NOT CPACK_PROPERTIES_FILE)
  set(CPACK_PROPERTIES_FILE "C:/Users/bolew/Documents/GitHub/Racing-Game/vcpkg_installed/x86-windows/vcpkg/blds/sdl3-image/x86-windows-dbg/CPackProperties.cmake")
endif()

if(EXISTS ${CPACK_PROPERTIES_FILE})
  include(${CPACK_PROPERTIES_FILE})
endif()
