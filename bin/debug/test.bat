@echo off

set curr_dir=%~dp0

%curr_dir%pkgtools -p pkg.script

pause

%curr_dir%pkgtools -e %curr_dir%test.dat %curr_dir%test

pause