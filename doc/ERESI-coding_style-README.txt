
$ID 

		------------------------------------------------------
		ERESI : The ELF Reverse Engineering Software Interface
		------------------------------------------------------
		*	    CODING STYLE & MANDATORY PRACTICES       *  
		******************************************************

Hi guys,

I compiled a few of the documentation that I wrote incrementally on
the Mailing list of the ERESI project. Those points only concerns the
coding style inside the project, and the mandatory practices to hope
getting your code working without breaking anything.

Please read those recommendations carefully before submitting any patch
or commiting something to the CVS.

Feel free to use any editors to acheive this presentation, but we strongly
recommand to use emacs-21 or a more recent version.

Here are the most important recommandations :


A/ INDENTATION

   1. The variables definition should be aligned with the function name 
   2. The indentation must be of 2 characters in the body of functions 
   3. Braces { and } should always be put at next line
   4. Never use automatic indentation unless you know what you are doing
   5. Your code must fit on 80 colums
   6. Your code shoud be compact be clear

B/ DEVELOPMENT

   1. Never use syscalls directly but use defined macros
   2. Use the profiler macros carefully ! 
   3. Your code should not generate any warnings
   4. Your code should be portable
   5. Your code shoud take advantage of the existing API
   6. Your code should be modular
   7. Your code should be formated

C/ TESTING

   1. Your code must be tested before commiting
   2. Be reactive to the crew CVS alerts

D/ DOCUMENTATION

   1. Write text files to explain the API of what you have done
   2. Comment your code in a reasonable fashion
   3. Use doxygen tags



A/ INDENTATION
--------------
--------------


   1. The variables definition should be aligned with the function name 
   --------------------------------------------------------------------

   GOOD:

   int           my_function()
   {
     int         var;
     char	 c;
     (...)

    
   BAD:

   int my_function() {
   int var;
   char c;
   (...)


   2. The indentation must be of 2 characters in the body of functions 
   -------------------------------------------------------------------

   /* Return the ELF header flags field */
   elfsh_Word      elfsh_get_flags(elfsh_Ehdr *hdr)
   {
      PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);

      if (!hdr)
        PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__,
	                 "Invalid NULL parameter", (elfsh_Word) -1);
      PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, (hdr->e_flags));
   }

   BAD:

   /* Change the section header table file offset */
   int             elfsh_set_shtoff(elfsh_Ehdr *hdr, elfsh_Addr off) {
       PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);

       if (!hdr)
               PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__,
                               "Invalid NULL parameter", -1);
		hdr->e_shoff = (elfsh_Off) off;
       PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
       } 


   3. Braces { and } should always be put at next line
   ---------------------------------------------------

   GOOD:

   if (test)
     var++;
   else
     {
       dosomething;
       anotherthing;
     }

   BAD:

   if(test) { var++ } else { otherthing; }


   4. Never use automatic indentation unless you know what you are doing
   ---------------------------------------------------------------------

   Never use an automatic indentation tool external to your editor : unless 
   you configure it to make the whole code compatible with the project coding 
   style of course. This is very important point as we dont have complete files 
   of many thousands of lines have changed indentation which is not the one
   of the project. 

   Additionally, this makes the cvs-web diff system to be completely inefficient
   as it will make appear all the lines where indentation has been modified.

   
   5. Your code must fit on 80 colums
   ----------------------------------

   Each line of your code must fit in a small terminal, so make sure you put
   a maximum width of 80 columns for each line.

   GOOD:

    sect = elfsh_get_section_by_name(world.curjob->current,
                                     world.curjob->curcmd->param[0],
                                     NULL, NULL, NULL);
   
   BAD : 
   
    sect = elfsh_get_section_by_name(world.curjob->current, world.curjob->curcmd->param[0], NULL, NULL, NULL);



  6. Your code shoud be compact be clear
  --------------------------------------

    Dont add any additional new break line if its not necessary, but put 
    spaces between keywords of the langage or operators. That includes :

    - if, else, for, while, sizeof

    - Put space after , but not before.

    - Put space on both sides of arithmetic or logic operands. That
    includes +, -, *, /, <<, >>, % , &, | and = signs

    - Never put additional spaces because of parenthesis ( ) or [ ]

    - Always put a space after a non-line-terminating ; or a , but not before.

    
			** DONT LET DEAD CODE IN THE CVS **

    You can keep that code as commented only, in case you plan to reuse it
    in the future or you are not sure it should be removed because you are
    not the author. In that case, ask on the mailing list.


