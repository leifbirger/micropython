#!/bin/bash

cd ../..

BRANCH_MPY=master

git status
git branch -v
git checkout $BRANCH_MPY
git status
git add -A
git commit -m "new release branch x"
git status

git fetch upstream
git merge upstream/$BRANCH_MPY
git status
git commit -m "new release branch x"
git status

