PATH=$PATH:~/cross-compiler/bin
for i in *.as ; do
 	sparc-as $i -o ${i/as/obj}
done