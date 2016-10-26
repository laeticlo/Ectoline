#

function getSource(_local_, src, a) {
  src = FILENAME
  gsub("^.*/", "", src)
  split(src, a, "\\.")
  return a[1]
}

/^CLB/ {
  $2 = getSource()
  $3 = ++NCLUST
}

{ 
  print $0
}
