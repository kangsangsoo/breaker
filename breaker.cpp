#pragma warning(disable:4996)
#include <stdio.h>
#include <math.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>
#define ALPHABET 26
#define MX_LENGTH 1200
#define ITERATION 500
#define PRE 3
#define MX_SCORE -30000

void hill_climbing(void);
double calc_fitness(void);
void generate_random_key(void);
void decrypt(void);
void init(void);
void read_ci(void);
void calc_quadgrams(void);

char text_ci[MX_LENGTH];
char text_pl[MX_LENGTH];
char key_parent[ALPHABET];
char key_current[ALPHABET];
char key_answer[ALPHABET];
char key_pre[PRE][ALPHABET];

double score_parent;
double score_max = MX_SCORE; // fitenss function¿¡ µû¶ó ¹Ù²ñ
double score_pre[PRE];
double quadgrams[ALPHABET][ALPHABET][ALPHABET][ALPHABET];

int main(void) {
	init();
	hill_climbing();

	int len = strlen(text_ci);
	for (int g = 0; g < PRE; g++) {
		printf("-------------- #%d result ------------\n", g);

		for (int i = 0; i < len; i++) {
			text_pl[i] = key_pre[g][text_ci[i] - 'A'];
		}
		for (int i = 0; i < strlen(text_ci); i++) {
			printf("%c", text_pl[i] + 'A');
		}

		printf("\nscore : %f", score_pre[g]);
		printf("\nencrypt_key\n");

		int real_key[ALPHABET] = { 0 };
		for (int i = 0; i < ALPHABET; i++) {
			printf("%c", 'a' + i);
			real_key[key_pre[g][i]] = i;
		}
		printf("\n");
		for (int i = 0; i < ALPHABET; i++) {
			printf("%c", real_key[i] + 'a');
		}
		printf("\n\n");
	}

	return 0;
}

void hill_climbing(void) {
	double score_current;

	for (int l = 0; l < ITERATION; l++) {
		generate_random_key();
		score_current = score_parent = MX_SCORE; // fitenss function¿¡ µû¶ó ¹Ù²ñ
		for (int j = 0; j < ALPHABET; j++) {
			for (int i = j + 1; i < ALPHABET; i++) {
				key_current[j] = key_parent[i];
				key_current[i] = key_parent[j];

				decrypt();

				score_current = calc_fitness();
				if (score_current > score_parent) {  // fitenss function¿¡ µû¶ó ¹Ù²ñ
					score_parent = score_current;
					for (int k = 0; k < ALPHABET; k++) {
						key_parent[k] = key_current[k];
					}
					if (score_max <= score_parent) {  // fitenss function¿¡ µû¶ó ¹Ù²ñ
						for (int k = 1; k < PRE; k++) {
							for (int k1 = 0; k1 < ALPHABET; k1++) {
								key_pre[k][k1] = key_pre[k - 1][k1];
							}
							score_pre[k] = score_pre[k - 1];
						}

						for (int k = 0; k < ALPHABET; k++) {
							key_answer[k] = key_current[k];
							key_pre[0][k] = key_current[k];
						}
						score_pre[0] = score_current;
						score_max = score_parent;
					}
					j = -1;
					break;
				}
				key_current[j] = key_parent[j];
				key_current[i] = key_parent[i];
			}
		}
	}
}

double calc_fitness(void) {
	int len = strlen(text_ci);
	double res = 0;

	for (int i = 3; i < len; i++) {
		res += quadgrams[text_pl[i - 3]][text_pl[i - 2]][text_pl[i - 1]][text_pl[i]];
	}

	return res;
}

void generate_random_key(void) {
	int chk[ALPHABET] = { 0 };
	srand(time(NULL));
	for (int i = 0; i < ALPHABET; i++) {
		int k = rand() % 26;
		if (chk[k] != 0) {
			i--;
			continue;
		}
		key_parent[i] = key_current[i] = k;
		chk[k] = 1;
	}
}

void decrypt(void) {
	int len = strlen(text_ci);
	for (int i = 0; i < len; i++) {
		text_pl[i] = key_current[text_ci[i] - 'A'];
	}
}

void init(void) {
	calc_quadgrams();
	read_ci();
}

