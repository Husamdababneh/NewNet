@echo off

REM Compile Here

SETLOCAL
set MODE=%1

set bin_int=.\bin-int\debug

IF NOT EXIST %bin_int% mkdir %bin_int%

set outputs=/Fp"%bin_int%\main.pch" /Fo%bin_int%\ /Fd"%bin_int%\vc142.pdb"


REM COMPILER OPTIONS START
set common_flags=-F1048576 -Zi -FC -GS- -Gs1048576 -nologo  -diagnostics:caret -std:c++17 -Wall
rem -WX
set debug_flags=-Ox 
set release_flags=-Ox 
set flags=%common_flags% %debug_flags%
REM COMPILER OPTIONS END

set debug_link_option=/DEBUG:FULL /NOLOGO /INCREMENTAL:NO /NODEFAULTLIB /STACK:0X100000,0X100000

IF /I "%1"=="release" ( set flags=%common_flags% %release_flags% ) 

set INCLUDE_PATHS=/I./src/
set source_files=.\src\main.cpp
set object_files=.\main.obj


set msvc_common_link_opts=/SUBSYSTEM:WINDOWS opengl32.lib Gdi32.lib User32.lib Kernel32.lib /ENTRY:main

REM compile main
cl /c  %INCLUDE_PATHS% %flags% .\src\main.cpp %outputs%
IF ERRORLEVEL 1 GOTO errorHandling

REM LINK 
pushd %bin_int%
link  %object_files% %msvc_common_link_opts%  /OUT:..\..\bin\first.exe %debug_link_option%
IF ERRORLEVEL 1 GOTO errorHandling
popd

:FINISH
ENDLOCAL
exit /b 0


:errorHandling
echo The build Failed
ENDLOCAL
exit /b 1




REM OLD COMPILER OPTIONS START

rem /D _UNICODE /D UNICODE 
rem /GF
rem /Zi /JMC /W3 /WX- /Od /TP /FC /Gm- /EHs /RTC1 /MTd /GS /fp:precise /Zc:wchar_t  /Zc:forScope /Zc:inline /std:c++17
rem /fsanitize=address

rem /d1reportSingleClassLayoutToken
rem Array
rem -fsanitize=address
rem  -EHs -fp:precise -Zc:inline -Zc:wchar_t -Zc:forScope -Zi
rem /d1reportSingleClassLayoutAst_
rem /d1reportSingleClassLayoutToken
rem /d1reportAllClassLayout
rem /d1reportSingleClassLayout<name>

REM Once we get rid of all c/c++ headers, we will enable this to see the layout of the struct
rem /d1reportAllClassLayout

rem -FA outputs asm file (.asm)
rem -FAc outputs asm file with code (.cod)


REM OLD COMPILER OPTIONS END
