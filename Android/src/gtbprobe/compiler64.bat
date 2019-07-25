@echo off

rem currently, Android NDK r19c has been used with the standalone toolchain generated with:
rem make_standalone_toolchain.py --arch arm64 --api 21 --install-dir d:\android-standalone-32
rem the script make_standalone_toolchain.py is in the NDK under: build\tools

set "make_path_64=D:\android-standalone-64\bin"
set "make_64=%make_path_64%\make"
set "OLD_PATH=%PATH%"
set "PATH=%make_path_64%;%OLD_PATH%"

echo Generating DanaSah 64 bit for ARM-Android.

cmd /c "%make_64%"

pause