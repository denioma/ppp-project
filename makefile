CFLAGS = -Wall -Wextra -g -O
objs = main.c structs.c

budget: $(objs)
	gcc $(CFLAGS) $(objs) -o budget
	@ echo "\nBuilt main program, please run \"make converter\" for converter."

converter: reader.c
	gcc $(CFLAGS) -o convert $<
	@ echo "\nBuilt reader."
	
