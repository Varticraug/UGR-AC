// Compilar con -O2 y -fopenmp
#include <stdlib.h>
#include <stdio.h>
#include <omp.h>

int main(int argc, char** argv){
	int i, j;
	double t1, t2, total;

	//Leer argumento de entrada (no de componentes del vector)
	if (argc<2){
		printf("Falta tamaño de matriz y vector\n");
		exit(-1);
	}

	unsigned int N = atoi(argv[1]); // Máximo N =2^32-1=4294967295 (sizeof(unsigned int) = 4 B)

	double *v1, *v2, **M;
	v1 = (double*) malloc(N*sizeof(double));// malloc necesita el tamaño en bytes
	v2 = (double*) malloc(N*sizeof(double)); //si no hay espacio suficiente malloc devuelve NULL
	M = (double**) malloc(N*sizeof(double *));

	if ( (v1==NULL) || (v2==NULL) || (M==NULL) ){
		printf("Error en la reserva de espacio para los vectores\n");
		exit(-2);
	}

	for (i=0; i<N; i++){
		M[i] = (double*) malloc(N*sizeof(double));
		if ( M[i]==NULL ){
			printf("Error en la reserva de espacio para los vectores\n");
			exit(-2);
		}
	}

	//A partir de aqui se pueden acceder las componentes de la matriz como M[i][j]

   #pragma omp parallel private(i)
   {
		//Inicializar matriz y vectores
      #pragma omp for
      for(i=0; i<N; i++)
         v1[i]=1;

      #pragma omp for
      for(i=0; i<N; i++) {
           for(j=0; j<N; j++) {
               M[i][j]=1;
           }
      }

		//Medida de tiempo
		#pragma omp single
		{
			t1 = omp_get_wtime();
		}

		//Calcular producto de matriz por vector v2 = M · v1
	   for(i=0; i<N; i++) {
	      v2[i]=0;
			double res=0;
			#pragma omp for
	      for(j=0; j<N; j++) {
	           res+=(M[i][j]*v1[j]);
	      }
			#pragma omp atomic
				v2[i]+=res;
	   }

		//Medida de tiempo
		#pragma omp single
		{
			t2 = omp_get_wtime();
		}
   }

	total = t2 - t1;

	//Imprimir el resultado y el tiempo de ejecución
	printf("Tiempo(seg.): %11.9f\t / Tamaño:%u\t/ V2[0]=%8.6f V2[%d]=%8.6f\n", total,N,v2[0],N-1,v2[N-1]);

	i=0, j=0;
   if (N < 30){
      for(i=0; i<N; i++)
         printf("%f ", v2[i]);
      printf("\n");
   }

	free(v1); // libera el espacio reservado para v1
	free(v2); // libera el espacio reservado para v2
	for (i=0; i<N; i++)
		free(M[i]);
	free(M);

	return 0;
}
