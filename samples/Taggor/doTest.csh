#/bin/csh -fv

set port = `../../config/guess_port`
set path = ($path ../../ports/$port/bin); rehash

# run Taggor

Taggor -a ../../scripts/data/aa_mono.ref -r 5 -s 0.5 -i 132134F_ME.pkl -o 132134F_ME.pst

# translate Chc and Chm chromosomes (both sequences are in arabido.fst)

PMTrans -i arabido.fst -o arabido.trn

# run PMMatch

PMMatch -a ../../scripts/data/aa_mono.ref -e ../../scripts/data/trypsine.ref -f arabido.trn -i 132134F_ME.pst -o 132134F_ME.hit

# run PMClust

PMClust -i 132134F_ME.hit -o 132134F_ME.cln

# test clusters

awk '/^F/ {print $6}' 132134F_ME.cln | sort | uniq > test.bak

diff test.ref test.bak

if ($status != 0) then
  echo '*** test failure ***'
  exit 1
else
  echo '+ test ok'
endif

# cleanup

\rm -r *.pst *.trn *.hit *.cln *.bak

exit 0

