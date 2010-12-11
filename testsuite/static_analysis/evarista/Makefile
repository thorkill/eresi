# Evarista testsuite 2008 -- The ERESI project
# Julien Vanegue

SRC	= 1.simple_appel_de_methode.cpp                         \
	  2.simple_utilisation_de_champ_public.cpp              \
	  3.simple_utilisation_de_pointeur.cpp    		\
          4.simple_utilisation_de_reference.cpp			\
	  5.simple_interprocedural.cpp				\
	  6.utilisation_pointeur_interprocedural.cpp		\
	  7.interprocedural_reference.cpp			\
	  8.interprocedural_reference_2.cpp			\
	  9.heritage_multiple_avec_appel_de_methodes_filles.cpp \
	  10.modification_de_champ_dans_une_methode.cpp         \
	  11.appel_force_de_methode_mere_surchargee.cpp         \
	  12.heritage_simple_avec_appel_de_methode.cpp          \
	  13.heritage_simple_avec_appel_methode_de_classe_mere.cpp \
	  14.heritage_simple_avec_appel_methode_virtuelle.cpp      \
	  15.heritage_simple_avec_parent_field_modif.cpp           \
	  16.signature_new_delete.cpp                              \
	  17.template_simple.cpp                                   \
	  18.signature_stdio.cpp                                   \
	  19.signature_vector.cpp                                  \
	  20.signature_list.cpp                                    \
	  21.signature_iterator.cpp				   \
	  22.signature_map.cpp					   \
	  23.heritage_de_profondeur_2_avec_appel_methodes_filles.cpp \
	  24.heritage_de_profondeur_2_avec_appel_methodes_parents.cpp \
	  25.heritage_de_profondeur_2_avec_modifications_des_champs_public.cpp \
	  26.heritage_multiple_avec_appel_de_methodes_parentes.cpp \
	  27.heritage_de_classe_template.cpp			   \
	  28.interprocedural_loop_array_copy.cpp		   \
	  29.loop_3conds_with_dowhile.cpp			   \
	  30.compounds_with_gotos_and_loop.cpp			   \
	  31.simple_types_recovery.cpp				   \
	  32.n-dimentional-objects-arrays.cpp			   \
	  33.interprocedural_struct_access.cpp			   \
	  34.interprocedural_heap_corruption_static.c

OBJ	 = $(SRC:.cpp=.o)
CC	 = g++
LD	 = g++
CXX	 = g++
CFLAGS	 = 
CXXFLAGS = 
LDFLAGS  = 

all	: ${OBJ}

	for file in ${OBJ}; 			  	   	    \
	    do 						   	    \
		echo "Linking Evarista test $$file";	   	    \
	 	${LD} ${LDFLAGS} $$file -o `echo $$file | cut -f1 -d'.'`.`uname -m` || break;  \
	    done;

clean	:
	rm -fr *.o *~

fclean	: clean
	rm -fr *.`uname -m`
