CFLAGS = -Wall -Wextra -g -O
objs = main.c structs.c

budget: $(objs)
	gcc $(CFLAGS) $(objs) -o budget
	@ echo "\nBuilt main program, please run \"make reader\" for converter."

reader: reader.c
	gcc $(CFLAGS) -o reader $<
	@ echo "\nBuilt reader."
	
