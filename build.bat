@echo off
chcp 65001

SET BUILD_DIR=build

if not exist %BUILD_DIR% (
    md %BUILD_DIR%
) 

cmake ./ -B %BUILD_DIR% -DQt5_DIR='C:/Qt/Qt5.12.8/5.12.8/msvc2017_64/lib/cmake/Qt5'