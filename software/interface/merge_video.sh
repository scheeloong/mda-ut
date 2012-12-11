#!/bin/bash

if [ $# -lt 3 ]; then
  echo "Usage: ./merge_video.sh <video1> <video2> <output video>"
  exit 1
fi

rm -f $3

avconv -i $1 -f image2 in1%d.png
avconv -i $2 -f image2 in2%d.png

count=1

while [ 1 ]; do
  if [ ! -f in1$count.png ]; then
    break
  fi
  if [ ! -f in2$count.png ]; then
    break
  fi
  convert in1$count.png in2$count.png -append out$count.png
  count=`expr $count + 1`
done

rm -f $OUT

avconv -f image2 -i out%d.png -vcodec copy -r 24 $3

rm -f *.png
