@echo off

REM configurable parameters
set FILE_PREFIX=screenshot

REM set the output folder to save the screen shots.
set OUTPUT_FOLDER=./preview/

REM the dir on the device where the screenshot will be stored temporarily
set WORKING_DIR=/sdcard/

REM create output file timestamp from date and time

REM replace "/" and ":" with "-"
set TIME_STAMP=%DATE:/=-%-%TIME::=-%

REM remove any space
set TIME_STAMP=%TIME_STAMP: =%

REM replace "." with "-"
set TIME_STAMP=%TIME_STAMP:.=-%

set FILE_NAME=%FILE_PREFIX%_%TIME_STAMP%.png
set FILE_PATH="%WORKING_DIR%%FILE_NAME%"
set LOCAL_FILE_PATH="%OUTPUT_FOLDER%%FILE_NAME%"

REM do it!
echo Source File: %FILE_PATH%
echo Output File: %LOCAL_FILE_PATH%
echo.
adb shell screencap -p %FILE_PATH%
adb pull %FILE_PATH% %LOCAL_FILE_PATH%
adb shell rm %FILE_PATH%
echo.
