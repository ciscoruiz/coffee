if [ "$1" == "" ]
then
   echo "Usage: $0 release-number"
   exit
fi

find . -name *SCCS.cc -exec grep -Hn coffee_sccs_define_tag {} \; | while read sccs
do
   file=$(echo $sccs | cut -f 1 -d :)
   line=$(echo $sccs | cut -f 2 -d :)
   to_remove=$(echo $sccs | cut -f 2 -d ,)

   echo "$line s/$to_remove.*//" > /tmp/xx
   echo "$line s/\$/$1);" >> /tmp/xx
   echo "x" >> /tmp/xx
   ex $file < /tmp/xx
done

