@echo off

rem currently, Android NDK r19c has been used with the standalone toolchain generated with:
rem make_standalone_toolchain.py --arch arm --api 16 --install-dir d:\android-standalone-32
rem the script make_standalone_toolchain.py is in the NDK under: build\tools

set "make_path_32=D:\android-standalone-32\bin"
set "make_32=%make_path_32%\make"
set "OLD_PATH=%PATH%"
set "PATH=%make_path_32%;%OLD_PATH%"

echo Generating DanaSah 32 bit for ARM-Android.

cmd /c "%make_32%"

pause