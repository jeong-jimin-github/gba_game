@echo off
setlocal enabledelayedexpansion

for %%F in (%*) do (
    echo Processing: %%F
    C:\devkitPro\tools\bin\grit.exe "%%F" -gb -gB16 -ftc -gu16
)

pause