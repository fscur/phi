:start
@echo OFF

cd ..
mkdir locale_catalog
if not %errorlevel%==0 (
    cd locale_catalog
    del /F /Q demon.pot
    cd ..
)
cd src

set xget=xgettext -d=demon -k_ -o ../locale_catalog/demon.pot
for /R %%f in (.\*) do call :concat %%f
%xget%

REM @echo ON

cd ..
cd locale_catalog
mkdir pt_BR
cd pt_BR
mkdir LC_MESSAGES
echo %errorlevel%
if %errorlevel%==0 (
    cd ..
    echo f | xcopy /f /y demon.pot .\pt_BR\LC_MESSAGES\demon.po
    cd pt_BR\LC_MESSAGES
    call demon.po
    echo f | xcopy /f /y demon.mo ..\..\..\locale\pt_BR\LC_MESSAGES\demon.mo
    cd ..\..\
    goto :end
)

cd ..\pt_BR\LC_MESSAGES
msgmerge -U demon.po ..\..\demon.pot
call demon.po
echo f | xcopy /f /y demon.mo ..\..\..\locale\pt_BR\LC_MESSAGES\demon.mo
cd ..\..\
goto :end

:concat
set xget=%xget% %1
goto :end

:end