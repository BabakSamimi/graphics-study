@echo off

SET ASSIMP_LIB_NAME=assimp-vc143-mt.lib

SET ROOT=%cd%

SET INCLUDES=/I %ROOT%\vendors /I %ROOT%\vendors\GLFW\include /I %ROOT%\vendors\glad\include /I %ROOT%\vendors\assimp\include

SET FLAGS=-DPERF=0 -DDEBUG=1


SET COMPILER_FLAGS=%ROOT%\vendors\glad\src\glad.c %INCLUDES% %FLAGS% -Od -Oi -Femain -Zi -nologo /MT

SET LINKER_FLAGS=opengl32.lib shell32.lib user32.lib kernel32.lib gdi32.lib %ROOT%\vendors\GLFW\lib-vc2022\glfw3_mt.lib %ROOT%\vendors\assimp\lib\%ASSIMP_LIB_NAME%

IF NOT EXIST bin mkdir bin
pushd bin

REM del *.pdb > NUL 2> NUL
cl %ROOT%\src\*.c %ROOT%\src\renderer\*.c %COMPILER_FLAGS% /link %LINKER_FLAGS%

popd
