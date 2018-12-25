.PHONEY:clean

ALL: philosopher 
RM_OBJ = philosopher

philosopher : comm.c philosopher.c 
	gcc -pthread $^ -o $@
clean:
	rm -rf $(RM_OBJ)
