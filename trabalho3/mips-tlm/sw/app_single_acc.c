#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include <math.h>
#include <stdint.h>

#define TRIG_ACELERATION
#define FLOAT_ACELERATION

#define MEMBASE 0
#define MEMSIZE 67108864U //536870912U
#define LOCK_BASE MEMSIZE
#define LOCK_SIZE 4U

#define FLOATING_BASE (LOCK_BASE + LOCK_SIZE)
#define FLOATING_ARG1 (FLOATING_BASE + 0*4U)
#define FLOATING_ARG2 (FLOATING_BASE + 1*4U)
#define FLOATING_ADD  (FLOATING_BASE + 2*4U)
#define FLOATING_SUB  (FLOATING_BASE + 3*4U)
#define FLOATING_MULT (FLOATING_BASE + 4*4U)
#define FLOATING_DIVI (FLOATING_BASE + 5*4U)
#define FLOATING_LOG2 (FLOATING_BASE + 6*4U)
#define FLOATING_SQRT (FLOATING_BASE + 7*4U)
#define FLOATING_SIZE (2+6)*4U

#define TRIG_BASE (FLOATING_BASE + FLOATING_SIZE)
#define COS_ADD TRIG_BASE
#define SIN_ADD (COS_ADD + 4U)
#define TRIGONOMETRIC_SIZE 8U

#ifdef TRIG_ACELERATION
	#define COS_AC(x) (cos_ac((x)))
#else
	#define COS_AC(x) (cos((x)))
#endif
#ifdef TRIG_ACELERATION
	#define SIN_AC(x) (sin_ac((x)))
#else
	#define SIN_AC(x) (sin((x)))
#endif

#ifdef FLOAT_ACELERATION
	#define ADD_AC(x,y) (add_ac((x),(y)))
#else
	#define ADD_AC(x,y) ((x) + (y))
#endif
#ifdef FLOAT_ACELERATION
	#define SUB_AC(x,y) (sub_ac((x),(y)))
#else
	#define SUB_AC(x,y) ((x) - (y))
#endif
#ifdef FLOAT_ACELERATION
	#define MUL_AC(x,y) (mul_ac((x),(y)))
#else
	#define MUL_AC(x,y) ((x) * (y))
#endif
#ifdef FLOAT_ACELERATION
	#define DIV_AC(x,y) (div_ac((x),(y)))
#else
	#define DIV_AC(x,y) ((x) / (y))
#endif

#ifdef FLOAT_ACELERATION
	#define LOG2_AC(x) (log2_ac((x)))
#else
	#define LOG2_AC(x) log2((x))
#endif
#ifdef FLOAT_ACELERATION
	#define SQRT_AC(x) (sqrt_ac((x)))
#else
	#define SQRT_AC(x) sqrt((x))
#endif

#define PI 3.14159265358979323846

#define HI(num) (((num) & 0x0000FF00) >> 8)
#define LO(num) ((num) & 0x000000FF)

volatile struct trig {
	float cos, sin;
} * trig = (struct trig *) TRIG_BASE;

volatile struct arit {
	float x,y,add,sub,mul,div, log2,sqrt, r9,r10;
} * arit = (struct arit *) FLOATING_BASE;

volatile int *outro = (int *) 67109040;

typedef struct _PGMData {
	int row;
	int col;
	int max_gray;
	int matrix[100][100];
} PGMData;

typedef struct Complex {
    float a;
    float b;
} Complex;

PGMData dados;
Complex F[100][100], P[100][100];

float cos_ac(float x){
	trig->cos = x;
	return trig->cos;
}

float sin_ac(float x){
	trig->sin = x;
	return trig->sin;
}

float add_ac(float x, float y){
	arit->x = x;
	arit->y = y;
	return arit->add;
}
float sub_ac(float x, float y){
	arit->x = x;
	arit->y = y;
	return arit->sub;
}
float mul_ac(float x, float y){
	arit->x = x;
	arit->y = y;
	return arit->mul;
}
float div_ac(float x, float y){
	arit->x = x;
	arit->y = y;
	return arit->div;
}
float log2_ac(float x){
	arit->log2 = x;
	return arit->log2;
}
float sqrt_ac(float x){
	arit->sqrt = x;
	return arit->sqrt;
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

}

void desloca_origem(int imagem[100][100], int row, int col) {
	int i, j;

	for (i = 0; i < row; i++) {
		for (j = 0; j < col; j++) {
			if ((i + j) % 2 != 0) {
				imagem[i][j] = -imagem[i][j];
			}
		}
	}

}

