#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include <math.h>
#include <float.h>

#define TRIG_ACELERATION
#define FLOAT_ACELERATION
#define FLOAT_ACELERATION2

#define MEMBASE 0
#define MEMSIZE 67108864U //536870912U
#define LOCK_BASE MEMSIZE
#define LOCK_SIZE 4U

#define NUM_PROCS 2

#define FLOATING_BASE (LOCK_BASE + LOCK_SIZE)
#define FLOATING_ARG1 (FLOATING_BASE + 0*4U)
#define FLOATING_ARG2 (FLOATING_BASE + 1*4U)
#define FLOATING_ADD  (FLOATING_BASE + 2*4U)
#define FLOATING_SUB  (FLOATING_BASE + 3*4U)
#define FLOATING_MULT (FLOATING_BASE + 4*4U)
#define FLOATING_DIVI (FLOATING_BASE + 5*4U)
#define FLOATING_LOG2 (FLOATING_BASE + 6*4U)
#define FLOATING_SQRT (FLOATING_BASE + 7*4U)
#define FLOATING_SIZE (NUM_PROCS * (2+6)*4U)

#define TRIGONOMETRIC_BASE (FLOATING_BASE + FLOATING_SIZE)
#define COS_ADD (TRIGONOMETRIC_BASE + 0*4U)
#define SIN_ADD (TRIGONOMETRIC_BASE + 1*4U)
#define TRIGONOMETRIC_SIZE (NUM_PROCS* 8U)

#ifdef TRIG_ACELERATION
	#define COS_AC(x) (cos_ac((procID), (x)))
#else
	#define COS_AC(x) (cos((x)))
#endif
#ifdef TRIG_ACELERATION
	#define SIN_AC(x) (sin_ac((procID), (x)))
#else
	#define SIN_AC(x) (sin((x)))
#endif

#ifdef FLOAT_ACELERATION
	#define ADD_AC(x,y) (add_ac((procID), (x),(y)))
#else
	#define ADD_AC(x,y) ((x) + (y))
#endif
#ifdef FLOAT_ACELERATION
	#define SUB_AC(x,y) (sub_ac((procID), (x),(y)))
#else
	#define SUB_AC(x,y) ((x) - (y))
#endif
#ifdef FLOAT_ACELERATION
	#define MUL_AC(x,y) (mul_ac((procID), (x),(y)))
#else
	#define MUL_AC(x,y) ((x) * (y))
#endif
#ifdef FLOAT_ACELERATION
	#define DIV_AC(x,y) (div_ac((procID), (x),(y)))
#else
	#define DIV_AC(x,y) ((x) / (y))
#endif

#ifdef FLOAT_ACELERATION2
	#define LOG2_AC(x) (log2_ac((procID), (x)))
#else
	#define LOG2_AC(x) log2((x))
#endif
#ifdef FLOAT_ACELERATION2
	#define SQRT_AC(x) (sqrt_ac((procID), (x)))
#else
	#define SQRT_AC(x) sqrt((x))
#endif

#define PI 3.14159265358979323846

volatile struct trig {
	float cos, sin;
} * trig = (struct trig *) TRIGONOMETRIC_BASE;

volatile struct arit {
	float x,y, add,sub,mul,div, log2,sqrt;
} * arit = (struct arit *) FLOATING_BASE;

float cos_ac(int procID, float x){
	trig[procID].cos = x;
	//printf("%d = 0x%X = &(trig[%d].cos) = 0x%X + 0x%X\n",&(trig[procID].cos),&(trig[procID].cos),procID, TRIGONOMETRIC_BASE, &(trig[procID].cos) - TRIGONOMETRIC_BASE/4);
	return trig[procID].cos;
}

float sin_ac(int procID, float x){
	trig[procID].sin = x;
	//printf("%X = &(trig[%d].sin) = %u + %u\n",&(trig[procID].cos),procID, TRIGONOMETRIC_BASE, &(trig[procID].sin) - TRIGONOMETRIC_BASE/4);
	return trig[procID].sin;
}

float add_ac(int procID, float x, float y){
	arit[procID].x = x;
	arit[procID].y = y;
	return arit[procID].add;
}
float sub_ac(int procID, float x, float y){
	arit[procID].x = x;
	arit[procID].y = y;
	return arit[procID].sub;
}
float mul_ac(int procID, float x, float y){
	arit[procID].x = x;
	arit[procID].y = y;
	return arit[procID].mul;
}
float div_ac(int procID, float x, float y){
	arit[procID].x = x;
	arit[procID].y = y;
	return arit[procID].div;
}
float log2_ac(int procID, float x){
	arit[procID].x = x;
	return arit[procID].log2;
}
float sqrt_ac(int procID, float x){
	arit[procID].x = x;
	return arit[procID].sqrt;
}

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

volatile PGMData dados;
volatile Complex F[100][100], P[100][100];

