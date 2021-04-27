SRC = ./src
OBJ = ./obj

all: obj_path kernel_stats.o

kernel_stats.o: $(SRC)/kernel_stats.c $(SRC)/kernel_stats.h
	mpicc -c $(SRC)/kernel_stats.c -o $(OBJ)/kernel_stats.o

obj_path:
	mkdir $(OBJ)

clean:
	rm -rf $(OBJ)
