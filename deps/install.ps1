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

.\install.bat