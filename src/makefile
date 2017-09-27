Compilier 	  = clang
Program   	  = form
Sources  	  = $(wildcard *.c)
Flags         = -Wall -std=c99
Objects       = $(Sources:.c=.o) 


all: $(Program)

$(Program): $(Objects) 
	$(Compilier) $(Flags) $(Objects) -o $@ 

.c.o: 
	$(Compilier) $(Flags) $< -c -o $@ $(Incflags)

clean: 
	rm -f *.o $(Program)