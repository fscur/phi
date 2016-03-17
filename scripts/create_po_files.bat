:start
@echo OFF

cd ..
mkdir locale
if not %errorlevel%==0 (
    cd locale
    del /F /Q demon.pot
    cd ..
)
cd src

set xget=xgettext -d=demon -k_ -o ../locale/demon.pot
for /R %%f in (.\*) do call :concat %%f
%xget%

cd ..
cd locale
mkdir pt_BR

if %errorlevel%==0 (
    echo f | xcopy /f /y demon.pot .\pt_BR\demon.po
    cd pt_BR
    call demon.po
    goto :end
)

cd pt_BR
msgmerge -o demon.po demon.po ..\demon.pot
call demon.po

goto :end

:concat
set xget=%xget% %1
goto :eof

:end    

REM cd ../../scripts

REM set choice=
REM set /P choice=
REM if "%choice%"=="" goto start