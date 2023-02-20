#!/bin/bash

cd ../../../esp-idf

BRANCH_IDF=release/v4.4

git status
git branch -v
git checkout $BRANCH_IDF
git status
git add -A
git commit -m "new release branch x"
git status

git fetch upstream
git merge upstream/$BRANCH_IDF
git status
git add -A
git commit -m "new release branch x"
git status
#git submodule update --init --recursive
git submodule update  --recursive
git status

