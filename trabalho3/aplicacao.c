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
	int **matrix;
} PGMData;

#define HI(num) (((num) & 0x0000FF00) >> 8)
#define LO(num) ((num) & 0x000000FF)

int **allocate_dynamic_matrix(int row, int col) {
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

}

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

PGMData* readPGM(const char *file_name, PGMData *data) {
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
 
	data->matrix = allocate_dynamic_matrix(data->row, data->col);
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
	return data;
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
	deallocate_dynamic_matrix(data->matrix, data->row);

}

float **allocate_float_matrix(int row, int col) {
	float **ret_val;
	int i;
 
	ret_val = (float **)malloc(sizeof(float *) * row);
	if (ret_val == NULL) {
		perror("memory allocation failure");
		exit(EXIT_FAILURE);
	}
 
	for (i = 0; i < row; ++i) {
		ret_val[i] = (float *)malloc(sizeof(float) * col);
		if (ret_val[i] == NULL) {
			perror("memory allocation failure");
			exit(EXIT_FAILURE);
		}
	}
 
	return ret_val;
}
 
void deallocate_float_matrix(float **matrix, int row) {
	int i;
 
	for (i = 0; i < row; ++i)
		free(matrix[i]);
	free(matrix);

}

void desloca_origem(int **imagem, int row, int col) {
	int i, j;

	for (i = 0; i < row; i++) {
		for (j = 0; j < col; j++) {
			if ((i + j) % 2 != 0) {
				imagem[i][j] = -imagem[i][j];
			}
		}
	}

}

void DFT(int **f, float **F, int row, int col) {
	int u, v, x, y;
	float real = 0, imaginaria = 0, aux = 0;

	for (v = 0; v < row; v++) {
		for (u = 0; u < col; u++) {
			real = 0;
			imaginaria = 0;
			for (y = 0; y < row; y++) {
				for (x = 0; x < col; x++) {
					aux = (u * x / col) + (v * y / row);
					real += f[y][x] * cos(2 * PI * aux);
					imaginaria += -f[y][x] * sin(2 * PI * aux);
				}
			}
			F[v][u] = sqrt((real * real) + (imaginaria * imaginaria));
		}
	}

}

void max_min(float **F, int row, int col, float *min, float *max) {
	
}

void transforma_intervalo(int **f, float **F, int row, int col) {

}

int main(int ac, char *av[]) {
	PGMData *dados;
	int i, j;
	float **F;

	if (ac != 3) {
		printf("Numero errado de argumentos\n");
		exit(0);
		return 0;
	}

	dados = (PGMData *) malloc(sizeof(PGMData));
	dados = readPGM(av[1], dados);

	desloca_origem(dados->matrix, dados->row, dados->col);

	F = allocate_float_matrix(dados->row, dados->col);

	DFT(dados->matrix, F, dados->row, dados->col);

	for (i = 0; i < dados->row; i++) {
		for (j = 0; j < dados->col; j++) {
			printf("%f ", F[i][j]);
		}
		printf("\n");
	}

	deallocate_float_matrix(F, dados->row);
	deallocate_dynamic_matrix(dados->matrix, dados->row);
	free(dados);

	exit(0);
	return 0;
}
