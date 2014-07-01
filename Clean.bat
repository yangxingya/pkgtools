echo off
del /s /q *.log *.orig *.user *.obj *.o *.aps *.ilk *.exp *.ncb *.opt *.plg *.pch *.idb *.bak *.ncb *.opt *.sbr *.bsc *.manifest BuildLog.htm *.res *.dep *.idc

set module_list=vcproj
for /D %%p in (%module_list%) do rd /s /q %%p\debug   >NUL 2>NUL
for /D %%p in (%module_list%) do rd /s /q %%p\release >NUL 2>NUL


