@echo off
:: setlocal enabledelayedexpansion
ctime -begin engineC.ctm
SET me=%~n0:

echo %me% Build starting

set CFLAGS=
::  -gcodeview -g
set LFLAGS= -incremental:no   
set LFLAGS=%LFLAGS%  -LIBPATH:"../deps/lib" -incremental:no -opt:ref 

set includes= -I"../deps/include" -I"../inc" -I"../deps"
set LIBPATH=-L"../deps/lib"
set LIBS= SDL2.lib SDL2main.lib 

set MAIN="..\src\main.cpp"

if not exist build mkdir build
pushd build

:: CL /P /C ..\src\application\game.cpp %includes%
:: PRETTY NEAT!!!

set DEBUG=debug
:: set argCount=0

set build_graphics=false
set build_game=false


for %%x in (%*) do (
	:: set /A argCount+=1
	REM set "argVec[!argCount!]=%%~x"

	if /I "%%x"=="init" (
		set /A pdb_num=1
	)
	
	if /I "%%x"=="env" (
		:: call "H:\Visual Studio\VC\vcvarsall.bat" x64
		call "%VS140COMNTOOLS%\..\..\VC\vcvarsall.bat" x64
	)
  
	if /I "%%x"=="gr" (
		echo %me% build graphics
		set build_graphics=true
	)
   
	if /I "%%x"=="g" (
		echo %me% build game
		set build_game=true
	)
	if /I "%%x"=="m" (
		echo %me% build main
		set build_main=true
	)
	
	if /I "%%x"=="r" (
		set DEBUG=release
	)
)

set /A pdb_num+=1


if /I "%DEBUG%"=="release" (
	set CFLAGS=%CFLAGS% -O3
)


if /I "%build_graphics%"=="true" (
del *.pdb > NUL 2> NUL
echo %me% "WAITING FOR PDB ..." > lock.tmp


set GRAPHICS_LIBS= opengl32.lib game_commondebug.lib %LIBS%
set GRAPHICS_SRC=..\src\graphics\graphics.c

clang  --shared %CFLAGS% %includes% %GRAPHICS_SRC% -o graphics.dll -z -incremental:no 
:: /MD -LD 
:: /link %GRAPHICS_LIBS%  %LFLAGS% %LIBPATH% -z -DEBUG:FASTLINK
del lock.tmp
)

if /I "%build_game%"=="true" (
del *.pdb > NUL 2> NUL
echo %me% "WAITING FOR PDB ..." > lock.tmp

set GAME_LIBS=
set GAME_SRC=..\src\application\game.c

clang --shared %CFLAGS% %includes% %GAME_SRC% -o game.dll -z -incremental:no 
::%GAME_LIBS% %LFLAGS% %PDB% %LIBPATH% -z -DEBUG:FASTLINK
del lock.tmp
)

:: damn its slow ~200ms
goto FOUND
set EXE=main.exe
FOR /F %%x IN ('tasklist /NH /FI "IMAGENAME eq %EXE%"') DO IF %%x == %EXE% goto FOUND
echo %me% Not running
goto FIN
:FIN
:: exe not running
:FOUND
echo %me% Running

popd
echo %me% Done!
ctime -end engineC.ctm