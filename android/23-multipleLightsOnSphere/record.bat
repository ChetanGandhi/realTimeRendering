@echo off

REM configurable parameters
set FILE_PREFIX=recording
set FILE_EXTENSION=mp4
set TIME_LIMIT=%1

IF "%TIME_LIMIT%" == "" set TIME_LIMIT=10

echo Recording for %TIME_LIMIT% sec.
echo.

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

set FILE_NAME=%FILE_PREFIX%_%TIME_STAMP%.%FILE_EXTENSION%
set FILE_PATH="%WORKING_DIR%%FILE_NAME%"
set LOCAL_FILE_PATH="%OUTPUT_FOLDER%%FILE_NAME%"

REM do it!
echo Source File: %FILE_PATH%
echo Output File: %LOCAL_FILE_PATH%
echo.
adb shell screenrecord --time-limit %TIME_LIMIT% %FILE_PATH%
adb pull %FILE_PATH% %LOCAL_FILE_PATH%
adb shell rm %FILE_PATH%
echo.