Complex complex_multiply(Complex x, Complex y) {
    Complex z;
    //z.a = x.a * y.a - x.b * y.b;
    z.a = SUB_AC(MUL_AC(x.a, y.a), MUL_AC(x.b, y.b));
    //z.b = x.a * y.b + x.b * y.a;
    z.b = ADD_AC(MUL_AC(x.a, y.b), MUL_AC(x.b, y.a));
    return z;
}

Complex complex_add(Complex x, Complex y) {
	Complex z;
	//z.a = x.a + y.a;
	z.a = ADD_AC(x.a, y.a);
	//z.b = x.b + y.b;
	z.b = ADD_AC(x.b, y.b);
	return z;
}

void DFT(int f[100][100], Complex F[100][100], int row, int col) {
	int u, v, x, y;
	float ang;
	Complex z, aux;

	for (v = 0; v < row; v++) {
		for (u = 0; u < col; u++) {
			aux.a = 0;
			aux.b = 0;
			//ang = (2 * PI * u / col);
			ang = DIV_AC(MUL_AC(MUL_AC(2,PI),u), col);
			for (x = 0; x < col; x++) {
				//z.a = f[v][x] * COS_AC(x * ang);
				z.a = MUL_AC(f[v][x], COS_AC(MUL_AC(x, ang)));
				//z.b = -f[v][x] * SIN_AC(x * ang);
				z.b = -MUL_AC(f[v][x], SIN_AC(MUL_AC(x, ang)));
				aux = complex_add(aux, z);
			}
			P[v][u].a = aux.a;
			P[v][u].b = aux.b;
		}
	}

	for (v = 0; v < row; v++) {
		for (u = 0; u < col; u++) {
			aux.a = 0;
			aux.b = 0;
			//ang = (2 * PI * v / row);
			ang = DIV_AC(MUL_AC(MUL_AC(2,PI),v), row);
			for (y = 0; y < row; y++) {
				//z.a = cos(y * ang);
				z.a = COS_AC(MUL_AC(y , ang));
				//z.b = -sin(y * ang);
				z.b = -SIN_AC(MUL_AC(y , ang));
				aux = complex_add(aux, complex_multiply(P[y][u], z));
			}
			F[v][u].a = aux.a;
			F[v][u].b = aux.b;
		}
	}

	for (v = 0; v < row; v++) {
		for (u = 0; u < col; u++) {
			//F[v][u].a = 20 * log2(sqrt((F[v][u].a * F[v][u].a) + (F[v][u].b * F[v][u].b)));
			F[v][u].a = MUL_AC(20,log2(sqrt(ADD_AC(MUL_AC(F[v][u].a, F[v][u].a), MUL_AC(F[v][u].b, F[v][u].b)))));
		}
	}

}

void max_min(Complex F[100][100], int row, int col, float *min, float *max) {
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

void transforma_intervalo(int f[100][100], Complex F[100][100], int row, int col) {
	float max, min;
	int v, u;

	max_min(F, row, col, &min, &max);
	
	for (v = 0; v < row; v++) {
		for (u = 0; u < col; u++) {
			//f[v][u] = 255 * (F[v][u].a - min) / (max - min);
			f[v][u] = MUL_AC(255, DIV_AC(SUB_AC(F[v][u].a,min), SUB_AC(max,min)));
		}
	}

}

void printFloat(float src){
	int i;
	union float_bytes {
       float val;
       unsigned x;
    } data;
    data.val = src;
	unsigned x = data.x;
	char s = (x>>31)?'-':'+';
	short e  = ((x<<1)>>(32-8-1))-256;
	unsigned frac = ((x<<9)>>(32-9));
	frac |= 1<<(32-9);
	printf("%X(%c|%d|%x)", x, s, e, frac);
	e = (e * 10) / 3;
	float y = src;
	if(e > 0){
		for (i = 0; i < e; i++){
			//y /= 10;
			y = DIV_AC(y,10);
		} 
	}else{
		for (i = 0; i > e; i--){
			//y *= 10;
			y = MUL_AC(y,10);
		} 
	}
	long z = (long) y;
	printf("%de%d\n", z, e);
}

int main(int ac, char *av[]) {
	//printf("(MUL_AC(27.0f, 42.0f)*100000.0) = %d\n",(int)((MUL_AC(27.0f, 42.0f)*100000.0) ) );
	
	int i, j;

	if (ac != 3) {
		printf("Numero errado de argumentos\n");
		exit(0);
		return 0;
	}

	readPGM(av[1], &dados);

	desloca_origem(dados.matrix, dados.row, dados.col);

	DFT(dados.matrix, F, dados.row, dados.col);

	transforma_intervalo(dados.matrix, F, dados.row, dados.col);

	writePGM(av[2], &dados);

	exit(0);
	return 0;
}
