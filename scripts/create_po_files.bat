:start
@echo OFF

create_po_files.exe "..\src" "..\locale_catalog" "demon"

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
msgmerge -o demon.po demon.po ..\..\demon.pot
call demon.po
echo f | xcopy /f /y demon.mo ..\..\..\locale\pt_BR\LC_MESSAGES\demon.mo
cd ..\..\
goto :end

:concat
set xget=%xget% %1
goto :end

:end