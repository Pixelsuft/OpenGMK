@echo off
cl.exe dllmain.c User32.lib Kernel32.lib /link /DLL /NODEFAULTLIB /MACHINE:X86 /OUT:iwbtgr-foreground-patch.dll
