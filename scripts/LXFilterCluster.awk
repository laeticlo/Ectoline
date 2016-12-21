#

function stackRecord(_local_) {
  _STACK[_NSTACK++] = $0
}

function stackPlay(_local_, i) {
  for (i = 0 ; i < _NSTACK ; i++)
    print _STACK[i]
}

function stackReset(_local_) {
  _NSTACK = 0
}

#

function count(a, _local_, n, e) {
  n = 0
  for (e in a) n++
  return n
}

function addElt(a, k, v) {
  if (a[k] == "")
    a[k] = v
  else if (! (v ~ "^(" a[k] ")$"))
    a[k] = a[k] "|" v
}

function countElt(s, _local_, a) {
  return (split(s, a, "\\|"))
}

BEGIN {
   if (MINHITLOC  == 0) MINHITLOC  = 5
   if (MINSPECLOC == 0) MINSPECLOC = 2
   if (MINVALLOC  == 0) MINVALLOC  = 2
   if (MINVALPEP  == 0) MINVALPEP  = 2
}

{
  stackRecord()
}

/^CLB/ {

  delete PST
  delete TAG
  delete SPE
  delete PEP
  delete LOC
  
  delete HLOC
  delete SLOC
  delete PLOC
  delete VLOC
  delete VPEP
  
  next
}

/^PST/ {
  PST[$2] = $2
  tag = int($3+0.5) "." $5 "." int($4+0.5)
  TAG[tag] = tag
  na = split($2, a, "\\.")
  spec = a[1]
  for (i = 2 ; i < na ; i++)
    spec = spec "." a[i]
  SPE[spec] = spec
  next
}

/^HIT F/ {
  pept = $7
  gsub("^.\\.", "", pept)
  gsub("\\..$", "", pept)
  PEP[pept] = pept
  pos = $3 "." $4
  LOC[pos] = pos
  HLOC[pos]++
  addElt(SLOC, pos, spec)
  addElt(PLOC, pos, pept)
  next
}

/^CLE/ {

   # get valid loc
   for (pos in LOC) {
      if ((HLOC[pos] >= MINHITLOC) && (countElt(SLOC[pos]) >= MINSPECLOC)) {
        VLOC[pos] = pos
      }
   }
   
   # valid peptides
   for (pos in VLOC) {
     VPEP[PLOC[pos]] = PLOC[pos]
   }
   
   # condition
   if ((count(VLOC) >= MINVALLOC) && (count(VPEP) >= MINVALPEP))
     stackPlay()
     
   stackReset()
   next
}
