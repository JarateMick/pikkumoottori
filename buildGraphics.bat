@echo off
ctime -begin engine.ctm

echo "build"

set CFLAGS= -Zi -nologo -EHs -Gm-  -GR-  -fp:fast -wd4311 -wd4312
rem -Ox -Oi 
set LFLAGS= -incremental:no opengl32.lib  
set LIBS=  SDL2.lib SDL2main.lib 
set ADDITIONAL= /I"../deps/include" /I"../inc" /I"../deps"

set MAIN="..\src\application\graphics.cpp"
set LIBPATH="/LIBPATH:\"../deps/lib\""

if not exist build mkdir build
pushd build

echo %cd%
del *.pdb > NUL 2> NUL

echo "WAITING FOR PDB ..." > lock.tmp
REM cl %CFLAGS% %ADDITIONAL% ..\src\game\game.cpp /MD -LD %LIBS% /link game_common.lib -LIBPATH:"../deps/lib" -incremental:no -opt:ref -PDB:game_%random%.pdb /LIBPATH:"../deps/lib" 
:: 
cl -Ox -Oi -Ob2 -Ot %CFLAGS% %ADDITIONAL% ..\src\graphics\graphics.cpp /MD -LD %LIBS% /link game_commondebug.lib -LIBPATH:"../deps/lib" -incremental:no -opt:ref -PDB:graphics_%random%.pdb /LIBPATH:"../deps/lib" 
del lock.tmp





set EXE=main.exe
FOR /F %%x IN ('tasklist /NH /FI "IMAGENAME eq %EXE%"') DO IF %%x == %EXE% goto FOUND
echo Not running
goto FIN
:FIN

REM W:\hotload\C-Hotloading-master\libs\gl3w\GL
REM platform exe
:: cl %CFLAGS% /MD %ADDITIONAL% %MAIN%  /link %LFLAGS% %LIBS% opengl32.lib %LIBPATH% /SUBSYSTEM:CONSOLE

:FOUND
echo Running

popd
echo Done!
ctime -end engine.ctm