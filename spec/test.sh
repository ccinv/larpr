moon ./tools/lar.moon -b -s ./spec/testcases testcases.lar
touch raw
moon ./tools/freeze.moon raw testcases container testcases.lar
busted
