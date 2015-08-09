@echo off
TITLE Generate Project
SETLOCAL ENABLEEXTENSIONS
SETLOCAL ENABLEDELAYEDEXPANSION

SET /p projectName=Please Enter Project Name:
SET "projectDir=..\..\apps\sandbox\%projectName%"
SET "templateName=mmEmptyExample"
SET "templateDir=template"
CLS
ECHO Generating project...

xcopy /e /i /y /q "%templateDir%\*" "%projectDir%"

cd "%projectDir%"
CLS
ECHO Replacing all occurrences of %templateName%...
CALL :explore
CLS
ECHO New project created in folder MotionMachine\apps\sandbox\%projectName%
PAUSE
GOTO :EOF

:explore
:: In current directory, Replace all file names containing "templateName" with "projectName"
FOR %%f IN (*) DO (
    SET "File=%%~f"
	REN "%%f" "!File:%templateName%=%projectName%!"
)
:: In current directory, Replace in all files any occurrence of "templateName" with "projectName"
FOR %%f IN (*) DO (
	set "ext=true"
	::echo %%~xf
	IF %%~xf==.suo set "ext=false"
	IF %%~xf==.sdf set "ext=false"
	IF %%~xf==.exp set "ext=false"
	IF %%~xf==.opensdf set "ext=false"
	IF %%~xf==.bsc set "ext=false"
	IF %%~xf==.pdb set "ext=false"
	IF %%~xf==.cache set "ext=false"
	::echo !ext!
	IF !ext!==true (
		FOR /F tokens^=*^ delims^=^ eol^= %%A IN ('"type "%%f""') DO (
			SET line=%%A
			IF "!line!"=="" (
				ECHO(.>>tempFile
			) ELSE (
				SET line=!line:%templateName%=%projectName%!
				ECHO(!line!>>tempFile
			)
		)
		MOVE /y tempFile "%%f"
	)	
)
:: In current directory, Replace all folder names containing "templateName" with "projectName"
FOR /D %%a IN (*) DO (
    SET "Folder=%%~a"
    REN "%%a" "!Folder:%templateName%=%projectName%!"
)
:: Loop to explore all subdirectories
FOR /D %%D IN (*) DO (
	set "fold=true"
	IF %%D==bin set "fold=false"
	IF %%D==obj set "fold=false"
	IF !fold!==true (
		CD %%D
		CALL :explore
		CD ..
	)
)
