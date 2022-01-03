$current_dir=@((Get-Item .).FullName)

cd "C:\Program Files (x86)\Microsoft Visual Studio"

[array] $arr=@(Get-ChildItem . -Directory -Name)
$year=$arr[0]

Invoke-Expression -Command ".\$year\Community\VC\Auxiliary\Build\vcvars64.bat"

cd "$current_dir"

Invoke-WebRequest -Uri https://zlib.net/zlib1211.zip -OutFile zlib.zip
Expand-Archive zlib.zip -DestinationPath .
del .\zlib.zip

Invoke-WebRequest -Uri https://sourceforge.net/projects/gnuwin32/files/libpng/1.2.37/libpng-1.2.37-src.zip/download?use_mirror=jaist -UserAgent [Microsoft.PowerShell.Commands.PSUserAgent]::FireFox -OutFile libpng.zip
Expand-Archive libpng.zip -DestinationPath .
del .\libpng.zip

cd .\zlib-1.2.11
nmake /f .\win32\Makefile.msc
move zlib.lib ..

cd ..\src\libpng\1.2.37

mkdir zlib
copy ..\..\..\zlib-1.2.11\*.h .\zlib
copy ..\..\..\zlib.lib .\zlib

cd .\libpng-1.2.37-src
nmake -f .\scripts\makefile.vcwin32
move .\libpng.lib ..\..\..\..

cd ..\..\..\..
Remove-Item -path .\manifest\ -recurse
Remove-Item -path .\src\ -recurse
Remove-Item -path .\zlib-1.2.11\ -recurse