for i in *.s; do
 	arm-elf-as $i -o ${i/.s/.obj}
done
