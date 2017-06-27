#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include <math.h>

#define PI 3.14159265358979323846

typedef struct _PGMData {
	int row;
	int col;
	int max_gray;
	int matrix[512][512];
} PGMData;

typedef struct Complex {
    float a;
    float b;
} Complex;

PGMData dados;
Complex F[512][512], P[512][512];

#define HI(num) (((num) & 0x0000FF00) >> 8)
#define LO(num) ((num) & 0x000000FF)

/*int **allocate_dynamic_matrix(int row, int col) {
	int **ret_val;
	int i;
 
	ret_val = (int **)malloc(sizeof(int *) * row);
	if (ret_val == NULL) {
		perror("memory allocation failure");
		exit(EXIT_FAILURE);
	}
 
	for (i = 0; i < row; ++i) {
		ret_val[i] = (int *)malloc(sizeof(int) * col);
		if (ret_val[i] == NULL) {
			perror("memory allocation failure");
			exit(EXIT_FAILURE);
		}
	}
 
	return ret_val;
}
 
void deallocate_dynamic_matrix(int **matrix, int row) {
	int i;
 
	for (i = 0; i < row; ++i)
		free(matrix[i]);
	free(matrix);

}*/

void SkipComments(FILE *fp) {
	int ch;
	char line[100];
 
	while ((ch = fgetc(fp)) != EOF && isspace(ch));
	if (ch == '#') {
		fgets(line, sizeof(line), fp);
		SkipComments(fp);
	} else {
		fseek(fp, -1, SEEK_CUR);
	}

}

void readPGM(const char *file_name, PGMData *data) {
	FILE *pgmFile;
	char version[3];
	int i, j;
	int lo, hi;
 
	pgmFile = fopen(file_name, "rb");
	if (pgmFile == NULL) {
		perror("cannot open file to read");
		exit(EXIT_FAILURE);
	}
 
	fgets(version, sizeof(version), pgmFile);
	if (strcmp(version, "P5")) {
		fprintf(stderr, "Wrong file type!\n");
		exit(EXIT_FAILURE);
	}
 
	SkipComments(pgmFile);
	fscanf(pgmFile, "%d", &data->col);
	SkipComments(pgmFile);
	fscanf(pgmFile, "%d", &data->row);
	SkipComments(pgmFile);
	fscanf(pgmFile, "%d", &data->max_gray);
	fgetc(pgmFile);
 	
	//data->matrix = allocate_dynamic_matrix(data->row, data->col);
	if (data->max_gray > 255)
		for (i = 0; i < data->row; ++i)
			for (j = 0; j < data->col; ++j) {
				hi = fgetc(pgmFile);
				lo = fgetc(pgmFile);
				data->matrix[i][j] = (hi << 8) + lo;
			}
	else
		for (i = 0; i < data->row; ++i)
			for (j = 0; j < data->col; ++j) {
				lo = fgetc(pgmFile);
				data->matrix[i][j] = lo;
			}
 
	fclose(pgmFile);

}
 
void writePGM(const char *filename, const PGMData *data) {
	FILE *pgmFile;
	int i, j;
	int hi, lo;
 
	pgmFile = fopen(filename, "wb");
	if (pgmFile == NULL) {
		perror("cannot open file to write");
		exit(EXIT_FAILURE);
	}
 
	fprintf(pgmFile, "P5 ");
	fprintf(pgmFile, "%d %d ", data->col, data->row);
	fprintf(pgmFile, "%d ", data->max_gray);
 
	if (data->max_gray > 255) {
		for (i = 0; i < data->row; ++i) {
			for (j = 0; j < data->col; ++j) {
				hi = HI(data->matrix[i][j]);
				lo = LO(data->matrix[i][j]);
				fputc(hi, pgmFile);
				fputc(lo, pgmFile);
			}
		}
	} else {
		for (i = 0; i < data->row; ++i)
			for (j = 0; j < data->col; ++j) {
				lo = LO(data->matrix[i][j]);
				fputc(lo, pgmFile);
			}
	}
 
	fclose(pgmFile);
	//deallocate_dynamic_matrix(data->matrix, data->row);

}

