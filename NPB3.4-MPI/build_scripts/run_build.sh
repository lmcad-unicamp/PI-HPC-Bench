#!/bin/bash

cd ..
mkdir bin
for p in cg ep is mg ft bt sp lu; do
  for c in A; do
    make $p CLASS=$c
  done
done