void read_ci(void) {
	FILE* fp = fopen("ciphertext.txt", "rt");
	int orig_ci_freq[ALPHABET] = { 0 };

	char ch;
	int i = 0;
	while (1) {
		ch = fgetc(fp);
		if (ch == -1) break;
		if (ch >= 'A' && ch <= 'Z') {
			text_ci[i] = ch;
			orig_ci_freq[ch - 'A']++;
			i++;

		}
	}
}

void calc_quadgrams(void) {
	FILE* fp = fopen("quadgrams.txt", "rt");

	char ch[4];
	int num;
	long long sum = 0;
	int i = 0;
	while (1) {
		fscanf(fp, "%c%c%c%c %d", ch, ch+1, ch+2, ch+3, &num);
		sum += quadgrams[ch[0] - 'A'][ch[1] - 'A'][ch[2] - 'A'][ch[3] - 'A'] = num;
		if (fgetc(fp) == -1) break;
		i++;
	}

	for (int a = 0; a < ALPHABET; a++) {
		for (int b = 0; b < ALPHABET; b++) {
			for (int c = 0; c < ALPHABET; c++) {
				for (int d = 0; d < ALPHABET; d++) {
					if(quadgrams[a][b][c][d] == 0) quadgrams[a][b][c][d] = logl(0.01/sum);
					else quadgrams[a][b][c][d] = logl(quadgrams[a][b][c][d] / sum);  
					if (isnan(quadgrams[a][b][c][d])) {
						printf("error");
						exit(0);
					}
				}
			}
		}
	}

	fclose(fp);
}

/* -------------------------------------------------------------------------------------------------------- 

double pl_quadgrams[ALPHABET][ALPHABET][ALPHABET][ALPHABET] = { 0 };
double monogram[ALPHABET];
double trigrams[ALPHABET][ALPHABET][ALPHABET];
double bigrams[ALPHABET][ALPHABET];
int monogram_ord[ALPHABET] = { 4, 19, 0, 14, 8, 13, 18, 7, 17, 3, 11, 2, 20, 12, 22, 5, 6, 24, 15, 1, 21, 10, 9, 23, 16, 25 };


void calc_monogram(void) {
	FILE* fp = fopen("monogram.txt", "rt");
	char ch;
	int num;
	long long sum = 0;
	while (1) {
		fscanf(fp, "%c %d", &ch, &num);
		sum += monogram[ch - 'A'] = num;
		if (fgetc(fp) == -1) break;
	}

	for (int i = 0; i < ALPHABET; i++) {
		monogram[i] = (monogram[i] / sum);

	}

	fclose(fp);
}

void calc_bigrams(void) {
	FILE* fp = fopen("bigrams.txt", "rt");

	char ch[4];
	int num;
	long long sum = 0;

	while (1) {
		fscanf(fp, "%c%c %d", ch, ch + 1, &num);
		sum += bigrams[ch[0] - 'A'][ch[1] - 'A'] = num;
		if (fgetc(fp) == -1) break;
	}

	for (int a = 0; a < ALPHABET; a++) {
		for (int b = 0; b < ALPHABET; b++) {
			bigrams[a][b] = (bigrams[a][b] / sum);
		}
	}

	fclose(fp);
}

void calc_trigrams(void) {
	FILE* fp = fopen("trigrams.txt", "rt");

	char ch[4];
	int num;
	long long sum = 0;

	while (1) {
		fscanf(fp, "%c%c%c %d", ch, ch + 1, ch + 2, &num);
		sum += trigrams[ch[0] - 'A'][ch[1] - 'A'][ch[2] - 'A'] = num;
		if (fgetc(fp) == -1) break;
	}

	for (int a = 0; a < ALPHABET; a++) {
		for (int b = 0; b < ALPHABET; b++) {
			for (int c = 0; c < ALPHABET; c++) {
				if (trigrams[a][b][c] == 0) trigrams[a][b][c] = logl(0.01 / sum);
				else trigrams[a][b][c] = logl(trigrams[a][b][c] / sum);
				if (isnan(trigrams[a][b][c])) {
					printf("error");
				}
			}
		}
	}

	fclose(fp);
}
*/