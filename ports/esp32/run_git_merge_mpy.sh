#!/bin/bash

cd ../../micropython

BRANCH-MPY = master

git status
git branch -v
git checkout $BRANCH-MPY
git status
git add -A
git commit -m "new release branch x"
git status

git fetch upstream
git merge upstream/$BRANCH-MPY
git status
git commit -m "new release branch x"
git status

