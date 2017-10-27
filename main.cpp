#include<iostream>
#include <unistd.h>

int main(int argc, char const *argv[]) {
	int pid = getpid();
	unsigned short n_jugador;
	for (n_jugador = 4; n_jugador > 0; --n_jugador) {
		pid = fork();
		if (pid == 0) {
			break;
		}
	}

	std::cout << "Soy el proceso: " << pid << " y el jugador n: " << n_jugador << "\n";
	//printf("Soy el proceso: %d\n", pid);




	return 0;
}