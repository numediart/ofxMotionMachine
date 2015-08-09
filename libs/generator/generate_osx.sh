#!/bin/bash

# Read new project name from user input
read -p 'Please Enter Project Name: ' projectName
projectDir="../../apps/sandbox/$projectName"

templateName="mmEmptyExample" # It's mmEmptyExample
templateDir="template" # and it's just in template/

# Prepare project directory
mkdir -p $projectDir
cp -r -f $templateDir/* $projectDir

cd $projectDir/

# Replace all folder names containing "templateName" with "projectName"
find . -type d -name "*$templateName*" -print0 | while read -r -d '' f; do
	mv -f "$f" "`echo "$f" | sed -e "s/$templateName/$projectName/g"`"
done

# In all files, replace any occurrence of "templateName"
# with "projectName" and then replace all file names
# containing "templateName" with "projectName"
find . -type f -print0 | while read -r -d '' f; do
	sed -i "" "s/$templateName/$projectName/g" "$f"
	mv -f "$f" "`echo "$f" | sed -e "s/$templateName/$projectName/g"`"
done