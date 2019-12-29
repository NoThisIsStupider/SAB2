#!/bin/bash
FILES=/home/nothisisstupider/SAB/SAB2/NewSonicThing/res/Models/Characters/ManiaSonic/*

NUMBER=0

for f in $FILES
do

if [[ "$f" == *00*.obj ]]
then
	FILENAME="${f:0:((${#f}-4))}"
	printf "${FILENAME}"
	mv $f "Dash${NUMBER}.obj"
	NUMBER=$((NUMBER + 1))
	printf "${f}"
fi
done
