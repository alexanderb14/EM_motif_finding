#include <stdlib.h>
#include <stdio.h>
#include <math.h>

typedef double **Motif;

static int Hmap[4] = { 'A', 'C', 'G', 'T' };
static int Lmap[4] = { 'a', 'c', 'g', 't' };

void genchar(double *pb, int *map)
{ double x;
  int    c;

  x = drand48();
  for (c = 0; c < 3; c++)
    if (x < pb[c])
      { putchar(map[c]);
        return;
      }
    else
      x -= pb[c];
  putchar(map[3]);
}

int main(int argc, char *argv[])
{ int    K, N, L;
  Motif  M;

  if (argc != 2)
    { fprintf(stderr,"Usage error: %s <file>\n",argv[0]);
      exit (1);
    }

  { int    j, c;
    double sum;
    FILE  *input;

    input = fopen(argv[1],"r");
    if (input == NULL)
      { fprintf(stderr,"%s: Cannot open %s\n",argv[0],argv[1]);
        exit (1);
      }

    if (fscanf(input," K=%d N=%d L=%d \n",&K,&N,&L) != 3)
      { fprintf(stderr,"%s: First line should be 'K=# N=# L=#'\n",argv[0]);
        exit (1);
      }
    if (K > L)
      { fprintf(stderr,"%s: K is bigger than L!\n",argv[0]);
        exit (1);
      }
    if (N <= 0 || K <= 0)
      { fprintf(stderr,"%s: N or K is less than 1!\n",argv[0]);
        exit (1);
      }

    M = (Motif) malloc(sizeof(double *)*(K+1));
    M[0] = (double *) malloc(sizeof(double)*(K+1)*4);
    for (j = 1; j <= K; j++)
      M[j] = M[j-1] + 4;

    for (j = 0; j <= K; j++)
      { if (fscanf(input," %lf %lf %lf %lf \n",M[j],M[j]+1,M[j]+2,M[j]+3) != 4)
          { fprintf(stderr,"%s: Should be 4 numbers in a row!\n",argv[0]);
            exit (1);
          }
        sum = 0.;
        for (c = 0; c < 4; c++)
          { if (M[j][c] < 0. || M[j][c] > 1.)
              { fprintf(stderr,"%s: Probability of %c at position %d is not in [0,1]\n",
                               argv[0],Hmap[c],j);
                exit (1);
              }
            sum += M[j][c];
          }
        if (fabs(sum - 1.) > 1.e-8)
          { fprintf(stderr,"%s: Sum of row %d is not 1\n",argv[0],j);
            exit (1);
          }
        M[j][3] += (1. - sum);
      }
    if ( ! feof(input))
      { fprintf(stderr,"%s: More than K+1 rows!\n",argv[0]);
        exit (1);
      }
  }

  printf("K=%d N=%d L=%d\n",K,N,L);

  { double *pb;
    int     j, z, i;

    pb = M[K];
    for (j = 0; j < N; j++)
      { z = drand48()*(L-K);
        for (i = 0; i < z; i++)
          genchar(pb,Lmap);
        for (i = 0; i < K; i++)
          genchar(M[i],Hmap);
        for (i = z+K; i < L; i++) 
          genchar(pb,Lmap);
        putchar('\n');
      }
  }

  exit (0);
}