volatile int *lock = (int *) 67108864U;
volatile int procIDCounterLock = 0;
volatile int rowsRemainingLock = 0;
volatile int startPointLock = 0;
volatile int maxLock = 0;
volatile int minLock = 0;
volatile int startPoint = 0;
volatile int procIDCounter = 0;
volatile int barrier0 = 0;
volatile int barrier1 = 0;
volatile int barrier2 = 0;
volatile int barrier3 = 0;
volatile int barrier4 = 0;
volatile int barrier5 = 0;
volatile int barrier6 = 0;
volatile int rowsRemaining = -1;
volatile float max = -FLT_MAX;
volatile float min = FLT_MAX;

#define HI(num) (((num) & 0x0000FF00) >> 8)
#define LO(num) ((num) & 0x000000FF)

/* Adquire o lock de hardware */
void AcquireGlobalLock() {
	while (*lock);
}

/* Libera o lock de hardware */
void ReleaseGlobalLock() {
	*lock = 0;
}

/* Adquire um lock local passado como parametro */
void AcquireLock(volatile int *local_lock) {
	while (1) {
		AcquireGlobalLock();
		if (*local_lock) {
  			ReleaseGlobalLock();
		} else {
  			*local_lock = 1;
  			ReleaseGlobalLock();
  			break;
		}
	}
}

/* Libera um lock local passado como parametro */
void ReleaseLock(volatile int *local_lock) {
	*local_lock = 0;
}

int getProcID() {
	int i;
	AcquireLock(&procIDCounterLock);
	i = procIDCounter;
	procIDCounter++;
	ReleaseLock(&procIDCounterLock);
	return i;
}

int getProcRow(int procID) {
	int row = dados.row/procIDCounter;
	AcquireLock(&rowsRemainingLock);
	if(rowsRemaining == -1) {
		rowsRemaining = dados.row % procIDCounter;
	} else {
		if(rowsRemaining != 0) {
			row += 1;
			rowsRemaining -= 1;
		}
	}
	ReleaseLock(&rowsRemainingLock);
	return row;
}

