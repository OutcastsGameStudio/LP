@echo off
:menu
cls
echo ===============================
echo   GESTIONNAIRE DE COMPRESSION
echo ===============================
echo.

set "FILE=..\Content\Characters\ANGENE\Mesh\ANGENE.uasset"
set "COMPRESSED=%FILE%.gz"


echo.
echo Actions disponibles:
echo [1] Compresser
echo [2] Decompresser  
echo [3] Auto (detecter et agir)
echo [4] Quitter
echo.

set /p "choice=Votre choix (1-4): "

if "%choice%"=="1" goto compress
if "%choice%"=="2" goto decompress
if "%choice%"=="3" goto auto
if "%choice%"=="4" goto quit

echo Choix invalide.
timeout /t 2 /nobreak > nul
goto menu

:compress
cls
echo Compression en cours...
powershell -ExecutionPolicy Bypass -File "compress.ps1" -InputFile "%FILE%"
echo.
echo Appuyez sur une touche pour retourner au menu...
pause > nul
goto menu

:decompress
cls
echo Decompression en cours...
powershell -ExecutionPolicy Bypass -File "decompress.ps1" -InputFile "%COMPRESSED%"
echo.
echo Appuyez sur une touche pour retourner au menu...
pause > nul
goto menu

:auto
cls
echo Detection automatique...
if exist "%COMPRESSED%" (
    if not exist "%FILE%" (
        echo Decompression automatique...
        powershell -ExecutionPolicy Bypass -File "decompress.ps1" -InputFile "%COMPRESSED%"
    ) else (
        echo Les deux fichiers existent, choisissez manuellement.
    )
) else if exist "%FILE%" (
    echo Compression automatique...
    powershell -ExecutionPolicy Bypass -File "compress.ps1" -InputFile "%FILE%"
) else (
    echo Aucun fichier trouve.
)
echo.
echo Appuyez sur une touche pour retourner au menu...
pause > nul
goto menu

:quit
echo Au revoir!
exit /b 0