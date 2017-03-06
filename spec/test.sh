moon ./tools/lar.moon -b -s ./spec/testcases testcases.lar
touch container
moon ./tools/freeze.moon container testcases testcases.lar
busted
