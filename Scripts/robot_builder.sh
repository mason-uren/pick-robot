#!/bin/bash

echo "Specify the path to your elipse-workspace (ie. /Users/your-user-name/ <path/to/eclipse-workspace> ):"

read workspace

echo "Selected workspace: ${workspace}"

eclipse \
	-nosplash \
	-application org.eclipse.cdt.managedbuilder.core.headlessbuild \
	-data ~/${workspace} \
	-cleanBuild "pick-robot" \
	-cleanBuild "pick-trigger-app"

