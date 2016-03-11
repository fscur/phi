@echo OFF

if not exist %1 (
goto end
)
xcopy /D /Y "%1" "%2"

:end