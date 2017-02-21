@echo OFF
create_po_files.exe "..\src" "..\locale_catalog" "demon"

cd ..\locale_catalog
if NOT EXIST .\pt_BR\LC_MESSAGES (
    mkdir .\pt_BR\LC_MESSAGES
    echo f | xcopy /f /y demon.pot .\pt_BR\LC_MESSAGES\demon.po
    cd pt_BR\LC_MESSAGES
    call demon.po
    echo f | xcopy /f /y demon.mo ..\..\..\locale\pt_BR\LC_MESSAGES\demon.mo
    goto :end
)

cd .\pt_BR\LC_MESSAGES
set VAR=
for /f %%i in ('msgattrib --untranslated demon.po') do set VAR=%%i
if NOT ("%VAR%")==("") (
    msgmerge -o demon.po demon.po ..\..\demon.pot
    call demon.po
)

echo f | xcopy demon.mo ..\..\..\locale\pt_BR\LC_MESSAGES\demon.mo /D /E /C /Q /H /R /Y /K

:end
cd ..\..\