#
# $Id: LXLocateHit.awk 1386 2011-08-23 14:04:58Z viari $
#
# locate cluster hits on features
#
# usage: awk -v FEAT=feat_file -f LXLocateHit.awk clust_file
#

# --------------------
# utilities
# --------------------

# --------------------
# print error message on stderr
#

function error(msg) {
    print "# error " msg >> "/dev/stderr"
    exit(1)
}

# --------------------
# get array size (awk bug workaround)
#

function mysize(a, _local_, e, n) {
  n = 0
  for (e in a)
    n++
  return(n)
}

# --------------------
# copy array a -> b
#

function mycopy(a, b, _local_, e, n) {
  delete b
  n = 0
  for (e in a) {
    b[e] = a[e]
    n++
  }
  return(n)
}

# --------------------
# add value 'v' for key 'k' in set 'a'
#

function myadd(a, k, v) {
  if (a[k] == "")
    a[k] = v
  else if (! (v ~ "^(" a[k] ")$"))
    a[k] = a[k] "|" v
}

# --------------------
# sort array a by lexicographic values
# and handling ties conservatively
#

function mysort(a, n, _local_, i, b) {
  for (i = 1 ; i <= n ; i++) 
    a[i] = a[i] "@" i
  asort(a)
  for (i = 1 ; i <= n ; i++)
    a[i] = b[split(a[i], b, "@")]
  return(n)
}

# --------------------
# internal dichotomic search for 'mysearch'
#

function mydicho(x, a, s, n, _local_, cur, low, hig, val) {
  low = 1
  hig = n
  while ((hig-low) > 1) {
    cur = int((low+hig)/2)
    val = a[s[cur]]+0
    if (val == x) break
    if (val < x)
      low = cur
    else
      hig = cur
  }
  return(cur)
}

# --------------------
# dichotomic search contig(s)
# spanning range [xfrom, xto]
#

function mysearch(xfrom, xto, xcontig, indx, from, to, contig, n, resin, resou, 
                  _local_, i, kin, kou, i0, i1, ifrom, ito) {
   delete resin
   delete resou
   i0 = i1 = mydicho(xfrom, from, indx, n)
   while ((i0 >= 1) && ((to[indx[i0]]+0 >= xfrom) ||\
                        (from[indx[i0]]+MAX_SPAN >= xfrom))) {
     i0--
   }
   if (i0 < 1) i0 = 1
   while ((i1 <= n) && (from[indx[i1]]+0 <= xfrom)) {
     i1++
   }
   if (i1 > n) i1 = n
   kin = kou = 0
   for (i = i0 ; i <= i1 ; i++) {
     if (contig[indx[i]] != xcontig)
       continue
     ifrom = from[indx[i]]+0
     ito = to[indx[i]]+0
     if ((ifrom <= xfrom) && (ito >= xto))
       resin[++kin] = i
     else if ((xfrom <= ito) && (xto >= ifrom))
       resou[++kou] = i
   }
   return(kin+kou)
}

# --------------------
# RULES
# --------------------

BEGIN {
  MAXSPAN = 0
  if (FEAT == "")
    error("need -v FEAT=feat_file")
  getline < FEAT
  if (length($0) == 0)
    error("cannot open/read feature file : " FEAT)

  print "# Reading CDS from " FEAT >> "/dev/stderr"
  
  while (getline < FEAT) {
  
    if ($1 != "CDS") continue
    
    contig = $2
    from   = sprintf("%9.9i", $3)
    to     = sprintf("%9.9i", $4)
    prot   = $8
    key    = contig "@" from "@" to
    
    if (PROT_KEY[key] != 0) {  # already registered
      myadd(PROT_KEY, key, prot)
      continue
    }
    
    NBEXON++
    KEY_EXON[NBEXON]    = key
    CONTIG_EXON[NBEXON] = contig
    FROM_EXON[NBEXON]   = from
    TO_EXON[NBEXON]     = to
    PROT_KEY[key]       = prot
    if (MAX_SPAN < to - from + 1) MAX_SPAN = to - from + 1
  }
  close(FEAT)
  
  print "# Nb CDS read : " NBEXON " MAXSPAN : " MAXSPAN >> "/dev/stderr"
  mycopy(FROM_EXON, SORT_FROM)
  mysort(SORT_FROM, NBEXON)
}

# --------------------
# start of Cluster
#

/^CLUSTER/ {
  print "# " $0 >> "/dev/stderr"
  print "CLB", $0
  state = 1
  seq   = 0
  next
}

# --------------------
# end of Cluster
#

/^%EndOfCluster/ {
  print "CLE", $0
  state = 0                 # init state
  next
}

# --------------------
# state = 1 : sequence header
#

(state == 1) {
  if (seq++ == 0) {
    $3 = 0
    print "SEQ", $0
  }
  contig = $1
  state = 2
  next
}

# --------------------
# state = 2 : pst header
#

(state == 2) {
  print "PST", $0  
  state = 3
  next
}

# --------------------
# state = 3 : hit header
#

(state == 3) && (! /^F|P /) {
  error("invalid input file format at : " $0)
}


(state == 3) {
  print "HIT", $0
  from = $2
  to = $3

  mysearch(from, to, contig, SORT_FROM, FROM_EXON, TO_EXON, 
                             CONTIG_EXON, NBEXON, resin, resou)
  nin = mysize(resin)
  nou = mysize(resou)
  print "HIN", nin, nou
  for (i = 1 ; i <= nin ; i++) {
      j = SORT_FROM[resin[i]]
      print "LOC IN", KEY_EXON[j], PROT_KEY[KEY_EXON[j]]
  }
  for (i = 1 ; i <= nou ; i++) {
      j = SORT_FROM[resou[i]]
      print "LOC OUT", KEY_EXON[j], PROT_KEY[KEY_EXON[j]]
  }

  state = 1         # back to sequence header
  next
}
