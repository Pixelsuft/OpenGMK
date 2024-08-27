@echo off
cl.exe dllmain.c User32.lib /link /DLL /MACHINE:X86 /OUT:iwbtgr-foreground-patch.dll
