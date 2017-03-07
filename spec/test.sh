./tools/lar.moon -b -s ./spec/testcases testcases.lar
touch raw
./tools/freeze.moon raw testcases container testcases.lar
busted
