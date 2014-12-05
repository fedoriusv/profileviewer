@echo OFF

echo ==================SEARCHING FOR VISUAL STUDIO EXPRESS =====================

set KEY_NAME="HKEY_LOCAL_MACHINE\SOFTWARE\Microsoft\VCExpress\10.0"
set VALUE_NAME=InstallDir

set ValueName=

FOR /F "usebackq tokens=3 delims=	" %%A IN (`REG QUERY %KEY_NAME% /v %VALUE_NAME% ^| FINDSTR ^"InstallDir  REG_SZ^"`) DO (
    set ValueName=%%A
)

if defined ValueName (
    @echo.
) else (
    @echo ERROR: VISUAL STUDIO EXPRESS not found.
    set IDE_DIR="C:\Program Files (x86)\Microsoft Visual Studio 10.0"
    @echo VISUAL STUDIO EXPRESS DEFAULT PATH = %IDE_DIR%
    @echo IF DEFAULT PATH IS INCORRECT, PLEASE MODIFY config_visual_studio_express.bat
    goto :no_reg
)

pushd %ValueName%

cd ../..

SET IDE_DIR=%CD%

popd

:no_reg

@echo VISUAL STUDIO EXPRESS PATH = %IDE_DIR%

SET IDE_BIN_DIR=%IDE_DIR%\VC\bin
SET IDE_PACKAGE_DIR=%IDE_DIR%\VC\vcpackages

SET VCBUILD_EXE=%IDE_PACKAGE_DIR%\vcbuild.exe
SET VCVARS32_BAT=%IDE_BIN_DIR%\vcvars32.bat

call %IDE_DIR%\VC\vcvarsall.bat x86

