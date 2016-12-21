#
# $Id: LXQualifyCluster.awk 1941 2013-07-25 15:47:42Z viari $
#
# convert gff3 format to tabulated format
#
# usage: awk -v FEAT=feat_file -f LXQualifyCluster.awk clust_file
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
#     else if (((xfrom >= ifrom) && (xfrom <= ito)) ||\
#              ((xto   >= ifrom) && (xto   <= ito)) ||\
#              ((xfrom <= ifrom) && (xto   >= ito)))
     else if ((xfrom <= ito) && (xto >= ifrom))
       resou[++kou] = i
   }
   return(kin+kou)
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
# count number of element in set 's'
#

function mycount(s, _local_, a) {
  return (split(s, a, "\\|"))
}

# --------------------
# wrap all elements in 'a' into string
#

function mywrap(a, _local_, e, s) {
  s = ""
  for (e in a)
    s = (s != "" ? s "@" : "") "" e "@" a[e]
  return(s)
}

# --------------------
# unwrap string 's' to array a
#

function myunwrap(s, a, _local_, i, n, b) {
  n = split(s, b, "@")
  for (i = 1 ; i <= n ; i += 2)
    a[b[i]] = b[i+1]
  return(n/2)
}

# --------------------
# RULES
# --------------------

BEGIN {
  if (FEAT == "")
    error("need -v FEAT=feat_file")
  getline < FEAT
  if (length($0) == 0)
    error("cannot open/read feature file : " FEAT)

  print "# Reading CDS from " FEAT >> "/dev/stderr"
  
  while (getline < FEAT) {
    if ($1 != "CDS") continue
    contig = $2
    from = sprintf("%9.9i", $3)
    to = sprintf("%9.9i", $4)
    prot = $8
    key = contig "@" from "@" to
    myadd(PROT_EXON, prot, key) # ??
    if (PROT_KEY[key] != 0) {
      myadd(PROT_KEY, key, prot)
      continue
    }
    NBEXON++
    KEY_EXON[NBEXON] = key
    CONTIG_EXON[NBEXON] = contig
    FROM_EXON[NBEXON] = from
    TO_EXON[NBEXON] = to
    if (MAX_SPAN < to - from + 1) MAX_SPAN = to - from + 1
    PROT_KEY[key] = prot
  }
  close(FEAT)
  print "# Nb CDS read : " NBEXON >> "/dev/stderr" 
  mycopy(FROM_EXON, SORT_FROM)
  mysort(SORT_FROM, NBEXON)
  printf("#clusno contig strand from to totspec typspec tothit hitin hitcross ")
  printf("hitout totpep totprot prot totclus protcds cdsin cdscross status")
  print ""
}

# --------------------
# start of Cluster
#

/^CLUSTER/ {
  RES_NBCLUST++
  state = 1
  clusno = $2
  clushit = $3
  cluspep = $5
  RES_CLUSNO[RES_NBCLUST] = clusno
  RES_NBHIT[clusno] = clushit
  RES_NBPEP[clusno] = cluspep
  print "# " $0 >> "/dev/stderr"
  next
}

# --------------------
# end of Cluster
#

/^%EndOfCluster/ {
  na = split(RES_SPECTRA[clusno], a, "\\|")
  for (i = 1 ; i <= na ; i++)
    ALL_SPECTRA[a[i]]++
  na = split(RES_PROT[clusno], a, "\\|")
  for (i = 1 ; i <= na ; i++)
    ALL_PROT[a[i]]++
  state = 0                 # init state
  next
}

# --------------------
# state = 1 : sequence header
#

(state == 1) {
  contig = $1
  RES_CONTIG[clusno] = contig
  RES_STRAND[clusno] = $2
  state = 2
  next
}

# --------------------
# state = 2 : spectrum header
#

