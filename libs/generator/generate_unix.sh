#!/bin/bash

read -p 'Please Enter Project Name: ' projectName
projectDir="../../../examples/alpha/$projectName"
templateName="mmEmptyExample"
templateDir="../../../examples/empty/mmEmptyExample"

# Prepare project directory
mkdir -p $projectDir
cp -r -f $templateDir/* $projectDir
cd $projectDir/

# Replace all folder names containing "templateName" with "projectName"
find . -type d -name "*$templateName*" -print0 | while read -r -d '' f; do
	mv -f "$f" "`echo "$f" | sed -e "s/$templateName/$projectName/g"`"
done

# Replace in all files any occurrence of "templateName" with "projectName"
# And then replace all file names containing "templateName" with "projectName"
find . -type f -name "*$templateName*" -print0 | while read -r -d '' f; do
	sed -i "" "s/$templateName/$projectName/g" "$f"
	mv -f "$f" "`echo "$f" | sed -e "s/$templateName/$projectName/g"`"
done
