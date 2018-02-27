echo > .codedocs
grep "#" codedocs.txt | grep = | sed 's/^# //' | sed 's/ =*.//' | while read variable
do
   new_attribute=$(grep "\b$variable\b" docs/Doxyfile.in | grep "=")
   new_value=$(echo $new_attribute | cut -f 2 -d =)

   if [ "$new_value" == "" ]
   then
      continue;
   fi

   echo $new_attribute >> .codedocs
done

