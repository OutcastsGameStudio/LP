@echo off
chcp 65001 >nul
setlocal enabledelayedexpansion

echo ================================================
echo    Verificateur Git/Git-LFS
echo ================================================
echo.

:: Vérification de Git
echo [1/3] Verification de Git...
git --version >nul 2>&1
if %errorlevel% equ 0 (
    for /f "tokens=*" %%i in ('git --version') do set git_version=%%i
    echo ✓ Git est installe : !git_version!
) else (
    echo ✗ Git n'est pas installe ou n'est pas dans le PATH
    echo.
    echo Veuillez installer Git depuis : https://git-scm.com/download/win
    pause
    exit /b 1
)

echo.

:: Vérification de Git LFS
echo [2/3] Verification de Git LFS...
git lfs version >nul 2>&1
if %errorlevel% equ 0 (
    for /f "tokens=*" %%i in ('git lfs version') do set lfs_version=%%i
    echo ✓ Git LFS est installe : !lfs_version!
) else (
    echo ✗ Git LFS n'est pas installe
    echo.
    echo Veuillez installer Git LFS depuis : https://git-lfs.github.io/
    pause
    exit /b 1
)

echo.

:: Exécution de git lfs install
echo [3/3] Execution de git lfs install...
git lfs install
if %errorlevel% equ 0 (
    echo ✓ Git LFS configure avec succes
) else (
    echo ✗ Erreur lors de l'execution de git lfs install
    pause
    exit /b 1
)

echo.
echo ================================================
echo Configuration terminee avec succes !
echo ================================================
echo.
pause