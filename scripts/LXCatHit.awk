#
# $Id: $
#
# catenate several hit files coming from different tag files (hence spectra)
# renaming pstid in hit file(s) to avoid potential collisions
#
# note: to catenate several hit files coming from different chromo files
#       you just need to 'cat' them
#
# usage :
# awk -f LXCatHits.awk <hit_file>+
#

function getSource(_local_, src, a) {
  src = FILENAME
  gsub("^.*/", "", src)
  split(src, a, "\\.")
  return a[1]
}

BEGIN {
  CURFILE = ""
  LINENO  = 0
}

(FILENAME != CURFILE) {
  CURHEAD = getSource()
  CURFILE = FILENAME
}

/^%EndOfHits/ {
  LINENO = -1
}

{
  LINENO++
}

(LINENO == 2) {
  $0 = CURHEAD "_" $0
}

{
  print $0
}

