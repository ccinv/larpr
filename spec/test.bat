@echo off
cmd /c moon .\tools\lar.moon -b -s .\spec\testcases testcases.lar
copy nul raw
cmd /c moon .\tools\freeze.moon raw testcases container testcases.lar
cmd /c busted
