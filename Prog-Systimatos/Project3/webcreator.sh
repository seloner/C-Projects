#!/bin/bash



directory=$1 
thefile=$2
w=$3
p=$4
site="site"
input=0
k=0
number=9999
array=();
delimeter="_"
page="/page"
html=".html"
arraysize=$(( $w*$p ))
linkflag=0
counter=0
usedarray=();
usedflag=0
htmlstart="<!DOCTYPE html> 
<html>
	<body>
		"
htmlend="	</body>
</html>"		


directory=$(  realpath $directory)
abspath=$(  realpath $thefile)
if [ -d "$directory" ]			# check $directory exists, even if it contains spaces
then
	echo "#directory ok"
else
	echo "Wrong directory"
	input=1

fi	
    
if [ -f "$thefile" ] 
then			# check if $file exists, even if it contains spaces
 	echo "#file ok"
else
	echo "Wrong file"
	input=1
fi	
    

if [ "$w" -eq "$w" ] 2>/dev/null   #If w is an integer    	
then
 	echo "#w ok"
else
 	echo "w is not an integer"
 	input=1
fi		
if [ "$p" -eq "$p" ] 2>/dev/null   #If w is an integer    	
then
 	echo "#p ok"
else
 	echo "p is not an integer"
 	input=1
fi


if [ "$input" -eq "$k" ]  #If input is ok    	
then									
	lines=$(wc -l < $thefile)
	if [ "$(ls -A $directory )" ]; then
     echo "#PURGE"
     rm -r $directory/*
	
   	 
	fi


	
 	if [ "$lines" -gt "$number" ]  #If number of lines is ok
 	then
 		echo "#Number of lines ok"
 		cd $directory						#cd  root
 		for i in `seq 0 $(($w-1))`
	        do

	        	  #$ bash -c 'RANDOM=640; echo $RANDOM $RANDOM $RANDOM'
	              mkdir  $site$i         #create site directory
	              for j in `seq 0 $(($p-1))`
	              	do
	              		  sitename=$directory"/"$site$i$page$i$delimeter$RANDOM$html  #generate array name

	              		  
			              array=("${array[@]}" "$sitename")               #push name into the array
			              
	              	done	                                             
	              



   
	       done
	       howmany=${#array[*]}
	 
	       for i in `seq 0 $(($w-1))`
	        do
	        	  
	              cd  $site$i   
	              echo "#creating website $i"      
	              for j in `seq 0 $(($p-1))`
	              	do

	              	 currensite=${array[$counter]}
	              	 counter=$(( $counter + 1))
	              	 
	              	 name=$(echo $currensite| cut -d'_' -f 2)
	            	 name="page"$i$delimeter$name
	              	 touch $name
					 echo "$htmlstart" >> $name
	              	 k=$(( $RANDOM % ($lines-2000) +1 ))	#generate k,m,f,q	
	       			 m=$(( $RANDOM % 999 +1001 ))
	       			 f=$(( $p/2+ 1 ))       			 
	       			 q=$(( $w/2+ 1 ))
	       			 htmlines=$(( ($f + $q)*$m/($f+$q) ))
	       			 echo "creating page $currensite with $htmlines lines starting at line $k"
	       			 stringtofind="$site$i"
	       			 for external in `seq 0 $(($f-1))`
	       			 do

			       			 while [  $linkflag -eq 0 ]; do           				
		             				randomindex=$(( $RANDOM % ($arraysize) )) #generate  a randomindex of the array
		             				string=${array[$randomindex]}
		  	         				times=${#usedarray[*]}		             				
		             				for t in `seq 0 $(( $times-1 ))`
		             				do
		             					usedstring=${usedarray[$t]}
		             					

		             					if echo "$string" | grep -q "$usedstring"; then
		             						usedflag=1
		             					fi	
		             				done	
		             				if [ "$usedflag" -eq 0 ]
		             				then	
			             				if echo "$string" | grep -q "$stringtofind"; then
	 											 linkflag=0
										else
												range=$(( $m/($f+$q) ))
												range=$(( $range+$k))
												
												sed  -n "$k,"$range"p;"$range"q" $abspath >> $name
	  											
	  											#linkname=$(echo $string| cut -d '_' -f 2)
	  											rest=${string#*$site}
												index=$( echo $(( ${#string} - ${#rest} - ${#site} )) )
				
												linkname=$( echo "${string:index}" )
												linkname="/"$linkname


	            	 							echo "adding link to $currensite value:$linkname"
												echo  "<a href=\"$linkname\">${linkname}_text</a> " >> $name
												
	  											linkflag=1
	  											usedarray=("${usedarray[@]}" "$string")
	  											#echo "${#usedarray[*]}"


										fi
									fi	
									usedflag=0
		             				

		        			 done
		        			 linkflag=0
		        			 
		        	done
		        	usedarray=();
		        	for internal in `seq 0 $(($f-1))`
		        	do
		        		while [  $linkflag -eq 0 ]; do           				
		             				randomindex=$(( $RANDOM % ($arraysize) )) #generate  a randomindex of the array
		             				string=${array[$randomindex]}
		             				usedarray=("${usedarray[@]}" "$currensite")
		  	         				times=${#usedarray[*]}
		  	         				for t in `seq 0 $(( $times-1 ))`
		             				do
		             					usedstring=${usedarray[$t]}
		             					

		             					if echo "$string" | grep -q "$usedstring"; then
		             						usedflag=1
		             					fi	
		             				done
		             				if [ "$usedflag" -eq 0 ]
		             				then	
			             				if echo "$string" | grep -q "$stringtofind"; then
	 											range=$(( $m/($f+$q) ))
												range=$(( $range+$k))
												
												sed  -n "$k,"$range"p;"$range"q" $abspath >> $name
	  											
	  											linkname=$(echo $string| cut -d'_' -f 2)
	            	 							linkname="/site"$i$page$i$delimeter$linkname
	            	 							echo "adding link to $currensite value:$linkname "
	            	 						
												echo  "<a href=\"$linkname\">${linkname}_text</a> " >> $name
	  											linkflag=1
	  											usedarray=("${usedarray[@]}" "$string")	 


	 											 

										else
												
	  											linkflag=0


										fi
									fi	
									usedflag=0
		             				

		        			done
		        			 linkflag=0	




		        	done
		        	usedarray=();


		        	sed  -n "$k,"$range"p;"$range"q" $abspath >> $name
	  												
		        	echo "$htmlend" >> $name
		        	

	              	 


	              done
	              	cd ..	                                             
	              



   
	       done





	       k=$(( $RANDOM % ($lines-2000) +1 ))		
	       m=$(( $RANDOM % 999 +1001 ))				
	       #echo $m
	       
	       	#for each in "${array[@]}"
			#do
  				#echo "$each"				
			#done






 	else
 		echo "#Give at least 10.000 number of lines"
 	fi			

fi

			
			
		
	       

	




