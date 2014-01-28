//EM algorithm for motif finding, Alexander Brauckmann 2014

#define _FILENAME "GATA1.txt"
#define _K 30
#define _N 20
#define _L 200
#define _pseudocounts 0.001

#include<stdio.h>
#include<stdlib.h>
#include<math.h>
#include <stdbool.h>

int K;
int N;
int L;

struct containerSeqs {
    char seqs[_N][_L];
};

struct containerM {
    int initial;
	float m[4][_K];
};

// probability motif starts at z
struct containerPrz {
    float prz[_N][_L];
};

void parseSeqs(struct containerSeqs cSeqs) {
    printf("K=%d, N=%d, L=%d\n", K, N, L);

	printf("\nSequences:\n");

	for(int n=0; n<N; n++) {
		for(int l=0; l<L; l++) {
			printf("%c", cSeqs.seqs[n][l]);
		}
		printf("\n");
	}
}

void parseM(struct containerM cM) {
    printf("   A    C    G    T\n");

    for(int k=0; k<=K; k++) {
        printf("%d |", k);

        for(int b=0; b<4; b++) {
			printf("%.2f ", cM.m[b][k]);
		}

        if(k==0) printf("  0-st line... background probability p_c");
        if(k==1) printf("  1-st to k-th line... M_pr");

		printf("\n");
	}
}

void parsePrz(struct containerPrz cPrz) {
    printf("K=%d, N=%d, L=%d\n", K, N, L);

	printf("\nProbability Matrix:\n");

	for(int n=0; n<N; n++) {
        printf("\n-------------\nn=%d\n", n);

		for(int z=0; z<L-K; z++) {
			printf("%f ", cPrz.prz[n][z]);
		}
	}
}

int getIntFormChar(char c) {
    int base;

    switch(c) {
        case 'a': base=0; break;
        case 'c': base=1; break;
        case 'g': base=2; break;
        case 't': base=3; break;
    }

    return base;
}

/*
    transform sequence matrix of mixed lower-/uppercase letters to
    a sequence matrix of lowercase letters
*/
struct containerSeqs transformSeqs(struct containerSeqs cSeqs) {
	for(int n=0; n<N; n++) {
		for(int l=0; l<L; l++) {
			if(cSeqs.seqs[n][l] == 'A') cSeqs.seqs[n][l] = 'a';
			if(cSeqs.seqs[n][l] == 'C') cSeqs.seqs[n][l] = 'c';
			if(cSeqs.seqs[n][l] == 'G') cSeqs.seqs[n][l] = 'g';
			if(cSeqs.seqs[n][l] == 'T') cSeqs.seqs[n][l] = 't';
		}
	}

	return cSeqs;
}

struct containerM compute_f_i_c(int s, int z, struct containerSeqs cSeqs) {
    struct containerM cM;

    //init PWM with zeros
    for(int k=0; k<=K; k++) {
        for(int b=0; b<4; b++) {
            cM.m[b][k] = 0;
        }
    }

    //compute p_c
    for(int i=0; i<z; i++) {
        switch(cSeqs.seqs[s][i]) {
			case 'a': cM.m[0][0]++; break;
			case 'c': cM.m[1][0]++; break;
			case 'g': cM.m[2][0]++; break;
			case 't': cM.m[3][0]++; break;
		}
    }

    for(int i=z+K; i<L; i++) {
        switch(cSeqs.seqs[s][i]) {
			case 'a': cM.m[0][0]++; break;
			case 'c': cM.m[1][0]++; break;
			case 'g': cM.m[2][0]++; break;
			case 't': cM.m[3][0]++; break;
		}
    }

    //compute f_i_c
    for(int k=1; k<=K; k++) {
        switch(cSeqs.seqs[s][z+k-1]) {
			case 'a': cM.m[0][k]++; break;
			case 'c': cM.m[1][k]++; break;
			case 'g': cM.m[2][k]++; break;
			case 't': cM.m[3][k]++; break;
		}
    }

	return cM;
}

struct containerM compute_M_pr(float pseudocount, struct containerM cM_pr) {
    for(int k=0; k<=K; k++) {
        float b_sum = 0;

        for(int b=0; b<4; b++) {
            b_sum += abs(cM_pr.m[b][k]) + pseudocount;
        }

        for(int b=0; b<4; b++) {
            cM_pr.m[b][k] = (abs(cM_pr.m[b][k]) + pseudocount)/b_sum;
        }
    }

    return cM_pr;
}

struct containerM compute_M_pr_outof_M_rel(float pseudocount, struct containerM cM_pr) {
    for(int k=0; k<K; k++) {
        float b_sum = 0;

        for(int b=0; b<4; b++) {
            b_sum += abs(cM_pr.m[b][k+1]) + pseudocount;
        }

        for(int b=0; b<4; b++) {
            cM_pr.m[b][k+1] = (abs(cM_pr.m[b][k+1]) + pseudocount)/b_sum;
        }
    }

    return cM_pr;
}

struct containerM compute_M_rel(struct containerM cM_pr) {
    struct containerM cM_rel;

    //copy p_c
    for(int b=0; b<4; b++) {
        cM_rel.m[b][0] = cM_pr.m[b][0];
    }

    //compute M_rel
    for(int k=1; k<=K; k++) {
        for(int b=0; b<4; b++) {
            float p_i_c = cM_pr.m[b][k];
            float p_c = cM_pr.m[b][0];

            cM_rel.m[b][k] = log(p_i_c/p_c);
        }
    }

