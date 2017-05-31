#!/bin/bash
n=$#
flag=0 
while [[ $flag -ne 5 ]] 
do
clear
echo  
echo
echo "				Simulation Environment"
echo "				**********************"
echo
echo
echo
echo 1. Fattree Architecture
echo 2. Butterfly Architecture
echo 3. Fattree Architecture - Busy Network
echo 4. Butterfly Architecture - Busy Network
echo 5. Exit
read flag
if [[ $flag -eq 1 ]]
then
./fattreenbc
elif [[ $flag -eq 2 ]]
then
./butterflynbc
elif [[ $flag -eq 3 ]]
then 
./fattree
elif [[ $flag -eq 4 ]]
then 
./butterfly
#else
#break
elif [[ $flag -eq 5 ]]
then
break
else
echo ""
echo "Invalid Input..."
fi
read char
done 

