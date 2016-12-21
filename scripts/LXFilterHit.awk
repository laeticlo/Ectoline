#
# $Id: $
#
# filter cluster hits
#
# usage: awk -f LXFilterHit.awk clust_file
#

# --------------------
# utilities
# --------------------

# --------------------
# RULES
# --------------------

BEGIN {
}

# --------------------
# start of Cluster
#

/^CLUSTER/ {
  cluster = $0
  state = 1
  next
}

# --------------------
# end of Cluster
#

/^%EndOfCluster/ {
  state = 0                 # init state
  next
}

# --------------------
# state = 1 : sequence header
#

(state == 1) {
  state = 2
  next
}

# --------------------
# state = 2 : spectrum header
#

(state == 2) {
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

  state = 1         # back to sequence header
  next
}
