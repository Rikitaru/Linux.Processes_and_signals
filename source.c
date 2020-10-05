#include <stdio.h>
#include <wait.h>
#include <signal.h>
#include <unistd.h>
#include <stdlib.h>

#define PATH "file.txt"
#define RESULT "result.txt"
#define RESULT_2 "result_2.txt"
#define M (1)

pid_t pidOne;
pid_t pidTwo;

int RUN_PROC_2 = 0;
int RUN_PROC_0 = 0;

void runProc_2(int arg) {
	RUN_PROC_2 = 1;
}
void runProc_0(int arg) {
	RUN_PROC_0 = 1;
}

int numericFile(const char *path, const char *res) {

	int c;
	int num = 1;

	FILE *file = fopen(path, "rt");

	if(!file) {
		return -1;
	}
	
	FILE *result = fopen(res, "wt");

	if(!result) {
		return -2;
	}
	
	fprintf(result, "%d. ", num);
	while((c = fgetc(file)) != EOF) {
		fputc(c, result);
		if(c == '\n')
			fprintf(result, "%d. ", ++num);

	}

	fclose(file);
	fclose(result);

}
int deleteString(const char *path, const char *res, int amount) {

	int c;
	int num = 0;

	FILE *file = fopen(path, "rt");

	if(!file) {
		return -1;
	}
	
	FILE *result = fopen(res, "wt");

	if(!result) {
		return -2;
	}
	

	while((c = fgetc(file)) != EOF)
		if(c == '\n') ++num;

	num -= (amount - 1);

	if(num > 0) {
		
		fseek(file, 0, SEEK_SET);

		while(((c = fgetc(file)) != EOF) && (num != 0)) {
			printf("%c", c);
			fputc(c, result);
			if(c == '\n') --num;

		}
	}	

	fclose(file);
	fclose(result);

}

int main(int argc, char* argv[]) {

	int FLAG = 0;

	if((pidOne = fork()) == 0) {
		
		sleep(1);
		numericFile(PATH, RESULT);
		kill(getppid(), SIGUSR1);


	}
	if(pidOne > 0 && ((pidTwo = fork()) == 0)) {

		struct sigaction sa2;
		sa2.sa_handler = runProc_2;
		sigaction(SIGUSR1, &sa2, NULL);

		while(!RUN_PROC_2) sleep(1);

		if(deleteString(RESULT, RESULT_2, M) < 0) printf("Error\n");
		kill(getppid(), SIGUSR2);

	}
	if(pidOne > 0 && pidTwo > 0) {

		int tmp = 0;

		struct sigaction sa0;
		sa0.sa_handler = runProc_0;
		sigaction(SIGUSR1, &sa0, NULL);

		struct sigaction sa0_2;
		sa0_2.sa_handler = runProc_0;
		sigaction(SIGUSR2, &sa0_2, NULL);
		
		while(!RUN_PROC_0) sleep(1);
		
		RUN_PROC_0 = 0;	

		kill(pidTwo, SIGUSR1);

		while(!RUN_PROC_0) sleep(1);

		FILE *file = fopen(RESULT_2, "rt");		

		while((tmp = fgetc(file)) != EOF) fprintf(file, "%c", tmp);

		fclose(file);

	}

	if(pidOne < 0 || pidTwo < 0) {

		printf("Ошибка\n");
		return -1;

	}

	return 0;

}