(state == 2) {
  split($1, a, "\\.")
  spectrum = a[1]
  myadd(RES_SPECTRA, clusno, spectrum)
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
  from = $2
  to = $3
  if ((RES_FROM[clusno]==0) || (from < RES_FROM[clusno]))
    RES_FROM[clusno] = from
  if ((RES_TO[clusno]==0) || (to > RES_TO[clusno]))
    RES_TO[clusno] = to
    
  nn = mysearch(from, to, contig, SORT_FROM, FROM_EXON, TO_EXON, 
                                  CONTIG_EXON, NBEXON, resin, resou)
  n = mysize(resin)
  if (n > 0) {
    for (i = 1 ; i <= n ; i++) {
      j = SORT_FROM[resin[i]]
      myadd(RES_CDSIN, clusno, KEY_EXON[j])
      myadd(RES_PROT, clusno, PROT_KEY[KEY_EXON[j]])
    }
    RES_NBHITIN[clusno]++
  }
  else {
    n = mysize(resou)
    if (n > 0) {
      for (i = 1 ; i <= n ; i++) {
        j = SORT_FROM[resou[i]]
        myadd(RES_CDSCROSS, clusno, KEY_EXON[j])
        myadd(RES_PROT, clusno, PROT_KEY[KEY_EXON[j]])
      }
      RES_NBHITCROSS[clusno]++
    }
    else {
      RES_NBHITOUT[clusno]++
    }
  }

  state = 1         # back to sequence header
  next
}

# --------------------
# END : time to print
#

END {
   for (i = 1 ; i <= RES_NBCLUST ; i++) {
     clusno = RES_CLUSNO[i]
     nprt = split(RES_PROT[clusno], prt, "\\|")
     ncdsin = split(RES_CDSIN[clusno], cdsin, "\\|")
     ncdscross = split(RES_CDSCROSS[clusno], cdscross, "\\|")
     nspectra = split(RES_SPECTRA[clusno], spectra, "\\|")
     ntypic = 0
     for (j = 1 ; j <= nspectra ; j++) {
        if (ALL_SPECTRA[spectra[j]] <= 1)
          ntypic++
     }

     info = clusno " " RES_CONTIG[clusno] " " RES_STRAND[clusno]
     info = info " " RES_FROM[clusno] " " RES_TO[clusno]
     info = info " " nspectra " " ntypic
     info = info " " RES_NBHIT[clusno]+0 " " RES_NBHITIN[clusno]+0
     info = info " " RES_NBHITCROSS[clusno]+0 " " RES_NBHITOUT[clusno]+0
     info = info " " RES_NBPEP[clusno]+0 " " nprt
     
     for (j = 1 ; j <= nprt ; j++) {
     
       prot = prt[j]
       nexp = mycount(PROT_EXON[prot])
       nin = ncr = 0
       
       for (k = 1 ; k <= ncdsin ; k++) {
         if (cdsin[k] ~ "^" PROT_EXON[prot] "$")
            nin++
       }
       
       for (k = 1 ; k <= ncdscross ; k++) {
         if (cdscross[k] ~ "^" PROT_EXON[prot] "$")
            ncr++
       }

       statut = "ANNOTATED"
       
       if (ntypic < 2)
         statut = statut "_DUBIOUS"
       else if (RES_NBPEP[clusno] < 3)
         statut = statut "_POSSIBLE"
       else
         statut = statut "_SURE"
       
       if (RES_NBHIT[clusno] == RES_NBHITIN[clusno])
         statut = statut "_OK"
       else
         statut = statut "_CHECK"
       
       printf("%s "       , info)
       printf("%s %d "    , prot, ALL_PROT[prot])
       printf("%d %d %d " , nexp, nin, ncr)
       printf("%s"        , statut)       
       print ""
     }
     
     if (nprt == 0) {
     
       statut = "UNANNOTATED"
       
       if (ntypic < 2)
         statut = statut "_DUBIOUS_CHECK"
       else if (RES_NBPEP[clusno] < 3)
         statut = statut "_POSSIBLE_CHECK"
       else
         statut = statut "_SURE_CHECK"
     
       printf("%s NONE 0 0 0 0 %s", info, statut)
       print ""
     }
     
   }

}
