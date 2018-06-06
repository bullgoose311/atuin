@echo off

git init

git add atuin.sln
git add ./src/*

git commit

git remote add origin https://github.com/bullgoose311/atuin.git REM add a remote named origin pointing at that URL

git pull origin master

git push origin master

REM so the steps are: init, add remote, pull, add, commit, push