int getStartPoint(int procRow) {
	int sp = 0;
	AcquireLock(&startPointLock);
	sp = startPoint;
	startPoint += procRow;
	ReleaseLock(&startPointLock);
	return sp;
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

void readPGM(const char *file_name, volatile PGMData *data) {
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

void writePGM(const char *filename, volatile PGMData *data) {
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

void desloca_origem(int procID, volatile int imagem[][100], int row, int col, int procStartPoint) {
	int i, j;

	for (i = procStartPoint; i < (row + procStartPoint); i++) {
		for (j = 0; j < col; j++) {
			if ((i + j) % 2 != 0) {
				imagem[i][j] = -imagem[i][j];
			}
		}
	}

}

Complex complex_multiply(int procID, Complex x, Complex y) {
    Complex z;
    //z.a = x.a * y.a - x.b * y.b;
    z.a = SUB_AC(MUL_AC(x.a, y.a), MUL_AC(x.b, y.b));
    //z.b = x.a * y.b + x.b * y.a;
    z.b = ADD_AC(MUL_AC(x.a, y.b), MUL_AC(x.b, y.a));
    return z;
}

Complex complex_add(int procID, Complex x, Complex y) {
	Complex z;
	//z.a = x.a + y.a;
	z.a = ADD_AC(x.a, y.a);
	//z.b = x.b + y.b;
	z.b = ADD_AC(x.b, y.b);
	return z;
}


void DFT(int procID, volatile int f[][100], volatile Complex F[][100], int row, int col, int procStartPoint, int numberOfProc) {
	int u, v, x, y, i, j;
	float ang;
	Complex z, aux;
	
	for (v = procStartPoint; v < (row + procStartPoint); v++) {
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
				aux = complex_add(procID, aux, z);
			}
			P[v][u].a = aux.a;
			P[v][u].b = aux.b;
		}
	}

	// Incrementa quando terminar a DFT na sua porção da matriz
	AcquireLock(&procIDCounterLock);
	barrier5++;
	ReleaseLock(&procIDCounterLock);

	while(barrier5 != numberOfProc);

	for (v = procStartPoint; v < (row + procStartPoint); v++) {
		for (u = 0; u < col; u++) {
			aux.a = 0;
			aux.b = 0;
			//ang = (2 * PI * v / dados.row);
			ang = DIV_AC(MUL_AC(MUL_AC(2,PI),v), dados.row);
			for (y = 0; y < dados.row; y++) {
				//z.a = cos(y * ang);
				z.a = COS_AC(MUL_AC(y , ang));
				//z.b = -sin(y * ang);
				z.b = -SIN_AC(MUL_AC(y , ang));
				aux = complex_add(procID, aux, complex_multiply(procID, P[y][u], z));
			}
			F[v][u].a = aux.a;
			F[v][u].b = aux.b;
		}
	}

	for (v = procStartPoint; v < (row + procStartPoint); v++) {
		for (u = 0; u < col; u++) {
			//F[v][u].a = 20 * log2(sqrt((F[v][u].a * F[v][u].a) + (F[v][u].b * F[v][u].b)));
			F[v][u].a = MUL_AC(20,LOG2_AC(SQRT_AC(ADD_AC(MUL_AC(F[v][u].a, F[v][u].a), MUL_AC(F[v][u].b, F[v][u].b)))));
		}
	}

}

void max_min(volatile Complex F[][100], int row, int col, int procStartPoint) {
	int v, u;

	float local_max = -FLT_MAX;
	float local_min = FLT_MAX;

	for (v = procStartPoint; v < (row + procStartPoint); v++) {
		for (u = 0; u < col; u++) {
			if (F[v][u].a > local_max) {
				local_max = F[v][u].a;
			}

			if (F[v][u].a < local_min) {
				local_min = F[v][u].a;
			}
		}
	}

	AcquireLock(&maxLock);
	if(local_max > max) {
		max = local_max;
	}
	ReleaseLock(&maxLock);

	AcquireLock(&minLock);
	if(local_min < min) {
		min = local_min;
	}
	ReleaseLock(&minLock);

}

void transforma_intervalo(int procID, volatile int f[][100], volatile Complex F[][100], int row, int col, int procStartPoint, int numberOfProc) {
	//float max, min;
	int v, u;

	max_min(F, row, col, procStartPoint);

	// Incrementa quando o proc terminar max_min da sua porção da matriz
	AcquireLock(&procIDCounterLock);
	barrier2++;
	ReleaseLock(&procIDCounterLock);

	/* Barreira para que a execução só continue quando todos os proc terminarem
	max_min e o max e o min tenham o valor correto */
	while(barrier2 != numberOfProc);

	for (v = procStartPoint; v < (row + procStartPoint); v++) {
		for (u = 0; u < col; u++) {
			//f[v][u] = 255 * (F[v][u].a - min) / (max - min);
			f[v][u] = MUL_AC(255, DIV_AC(SUB_AC(F[v][u].a,min), SUB_AC(max,min)));
		}
	}

}

int main(int ac, char *av[]) {
	int i, j;

	int procStartPoint = 0;
	int procRow = 0;
	int numberOfProc = 0;

	if (ac != 3) {
		printf("Numero errado de argumentos\n");
		exit(0);
		return 0;
	}
	
	numberOfProc = NUM_PROCS;
	int procID = getProcID(procRow);

	///////////////////////////////////////////////////////////////////////////
	/*
	AcquireGlobalLock();

	printf("%d: 1000*COS_AC(PI*%d/4) = %d\n", procID, procID, (int) (1000*COS_AC(PI*procID/4) ) );

	ReleaseGlobalLock();
	while(1);

	exit(0);
	*/
	///////////////////////////////////////////////////////////////////////////

	// Só o zero lê
	if(procID == 0) {
		readPGM(av[1], &dados);
	}

	AcquireLock(&procIDCounterLock);
	barrier6++;
	ReleaseLock(&procIDCounterLock);

	// Barreira para que a execução continue quando todos os proc tem um ID
	while(barrier6 != numberOfProc);

	procRow = getProcRow(procID);

	procStartPoint = getStartPoint(procRow);

	desloca_origem(procID, dados.matrix, procRow, dados.col, procStartPoint);

	// Incrementa quando terminar de deslocar a origem da sua porção da matriz
	AcquireLock(&procIDCounterLock);
	barrier0++;
	ReleaseLock(&procIDCounterLock);

	/* Barreira para que a execução só continue quando todos os proc terminarem
		desloca_origem */
	while(barrier0 != numberOfProc);

	DFT(procID,dados.matrix, F, procRow, dados.col, procStartPoint, numberOfProc);

	// Incrementa quando terminar a DFT na sua porção da matriz
	AcquireLock(&procIDCounterLock);
	barrier1++;
	ReleaseLock(&procIDCounterLock);

	// Barreira para que a execução só continue quando todos os proc terminarem DFT
	while(barrier1 != numberOfProc);

	transforma_intervalo(procID, dados.matrix, F, procRow, dados.col, procStartPoint, numberOfProc);

	// Incrementa quando terminar de transformar_intervalo
	AcquireLock(&procIDCounterLock);
	barrier3++;
	ReleaseLock(&procIDCounterLock);

	/* Barreira para que a execução só continue quando todos os proc terminarem
		transformar_intervalo */
	while(barrier3 != numberOfProc);

	// Só o zero escreve
	if(procID == 0) {
		writePGM(av[2], &dados);
	}

	AcquireLock(&procIDCounterLock);
	barrier4++;
	ReleaseLock(&procIDCounterLock);

	while (barrier4 != numberOfProc);

	exit(0);
	return 0;
}