B/ DEVELOPMENT
--------------
--------------

   1. Never use syscalls directly but use defined macros
   -----------------------------------------------------
   
   BAD:

           case NT_PRPSINFO:
             if(read(file->fd, &file->core.prpsinfo, descsz) != descsz)
               PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, -1);
             break;

  GOOD:

         case NT_PRPSINFO:
            XREAD(file->fd, &file->core.prpsinfo, descsz, -1);

   The same holds for malloc (XALLOC), realloc (XREALLOC), 
   free (XFREE), lseek (XSEEK), write (XWRITE), and close (XCLOSE).

   See: ./libaspect/include/libaspect-profiler.h for all macros.


   2. Use the profiler macros carefully ! 
   --------------------------------------

   Those allow your functions and allocations to be present in the 
   profiling (profile command) BUT take care if you are coding I/O 
   functions, as we dont want them to be present in the profiling 
   (else we will see profile output each time we type a key, which
   is very disturbing).

   Those macros are the one starting with PROFILER_* and NOPROFILER_*
   macros. If you have any doubt on using those macros, ask for help.

   They are defined in the file : libaspect/include/libaspect-profiler.h

   If you have any doubt if you should use the profiler in your function,
   use it and check in the result if it brings too much unwanted profile
   output inside elfsh.


   3. Your code should not generate warnings
   -----------------------------------------

   You should make sure that you dont let :

   - unused or uninitialized variables (use a decent version of gcc, 
   for instance gcc 4.x.

   - Make sure your code at least compiles when beeing compiled using 
   g++. This way, it can be used in C++ projects. The configure file 
   has a --use-cpp option that helps you for testing this.


   4. Your code should be portable
   -------------------------------

   If you just have access to your development machine, please get more 
   accounts or ask us to help you doing the portability checks before you 
   submit a patch for inclusion to cvs. For instance, if you develop on 
   Linux, try to make sure your code at least does not break the BSD 
   tree. In the best case scenario, make your code portable directly, 
   so that all your work is also available on FreeBSD, NetBSD, OpenBSD, 
   and Solaris (the main target operating systems of the project).


   5. Your code shoud take advantage of the existing API
   -----------------------------------------------------

   The API of the project has more than 500 functions.

   Well often, you will find inside it the function that you need. Dont
   duplicate code, your patch will make the project bigger and harder
   to maintain in the future.

   If you need to reuse an API but you need to change a small detail inside its
   code, make sur you dont break any feature that used this function before by
   testing the project. Make your change in the respect of the coding style
   defined earlier in that document.

   If you have to change more than a third of a function code, then you may 
   break it into smaller pieces, and call those smaller functions where you
   need them.
   
   If you add any function you must add its prototype at the correct place 
   in the correct header file.


   6. Your code should be modular
   ------------------------------

   If you have to do big changes and if you are coding a new big feature 
   inside the project, think about using VECTORS. Vectors are multidimensional
   arrays that allow for discriminating objects inside the framework. 
   For instance, if you add a feature that has a different implementation
   depending on the architecture of the Operating System, you might want
   to call a different
   function depending on the local parameters of the machine you are running
   ERESI on. The Vector data structures helps you at this. Additionally, vectors
   are reflective objects inside 

   More detailed use of vectors will be soon described in the HOWTO-port-ERESI
   documentation, describing all the hooks that allow for reflection inside
   the framework.

   Reflection is the technique that allow for internal objects of the framework	
   to be bound directly in the ERESI language. In other words, you can modify
   the content of framework objects as well, but for this you need to use
   HASH TABLES (hash_t) and VECTORS (vector_t) objects, whoose API is both
   implemented in the LIBASPECT component.


   7. Your code should be formated
   -------------------------------

   - Use explicit variable and function names !

   BAD:

   int f(int a, int r, int m)
   {
       if (!a)
          return (r % m);
        r *= a--;
        return (f(a, r, m));
   }

   GOOD:

   int	factorial_modulo(int nbr, int result, int modulo)
   {
     if (!nbr)
       return (result % modulo);
     result *= nbr--;
     return (factorial_modulo(nbr, result, modulo));
   }

   - Dont use meaningless suffixes like _adv : use a function
   name that self-describes the content of it.

   - Always prefix your functions name depending on the name
   of the component you are working into. Prefixes are :

   aproxy_   : for liballocproxy
   aspect_   : for libaspect
   asm_      : for libasm
   elfsh_    : for libelfsh
   mjr_      : for libmjollnir
   edfmt_    : for the eresi debug format lib
   dump_     : for libdump
   vm_       : for librevm
   e2dbg_    : for the debugger

   Sometimes, special functions have special prefixing that get
   out of this scheme. For exemple:

   profiler_ : for profiler functions inside libaspect
   parse_    : for parsing handlers (librevm/lang/grammar.c)
   cmd_      : for command handlers (librevm/cmd/*)

   Libui does not have yet a prefix scheme, but a good candidate
   is certainly: ui_ (to be determined) 


C/ TESTING
----------
----------


   1. Your code must be tested before commiting
   --------------------------------------------

   There is nothing that makes loose our time more than being blocked
   because one of the developer did not test (and sometimes even compiled)
   correctly his work, and this can block a whole team for many hours. 


   2. Be reactive to the crew CVS alerts
   -------------------------------------

   If unfortunately your code is not working after following all those
   advices (for instance you commited code that worked on linux but
   its not even compiling on BSD or Solaris), then ask for access to
   BSD or solaris machines if you dont have one, to resolve the problem
   quickly. The project aims to be multi-OS and multi-architecture and
   you cannot state that you did a multi-OS/arch work, or even a real
   group project, if you dont follow this rule.




D/ DOCUMENTATION :
------------------

   1. Write text files to explain the API of what you have done
   ------------------------------------------------------------

   That includes how to use it from the shell or from the ERESI language, if
   you bound your features inside the language.


   2. Comment your code in a reasonable fashion
   --------------------------------------------

   You should always have at least one line of comment for each function, if
   its small, or more comments in the body of the function if the function is
   more complex. The good way to put comments is :

   /** <--- double star allow for doxygen outputs
   *** This is the sumup of my comment
   ***
   *** More details about what Im talking about 
   **/
   int		my_function()
   {
     if (test)
       myvar++;
     else
       {
         I_do_something_else;
       }
      return (blah);
    }

    3. Use doxygen tags
    -------------------
    The eresi project grows and evolves too much to be kept appart from the source code.
    To facilitate documentation process, an effort has been made to migrate source code from
    comments with no constitency to doxygen commented source files.
    
    Use doxygen tags at last for files and functions.
    This will provide an API reference for future developpers which can be browsed online.
    Following tags are mandatory :

    Each .c file must begin with a section like this:

    /**
     * @file path/to/the/file.c
     * Quick description fo file content.
     */

    Functions must be preceded by comments like this:

    /**
     * Brief description of behavior
     *
     * Extended description.
     *
     * @param obj Pointer to data object
     * @param str String describing element
     * @return Returns variable lenght or NULL on error
     */
    int extract_length(objet_t *obj, char *str)
    {
      // ...
    }


----------------------------------------------------------------------------
----------------------------------------------------------------------------



That is all for now. This documentation is updated and completed soon, so
please check the CVS for new versions !

Thanks for your attention and your comprehension.

-may




		