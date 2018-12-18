@echo off

SET OPTS=-FC -GR- -EHa- -nologo -Zi
SET CODE_HOME=%cd%
pushd build
cl %OPTS% %CODE_HOME%\win32_myraytracer.cpp -Femyraytracer.exe kernel32.lib gdi32.lib user32.lib
popd