    return cM_rel;
}

/*	Pr(S generated by M)
	Pr(S|M)
*/
float compute_M_rel_plus_S(int s, int z, struct containerSeqs cSeqs, struct containerM cM_rel) {
    //Pr(S|M)
    float sum = 0;

    for(int k=1; k<=K; k++) {
        int base = getIntFormChar(cSeqs.seqs[s][z+k-1]);

        sum += cM_rel.m[base][k+1];
    }

    return sum;
}

struct containerPrz compute_Prz_matrix(struct containerM cM_rel, struct containerSeqs cSeqs) {
    struct containerPrz cPrz;

    for(int s=0; s<N; s++) {
        float sum = 0;

        //get z_i_j and sum, -> 20131203_115251.jpg
        for(int z=0; z<=L-K; z++) {
            float z_i_j = pow(2, compute_M_rel_plus_S(s, z, cSeqs, cM_rel));
            cPrz.prz[s][z] = z_i_j;

            sum += z_i_j;
        }

        //normalize with respect to sum
        for(int z=0; z<=L-K; z++) {
            float pr_z_i_j = cPrz.prz[s][z] / sum;
            cPrz.prz[s][z] = pr_z_i_j;
        }
    }

    return cPrz;
}

float EM(int s_initial, int z_initial, struct containerSeqs cSeqs, bool display_max) {
    //init max values
    struct containerM cM_pr_max;
    float score_max = 0;

    //compute initial PWM
    struct containerM cM_pr = compute_M_pr(_pseudocounts, compute_f_i_c(s_initial, z_initial, cSeqs));
    struct containerM cM_rel = compute_M_rel(cM_pr);

    //matrix converges @ it 20-25, so 30 iterations should be about to get close enough :]
    for(int it=0; it<30; it++) {
        /*-----E-Step-----
          compute Pr(Z|S, m) = 2^(M_rel (+) S. We will use this later on in the M-Step.
        */
        struct containerPrz cPrz = compute_Prz_matrix(cM_rel, cSeqs);

        /*-----M-Step-----
         compute new f_i_c. First init new M_rel with zeros, then weight M_rel (+) S with the probability
         that the motif occured @ that position.
        */
        struct containerM cM_rel_new;
        for(int k=0; k<=K; k++) {
            for(int b=0; b<4; b++) cM_rel_new.m[b][k] = 0;
        }

        for(int s=0; s<N; s++) {
            for(int z=0; z<=L-K; z++) {
                float arg = cPrz.prz[s][z] * compute_M_rel_plus_S(s, z, cSeqs, cM_rel);

                for(int k=0; k<=K; k++) {
                    int base = getIntFormChar(cSeqs.seqs[s][z+k]);
                    cM_rel_new.m[base][k+1] += arg;
                }
            }
        }
        //as background probability stays the same, just copy it
        for(int b=0; b<4; b++) {
            cM_rel_new.m[b][0] = cM_pr.m[b][0];
        }

        //compute new m_pr
        cM_pr = compute_M_pr_outof_M_rel(_pseudocounts, cM_rel_new);
        //compute new m_rel
        cM_rel_new = compute_M_rel(cM_pr);

        //calculate score of matrix
        float score = 0;
        for(int s=0; s<N; s++) {
            for(int z=0; z<=L-K; z++) {
                score += cPrz.prz[s][z] * compute_M_rel_plus_S(s, z, cSeqs, cM_rel);
            }
        }
        //update score_max if neccessarry
        if(score>score_max) {
            score_max = score;
            cM_pr_max = cM_pr;
        }

        //update m_rel for next iteration
        cM_rel = cM_rel_new;
    }

    if(display_max)
        parseM(cM_pr_max);

    return score_max;
}

/*do the EM several times for different initial PWMs in order to try to find
  the GLOBAL maximum.
  as "initial guess" we compute the M_pr + pseudocounts for a given s and z.
*/
void EM_max(struct containerSeqs cSeqs) {
    float score_max = 0;
    int z_max;

    for(int s=0; s<1; s++) {
        for(int z=0; z<=L-K; z++) {
            float score = EM(s, z, cSeqs, false);
            printf("s=%d, z=%d score_max=%f\n", s, z, score);

            //save maximum
            if(score>score_max) {
                score_max = score;
                z_max=z;
            }
        }
    }

    /*in the end of the day we see that different starting values
      actually do result in different scores, thus in different local maxima.
    */
    printf("----------------------\n");
    printf("overall maximum score:\n");
    printf("s=%d, z=%d score=%f\n", 0, z_max, score_max);
    EM(0, z_max, cSeqs, true);
}

int main(int argc, char *argv[]) {
	FILE *input;
	input = fopen(_FILENAME,"r");
	struct containerSeqs cSeqs;

	fscanf(input,"K=%d N=%d L=%d\n",&K,&N,&L);


	for(int n=0; n<N; n++) {
        char c = fgetc(input);
        int l = 0;

		while(c=='a' || c=='c' || c=='g' || c=='t' || c=='A' || c=='C' || c=='G' || c=='T') {
            cSeqs.seqs[n][l] = c;
            c = fgetc(input);
            l++;
		}
	}

	cSeqs = transformSeqs(cSeqs);
	parseSeqs(cSeqs);

    EM_max(cSeqs);

	return 0;
}
