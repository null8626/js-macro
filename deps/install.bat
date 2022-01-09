@echo off
:: the makefiles won't work with powershell.
:: it only works on command prompt.

setlocal

cd .\zlib-1.2.11
nmake /f .\win32\Makefile.msc
move zlib.lib ..

cd ..\src\libpng\1.2.37

mkdir zlib
copy ..\..\..\zlib-1.2.11\*.h .\zlib
copy ..\..\..\zlib.lib .\zlib

cd .\libpng-1.2.37-src
nmake /f .\scripts\makefile.vcwin32
move .\libpng.lib ..\..\..\..

cd ..\..\..\..

rd /s /q manifest
rd /s /q src
rd /s /q zlib-1.2.11

endlocal