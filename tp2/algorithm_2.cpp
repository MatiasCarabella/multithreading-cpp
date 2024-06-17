#include <iostream>
#include <unistd.h>
#include <sys/wait.h>
#include <cstdlib>
#include <ctime>

using namespace std;

// Variable 'global'
int Compartida = 0;

void procesoTipo1(int id) {
    try {
        cout << "Instancia " << id << " del proceso 1" << endl;

        // Generar un tiempo de espera aleatorio entre 0 y 2 segundos
        int sleep_time = rand() % 2001; // rand() % 2001 genera un número entre 0 y 2000
        usleep(sleep_time * 1000); // usleep espera en microsegundos, por lo tanto multiplicamos por 1000 para convertir milisegundos a microsegundos

        // Incrementar la variable global Compartida
        Compartida++;

        // Salir del proceso tipo 1
        exit(0);
    } catch (const exception& e) {
        cerr << "Excepción en procesoTipo1, id: " << id << ": " << e.what() << endl;
    }
}

void procesoTipo2(int id) {
    try {
        cout << "Instancia " << id << " del proceso 2" << endl;

        // Generar un tiempo de espera aleatorio entre 0 y 2 segundos
        int sleep_time = rand() % 2001; // rand() % 2001 genera un número entre 0 y 2000
        usleep(sleep_time * 1000); // usleep espera en microsegundos, por lo tanto multiplicamos por 1000 para convertir milisegundos a microsegundos

        // Leer y mostrar el valor de la variable global Compartida
        cout << "Valor de la variable Compartida: " << Compartida << endl;

        // Salir del proceso tipo 2
        exit(0);
    } catch (const exception& e) {
        cerr << "Excepción en procesoTipo2, id: " << id << ": " << e.what() << endl;
    }
}

int main() {
    int N, M;

    try {
        // Inicializar la semilla para la generación de números aleatorios
        srand(static_cast<unsigned int>(time(0)));

        // Pedir al usuario que ingrese los valores de N y M
        cout << "Ingrese el número de procesos de tipo 1 (N): ";
        cin >> N;
        if (cin.fail()) {
            throw invalid_argument("Entrada inválida. Por favor ingrese números enteros.");
        }

        cout << "Ingrese el número de procesos de tipo 2 (M): ";
        cin >> M;
        if (cin.fail()) {
            throw invalid_argument("Entrada inválida. Por favor ingrese números enteros.");
        }

        // Crear N procesos de tipo 1
        for (int i = 0; i < N; ++i) {
            pid_t pid = fork();
            if (pid == 0) { // Proceso hijo
                procesoTipo1(i);
            } else if (pid < 0) { // Error al crear el proceso
                cerr << "Error al crear proceso tipo 1, id: " << i << endl;
            }
        }

        // Crear M procesos de tipo 2
        for (int i = 0; i < M; ++i) {
            pid_t pid = fork();
            if (pid == 0) { // Proceso hijo
                procesoTipo2(i);
            } else if (pid < 0) { // Error al crear el proceso
                cerr << "Error al crear proceso tipo 2, id: " << i << endl;
            }
        }

        // Esperar a que todos los procesos hijos terminen su ejecución
        int status;
        while (wait(&status) > 0); // Esperar a que todos los hijos terminen

        // Mensaje de finalización de la ejecución
        cout << "Se ha finalizado la ejecución." << endl;

    } catch (const exception& e) {
        // Capturar y mostrar cualquier excepción ocurrida en la función principal
        cerr << "Excepción en la función principal: " << e.what() << endl;
    }

    return 0;
}
