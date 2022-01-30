#!/bin/bash

cd ../../../esp-idf

BRANCH-IDF = release/v4.3

git status
git branch -v
git checkout $BRANCH-IDF
git status
git add -A
git commit -m "new release branch x"
git status

git fetch upstream
git merge upstream/$BRANCH-IDF
git status
git add -A
git commit -m "new release branch x"
git status
git submodule update --init --recursive
git status

