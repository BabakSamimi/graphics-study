@echo off

SET VENDOR_PATH=%cd%\vendors

SET LIBS=opengl32.lib shell32.lib user32.lib kernel32.lib gdi32.lib %VENDOR_PATH%\GLFW\lib-vc2022\glfw3_mt.lib

SET INCLUDES=/I %VENDOR_PATH%\GLFW\include /I %VENDOR_PATH%\glad\include

IF NOT EXIST bin mkdir bin
pushd bin

REM del *.pdb > NUL 2> NUL
cl ..\src\win64_main.c %VENDOR_PATH%\glad\src\glad.c %LIBS% %INCLUDES% -O2 -Femain -Zi /MT

popd
