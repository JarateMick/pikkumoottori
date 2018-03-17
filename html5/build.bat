@echo off

set argCount=0

set LFLAGS=
set LFLAGS=%LFLAGS% --embed-file ../build/assets@assets
set LFLAGS=%LFLAGS% -s FORCE_FILESYSTEM=1 -s FULL_ES3=1 -s TOTAL_MEMORY=1524000000
set LFLAGS=%LFLAGS% -s ALLOW_MEMORY_GROWTH=0 --preload-file ../build/assets@assets
set LFLAGS=%LFLAGS% -s EXPORTED_FUNCTIONS="['_mainf', 'ccall']"

set CFLAGS=
set CFLAGS=%CFLAGS% -std=c++11 -D__EMSCRIPTEN__=1 -DGL_GLEXT_PROTOTYPES=1 -s USE_SDL=2
set CFLAGS=%CFLAGS% 
:: set OUTPUT=-o peli.html

set INCLUDE=-I"../inc" -I"../deps/include" -I"../deps/include/freetype"
set OUTPUT= -o moottori.js

set SRC=../src/main.cpp
set DEBUG=release

for %%x in (%*) do (
   set /A argCount+=1
   REM set "argVec[!argCount!]=%%~x"
  
    if /I "%%x" == "debug" (
		echo debug
		set CFLAGS=%CFLAGS% -g4 -s ASSERTIONS=1 
		set DEBUG=debug
		REM SET DEBUG MODE
	) 
	
	:: NOT FUNCTIONAL
	if /I "%%x"=="swap" (
		echo swap
		set LFLAGS=%LFLAGS% --embed-file assets/
		set OUTPUT=-o swapBuild/peli.html
	)
	:: NOT FUNCTIONAL
	if /I "%%x"=="prener" (
		echo Emterprener activated
		set CFLAGS=%CFLAGS% -s SWAPPABLE_ASM_MODULE=1 
		set LFLAGS=%LFLAGS% -s EMTERPRETIFY_ASYNC=1 -s EMTERPRETIFY=1 -s EMTERPRETIFY_ASYNC=1 -s FINALIZE_ASM_JS=0
	)
)
echo Number of processed arguments: %argCount%

if /I "%DEBUG%"=="debug" (
	echo DEBUG MODE
	set LFLAGS=%LFLAGS% -s ASSERTIONS=2 -s DEMANGLE_SUPPORT=1
) else (
	echo RELEASE MODE
	set CFLAGS=%CFLAGS% -O3
)
echo Build Starting...

em++ %CFLAGS% %SRC% %LFLAGS% %INCLUDE% %OUTPUT%

rem  -O3 -g4 -s WASM=1 -s "BINARYEN_METHOD='native-wasm,asmjs'" 
rem -Wall -Wextra -Wall

echo Build Finished!











