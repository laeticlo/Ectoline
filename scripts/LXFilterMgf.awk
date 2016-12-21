#
# $Id: LXFilterMgf.awk 1386 2011-08-23 14:04:58Z viari $
#
# filter mgf files to keep n highest peaks per spectrum
#
# usage: awk [-v KEEP=<n>] -f LXFilterMgf.awk mgfFile
#

# --------------------
# min/max utilities
#
function min(a, b) {
  return (a < b ? a : b)
}

function max(a, b) {
  return (a > b ? a : b)
}

# --------------------
# sort floating values of arrayIn to ArrayOut
# on output arrayOut[i:1->n] = ith value
#
function sortFloat(arrayIn, arrayOut, _local_, tmpArray, srtArray, key, i, n) {
  delete tmpArray
  n = 0
  for (key in arrayIn) {
    value = sprintf("%015.5f", arrayIn[key])
    tmpArray[value "_" key] = key
  }
  delete srtArray
  n = asorti(tmpArray, srtArray)
  for (i=1 ; i<=n ; i++)
    arrayOut[i] = tmpArray[srtArray[i]]
  return n;
}

# --------------------
# Rules
# --------------------

BEGIN {
  if (KEEP == 0) KEEP = 100
}

/^END IONS/ {

  # -----------------------
  # sort by intensities and keep best peaks
  #
  n = sortFloat(Intz, sorti)
  k = min(n, KEEP) 
  imin = n - k + 1
  delete mass
  delete intz
  for (i = imin; i <= n ; i++) {
    mass[i] = Mass[sorti[i]]
    intz[i] = Intz[sorti[i]]
  }

  # -----------------------
  # sort by masses and print
  #
  n = sortFloat(mass, sortm)
  for (i = 1; i <= n ; i++) {
    print mass[sortm[i]], intz[sortm[i]]
  }

  # -----------------------
  # reset
  #
  Npeak = 0
  delete Mass
  delete Intz
}

/^[A-Z]/ {      # passthru textual information
  print $0
  next
}

{
  NPeak++
  Mass[NPeak] = $1
  Intz[NPeak] = $2
}
