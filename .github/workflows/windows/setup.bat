set OPENSSL_VERSION=3.2.0
set PERL_VERSION=5.38.2.2
set NASM_VERSION=2.16.01
set PATH=%PATH%;%cd%\perl\bin;%cd%\nasm-%NASM_VERSION%

curl --output perl.zip -L -X GET "https://github.com/StrawberryPerl/Perl-Dist-Strawberry/releases/download/SP_53822_64bit/strawberry-perl-%PERL_VERSION%-64bit-portable.zip"
curl -X GET "https://www.openssl.org/source/openssl-%OPENSSL_VERSION%.tar.gz" --output openssl-%OPENSSL_VERSION%.tar.gz
curl --output nasm.zip -L -X GET "https://www.nasm.us/pub/nasm/releasebuilds/2.16.01/win64/nasm-%NASM_VERSION%-win64.zip"

tar xf openssl-%OPENSSL_VERSION%.tar.gz
tar xf perl.zip
tar xf nasm.zip

dir "C:\Program Files"
dir "C:\Program Files\Microsoft Visual Studio"
"C:\Program Files\Microsoft Visual Studio\2022\Community\VC\Auxiliary\Build\vcvarsall.bat" x86_amd64

cd openssl-%OPENSSL_VERSION%
nmake install

if %errorlevel%!=0 (
perl Configure VC-WIN64A
set CL=/MP
nmake
nmake install
)