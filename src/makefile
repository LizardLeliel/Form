Compilier 	  = clang
Program   	  = form
Sources  	  = $(wildcard *.c)
Flags         = -Wall -std=c99 -g3 -ggdb 
Objects       = $(Sources:.c=.o) 


all: $(Program)

$(Program): lex.yy.c $(Objects) lex.yy.o
	$(Compilier) $(Flags) $(Objects) -o $@  lex.yy.o

.c.o: 
	$(Compilier) $(Flags) $< -c -o $@ $(Incflags)

lex.yy.c:
	flex --header-file=lex.yy.h formLang.flex

clean: 
	rm -f *.o $(Program)
	rm -f lex.yy.*

remake:
	make clean
	make