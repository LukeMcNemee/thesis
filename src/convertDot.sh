#!/bin/bash
for file in *.dot; do dot -Kneato -n -Tpng "$file" -o\
"$(basename $file .dot).png"; done

for file in *.png; do convert\
"$file" "$(basename $file .png).gif"; done

gifsicle --delay=1 --loop *.gif > anim.gif
