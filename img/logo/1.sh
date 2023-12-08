#!/bin/bash
i=1
for f in *.jpg; do
  mv -- "$f" "$i.jpg"
  echo "Renamed $f to $i.jpg"
  i=$((i+1))
done
