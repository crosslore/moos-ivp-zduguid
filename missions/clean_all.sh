#!/bin/bash 

for folder in *; do
   if [ -d $folder ]; then
      echo -n cd $folder
      cd $folder; 
      if [ -f clean.sh ]; then
	  echo "  [Cleaning]"
	  ./clean.sh
      fi
      echo ""
      cd .. 
   fi
done
