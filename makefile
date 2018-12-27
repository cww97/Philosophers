.PHONEY:clean

ALL: philosopher_omp 
RM_OBJ = philosopher_omp

philosopher_omp : philosopher_omp.c 
	gcc -fopenmp $^ -o $@
philosopher : philosopher.c 
	gcc -pthread $^ -o $@
clean:
	rm -rf $(RM_OBJ)
