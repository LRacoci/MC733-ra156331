all: omp-separado

gp-omp-separado:
	gcc -pg -mtune=native -fopenmp main.c -o main
	./main 1000000
	gprof main > main.with-mtune-omp.stats

gp-omp-junto:
	gcc -pg -mtune=native -fopenmp junto.c -o junto
	./junto 1000000
	gprof junto > junto.with-mtune-omp.stats

omp-separado:
	gcc -mtune=native -fopenmp main.c -o main
	time ./main 1000000

omp-junto:
	gcc -mtune=native -fopenmp junto.c -o junto
	time ./junto 1000000

gproof-separado:
	gcc -pg -mtune=native main.c -o main
	./main 1000000
	gprof main > main.with-mtune.stats

o0-separado:
	gcc -O0 main.c -o main
	time ./main 1000000

o1-separado:
	gcc -O1 main.c -o main
	time ./main 1000000

o2-separado:
	gcc -O2 main.c -o main
	time ./main 1000000

o3-separado:
	gcc -O3 main.c -o main
	time ./main 1000000

mtune-separado:
	gcc -mtune=native main.c -o main
	time ./main 1000000

gproof-junto:
	gcc -pg -mtune=native junto.c -o junto
	./junto 1000000
	gprof junto > junto.with-mtune.stats

o0-junto:
	gcc -O0 junto.c -o junto
	time ./junto 1000000

o1-junto:
	gcc -O1 junto.c -o junto
	time ./junto 1000000

o2-junto:
	gcc -O2 junto.c -o junto
	time ./junto 1000000

o3-junto:
	gcc -O3 junto.c -o junto
	time ./junto 1000000

mtune-junto:
	gcc -mtune=native junto.c -o junto
	time ./junto 1000000

clean:
	rm -f main junto primo