#
# $Id: $
#
# convert pepclust output to gff3 format
#
# usage :
# awk [-v NUM=<integer>] [-v MINHIT=<integer>] [-v MINPEP=<integer>] \
#     [-v HEAD=0|1] [-v REDIR=0|1] -f LXClust2Gff.awk <pepclust_output>
#
# optional arguments:
#
# NUM : only output cluster NUM (dft: output all clusters)
#
# MINHIT : only output clusters with #hit >= MINHIT (dft: MINHIT=0)
# 
# MINPEP : only output clusters with #peptide >= MINPEP (dft: MINPEP=0)
#
# HEAD : output gff3 metadata header (dft: 0 no header)
#
# REDIR : output in seqid file (dft: output on stdout)
#

function getSource(_local_, src, a) {
  src = FILENAME
  gsub("^.*/", "", src)
  split(src, a, "\\.")
  return a[1]
}

function printHit(_local_, out) {

  out = REDIR ? seqid ".gff" : "/dev/stdout"
  
  print seqid "\tECTOLINE\tpsthit\t" hitfrom "\t" hitto "\t.\t" \
        ((seqstrd == "D") ? "+" : "-") "\t.\t" \
        "SPEC=" specid ";PST=" pst ";PRO=" hitpro ";PEP=" hitpep \
        ";CLU=" clusno ";SRC=" getSource() >> out
        
  if (REDIR) close(out)
}

function printCluster(_local_, out) {

  out = REDIR ? seqid ".gff" : "/dev/stdout"

  print seqid "\tECTOLINE\tpstcluster\t" clusfrom "\t" clusto "\t.\t" \
        ((seqstrd == "D") ? "+" : "-") "\t.\t" \
        "CLUSNO=" clusno ";HIT=" clushit ";PEP=" cluspep \
        "SRC=" getSource() >> out
        
  if (REDIR) close(out)
}

function min(a, b) {
  return (a < b ? a : b)
}

function max(a, b) {
  return (a > b ? a : b)
}

# --------------------
# RULES
# --------------------

BEGIN {
  state = "start"
}

# --------------------
# end of Cluster
#

/^%EndOfCluster/ {
  if (clusno != 0) printCluster();
  state = "start"
  next
}

# --------------------
# start of Cluster
#

(state == "start") && /^CLUSTER/ {
  clusno   = $2
  clushit  = $3
  cluspep  = $5
  clusfrom = 0
  clusto   = 0
  state = ((clushit >= MINHIT) && (cluspep >= MINPEP)) ? "seq" : "skip"
  next
}

# --------------------
# Sequence line
#
(state == "seq") {
  seqid   = $1
  seqstrd = $2
  seqfram = $3
  seqlen  = $4
  state = "spec"
  next
}

# --------------------
# Spectrum line
#
(state == "spec") {
  specid = $1
  pst    = $2 "[" $4 "]" $3
  state = "hit"
  next
}

# --------------------
# Hit line
#
(state == "hit") {
  hittype = $1
  hitfrom = $2
  hitto   = $3
  hitpro  = $6
  hitpep  = $7
  if (hittype == "F") {
    minpos = min(hitfrom, hitto)
    maxpos = max(hitfrom, hitto)
    clusfrom = (clusfrom == 0) ? minpos : min(clusfrom, minpos)
    clusto = (clusto == 0) ? maxpos : max(clusto, maxpos)
    printHit()
  }
  ## state = "spec"
  state = "seq" # error in doc
  next
}
