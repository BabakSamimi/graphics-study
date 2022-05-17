@echo off

SET VENDOR_PATH=%cd%\vendors
SET INCLUDES=/I %VENDOR_PATH%\GLFW\include /I %VENDOR_PATH%\glad\include

SET FLAGS=-DPERF=0 -DDEBUG=1

SET COMPILER_FLAGS=%VENDOR_PATH%\glad\src\glad.c %INCLUDES% %FLAGS% -O2 -Femain -Zi -nologo /MT

SET LINKER_FLAGS=opengl32.lib shell32.lib user32.lib kernel32.lib gdi32.lib %VENDOR_PATH%\GLFW\lib-vc2022\glfw3_mt.lib

IF NOT EXIST bin mkdir bin
pushd bin

REM del *.pdb > NUL 2> NUL
cl ..\src\*.c %COMPILER_FLAGS% /link %LINKER_FLAGS%

popd
