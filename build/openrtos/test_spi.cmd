@echo off

set CFG_PROJECT=%~n0
set MAKEJOBS=1
rem set AUTOBUILD=1

call build.cmd

@if not defined NO_PAUSE pause