/*Complex **allocate_Complex_matrix(int row, int col) {
	Complex **ret_val;
	int i;
 
	ret_val = (Complex **)malloc(sizeof(Complex *) * row);
	if (ret_val == NULL) {
		perror("memory allocation failure");
		exit(EXIT_FAILURE);
	}
 
	for (i = 0; i < row; ++i) {
		ret_val[i] = (Complex *)malloc(sizeof(Complex) * col);
		if (ret_val[i] == NULL) {
			perror("memory allocation failure");
			exit(EXIT_FAILURE);
		}
	}
 
	return ret_val;
}
 
void deallocate_Complex_matrix(Complex **matrix, int row) {
	int i;
 
	for (i = 0; i < row; ++i)
		free(matrix[i]);
	free(matrix);

}*/

void desloca_origem(int imagem[512][512], int row, int col) {
	int i, j;

	for (i = 0; i < row; i++) {
		for (j = 0; j < col; j++) {
			if ((i + j) % 2 != 0) {
				imagem[i][j] = -imagem[i][j];
			}
		}
	}

}

Complex multiply(Complex x, Complex y) {
    Complex z;
    z.a = x.a * y.a - x.b * y.b;
    z.b = x.a * y.b + x.b * y.a;
    return z;
}

Complex add(Complex x, Complex y) {
	Complex z;
	z.a = x.a + y.a;
	z.b = x.b + y.b;
	return z;
}

void DFT(int f[512][512], Complex F[512][512], int row, int col) {
	int u, v, x, y, i, j;
	float ang;
	Complex z, aux;
	//Complex **P;

	//P = allocate_Complex_matrix(row, col);

	for (v = 0; v < row; v++) {
		for (u = 0; u < col; u++) {
			aux.a = 0;
			aux.b = 0;
			ang = (2 * PI * u / col);
			for (x = 0; x < col; x++) {
				z.a = f[v][x] * cos(x * ang);
				z.b = -f[v][x] * sin(x * ang);
				aux = add(aux, z);
			}
			P[v][u].a = aux.a;
			P[v][u].b = aux.b;
		}
	}

	for (v = 0; v < row; v++) {
		for (u = 0; u < col; u++) {
			aux.a = 0;
			aux.b = 0;
			ang = (2 * PI * v / row);
			for (y = 0; y < row; y++) {
				z.a = cos(y * ang);
				z.b = -sin(y * ang);
				aux = add(aux, multiply(P[y][u], z));
			}
			F[v][u].a = aux.a;
			F[v][u].b = aux.b;
		}
	}

	//deallocate_Complex_matrix(P, row);

	for (v = 0; v < row; v++) {
		for (u = 0; u < col; u++) {
			F[v][u].a = 20 * log2(sqrt((F[v][u].a * F[v][u].a) + (F[v][u].b * F[v][u].b)));
		}
	}

}

void max_min(Complex F[512][512], int row, int col, float *min, float *max) {
	float r_min = F[0][0].a, r_max = F[0][0].a;
	int v, u;

	for (v = 0; v < row; v++) {
		for (u = 0; u < col; u++) {
			if (F[v][u].a > r_max) {
				r_max = F[v][u].a;
			}
			if (F[v][u].a < r_min) {
				r_min = F[v][u].a;
			}
		}
	}

	*min = r_min;
	*max = r_max;

}

void transforma_intervalo(int f[512][512], Complex F[512][512], int row, int col) {
	float max, min;
	int v, u;

	max_min(F, row, col, &min, &max);
	
	for (v = 0; v < row; v++) {
		for (u = 0; u < col; u++) {
			f[v][u] = 255 * (F[v][u].a - min) / (max - min);
		}
	}

}

int main(int ac, char *av[]) {
	//PGMData *dados;
	int i, j;
	//Complex **F;

	if (ac != 3) {
		printf("Numero errado de argumentos\n");
		exit(0);
		return 0;
	}

	//dados = (PGMData *) malloc(sizeof(PGMData));
	readPGM(av[1], &dados);

	desloca_origem(dados.matrix, dados.row, dados.col);

	//F = allocate_Complex_matrix(dados->row, dados->col);

	DFT(dados.matrix, F, dados.row, dados.col);

	transforma_intervalo(dados.matrix, F, dados.row, dados.col);

	writePGM(av[2], &dados);

	//deallocate_Complex_matrix(F, dados->row);
	//free(dados);

	exit(0);
	return 0;
}
