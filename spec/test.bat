@echo off
cmd /c moon .\tools\lar.moon -b -s .\spec\testcases testcases.lar
cmd /c busted
