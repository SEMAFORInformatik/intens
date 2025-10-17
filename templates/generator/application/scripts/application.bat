@echo off

set debug=1

set TOP_DIR=%cd%
set USER_DIR=%USERPROFILE%\${application}
set LOG_DIR=%USER_DIR%\log
set STARTUP_LOG=%LOG_DIR%\${application}.startup

:: mkdir creates directories recursively
IF NOT EXIST %LOG_DIR%\NUL mkdir %LOG_DIR%

set DB_HOST=localhost
set DB_PORT=8080
set REST_SERVICE_BASE=http://%DB_HOST%:%DB_PORT%/workbook

set API_GATEWAY_EXE=api-gateway.exe
set API_GATEWAY_HOST=localhost
set API_GATEWAY_PORT=15558
set API_GATEWAY_PROGRESS_PORT=15564
set API_GATEWAY_CONTROL_PORT=15565
set API_GATEWAY_LOGGER_PORT=15574

set INTENS_REPLY_PORT=0

set CITHEL_SOLVER_EXE=solver-win.exe
set CITHEL_SOLVER_HOST=localhost
set CITHEL_ZMQ_REQ_PORT=5555
set CITHEL_ZMQ_PULL_PORT=6000

set BITMAP_BASE_PATH=%TOP_DIR%/bitmaps
set BITMAP_PATH=%BITMAP_BASE_PATH%;%BITMAP_BASE_PATH%\icons;%BITMAP_BASE_PATH%\icons\status

set PATH=%TOP_DIR%\api-gateway;%TOP_DIR%\bin;%PATH%

if debug == 1 (
    echo "Configuration" >> %STARTUP_LOG%
    echo "PATH=%PATH%" >> %STARTUP_LOG%
    echo "TOP_DIR=%TOP_DIR%" >> %STARTUP_LOG%
)

set STYLE="${application}"

if debug == 1 (
    echo "Starting API-Gateway" >> %STARTUP_LOG%
)

start /b %API_GATEWAY_EXE% -z > %LOG_DIR%\api-gateway.log 2> %LOG_DIR%\api-gateway.err

if debug == 1 (
    echo "Starting ${applicationShortName}" >> %STARTUP_LOG%
)

intens.exe ^
    --rightIcon semafor ^
    --undo ^
    --notitle ^
    --replyPort %INTENS_REPLY_PORT% ^
    --maxlines 10000 ^
    --qtGuiStyle %STYLE% ^
    --startupImage %TOP_DIR%\bitmaps\startup ^
    --resfile %TOP_DIR%\etc\${application}.ini ^
    %%TOP_DIR%\etc\${application}.des > %LOG_DIR%\${application}.log 2> %LOG_DIR%\${application}.err
