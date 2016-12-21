#
# $Id: LXGffToTab.awk 1386 2011-08-23 14:04:58Z viari $
#
# convert gff3 format to tabulated format
#
# usage: awk -f LXGffToTab.awk gffFile+
#

# --------------------
# Rules
# --------------------

BEGIN {
  FS = "\t"                         # gff3 separator
  UFEAT = "gene|mRNA|CDS|exon"      # original features
  DFEAT = "intron|inter"            # computed features
  
  print "#feat contig from to strand phase len gene"
}

# --------------------
# file header (or footer)
#

/^#/ {
  if (fname != FILENAME) {
    prevgene = ""
  }
  fname = FILENAME
  next
}

# --------------------
# gene feature : get name
# and continue
#

($3 == "gene") {
  split($NF, a, ";")
  split(a[2], a, "=")
  name = a[2]
  prevexon = ""
}

# --------------------
# any feature : get key
# and continue
#

{
  contig = $1
  key = $3
  from = $4
  to = $5
  strand = $7
  frame = $8
}

# --------------------
# feature key not in UFEAT : next line
#

(! (key ~ UFEAT)) {
  next
}

# --------------------
# any UFEAT feature : print result
# and continue
#

{
  len = to - from + 1
  print key, contig, from, to, strand, frame, len, name
}

# --------------------
# exon feature : compute intron
# and continue
#

(key == "exon") {
  prev = to
  if (prevexon != "") {
    key = "intron"
    to = from - 1
    from = prevexon + 1
  }
  prevexon = prev
}

# --------------------
# gene feature : compute intergenic
# and continue
#

(key == "gene") {
  prev = to
  if (prevgene != "") {
    key = "inter"
    to = from - 1
    from = prevgene + 1
  }
  prevgene = prev
}

# --------------------
# feature key not in DFEAT : next line
#

(! (key ~ DFEAT)) {
  next
}

# --------------------
# any DFEAT feature : print result
# and continue
#

{
  len = to - from + 1
  print key, contig, from, to, strand, frame, len, name
}
