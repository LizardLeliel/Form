Compilier 	  = clang
Program   	  = form
Sources  	  = $(wildcard *.c)
Flags         = -Wall -std=c99
Objects       = $(Sources:.c=.o) 


all: $(Program)

$(Program): $(Objects) 
	$(Compilier) $(Flags) $(Objects) -o $@ 

.c.o: 
	flex formLang.flex
	$(Compilier) $(Flags) $< -c -o $@ $(Incflags)

clean: 
	rm -f *.o $(Program)
	rm -f lex.yy.c