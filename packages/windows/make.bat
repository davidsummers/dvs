REM Build the DVS installer.

pushd ..\..

for /f %%i in ( 'type Version.txt' ) do set DVS_VERSION=%%i

for /f %%i in ( 'git rev-parse --short HEAD' ) do set BUILD=%%i

set DVS_DIR=%CD%

set BUILD_DIR=%DVS_DIR%\build

if not exist %BUILD_DIR% (
mkdir %BUILD_DIR%
pushd %BUILD_DIR%
echo %CD%
cmake -DGIT_HASH=%BUILD% %DVS_DIR%
popd
)

pushd %BUILD_DIR%
cmake --build .

copy %DVS_DIR%\packages\windows\dvs.iss
iscc dvs.iss /Fdvs-setup-%DVS_VERSION%.%BUILD%.x86_64

popd

popd
