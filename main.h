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

void parsePWM(struct containerPWM cPWM) {
	printf("\nPWM:\n");
    printf("   A    C    G    T\n");

    for(int k=0; k<=K; k++) {
        printf("%d |", k);

        for(int b=0; b<4; b++) {
			printf("%.2f ", cPWM.pwm[b][k]);
		}

		printf("\n");
	}
}

void parsePrz(struct containerPrz cPrz) {
    printf("K=%d, N=%d, L=%d\n", K, N, L);

	printf("\nProbability Matrix:\n");

	for(int n=0; n<N; n++) {
        printf("-------------\nn=%d\n", n);

		for(int z=0; z<L-K; z++) {
			printf("%f ", cPrz.prz[n][z]);
		}
		printf("\n");
	}
}
