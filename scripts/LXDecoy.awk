#
# $Id: LXDecoy.awk 1362 2011-08-22 14:30:45Z viari $
#
# make a decoy protein database from fasta file
#
# note: a "R" is added in front of sequence names
#
# usage awk -f LXDecoy.awk <fastafile>
#

#
# reverse a string
#
function reverse(s, _local_, i, n, rs) {
  rs = "";
  n = length(s);
  for (i = 1 ; i <= n ; i++)
    rs = rs "" substr(s, n - i + 1, 1);
  return rs;
}

#
# printout fasta sequence
#
function printFasta(name, seq, _local_, i, from, len, nchunk, nrest) {
  from = 1
  len = length(seq)
  nchunk = int(len / 60)
  nrest = len % 60
  print name
  for (i = 1 ; i <= nchunk ; i++) {
    print substr(seq, from, 60)
    from += 60
  }
  if (nrest) {
    print substr(seq, from, nrest)
  }
}

#
# rules
#

/^>/ {
  if (name != "") {
    printFasta(name, reverse(seq))
  }
  name = ">R" substr($0, 2)
  seq = ""
  next
}

{
  seq = seq "" $0
}

END {
  printFasta(name, reverse(seq))
}
