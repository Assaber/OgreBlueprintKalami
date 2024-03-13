@echo off
chcp 65001

SET BUILD_DIR=build

if not exist %BUILD_DIR% (
    md %BUILD_DIR%
) 

cd %BUILD_DIR%
cmake ../