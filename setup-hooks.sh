#!/usr/bin/sh
#
# File: setup-hooks.sh
# Author: Derek Huang
# Copyright: MIT License
#
# Git hooks setup script.
#
# This simply copies all the hook scripts in this directory to .git/hooks and
# marks the copies as executable with chmod +x.
#

# must run where .git directory is
if ! [ -d .git ]
then
    echo "error: must be run from repo root, i.e. where the .git directory is"
    exit 1
fi

# copy all hook scripts and ensure they are executable
for hook in $(ls tools/git)
do
    # copy + ensure executable
    cp tools/git/$hook .git/hooks && chmod +x .git/hooks/$hook
    # verify that hook is installed and executable
    if ! [ -x .git/hooks/$hook ]
    then
        echo "error: failed to install Git $hook hook"
        exit 1
    fi
    echo "installed Git $hook hook"
done
