#include <stdio.h>
#include <unistd.h>

int main() {
    printf("gas : hello\n");
    printf("--------------------\n");
	printf("nic : ");
    sleep(1); // Sleep for a short duration to visualize the effect

	int pid = fork();

	if ( pid == 0 ) {
		for (int i = 1; i <= 100; ++i) {
			printf("\033[F"); // ANSI escape code for moving the cursor up
			printf("gas : Message %d\n", i);
			printf("--------------------");
			sleep(1);
		}
	} else {
		while (1) {
			char message[100];
			printf("--------------------\n");
			printf("nic : ");
			scanf("%s",message);
		}
	}

    // Move the cursor up two lines and print a new message
    printf("nic : nouveau message\n");
	printf("--------------------\n");
	printf("nic : ");

    return 0;
}
