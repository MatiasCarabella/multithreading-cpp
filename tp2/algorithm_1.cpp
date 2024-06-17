#include <iostream>
#include <thread>
#include <mutex>
#include <chrono>
#include <cstdlib>
#include <ctime>

using namespace std; // Para evitar el uso de std:: constantemente

// Variable global compartida entre los hilos de tipo 1 y 2
int Compartida = 0;

// Mutex para sincronizar acceso a la variable Compartida
mutex mtx;

// Función para los hilos de tipo 1
void hiloTipo1(int id) {
    try {
        // Mostrar mensaje identificando la instancia del hilo
        cout << "Instancia " << id << " del hilo 1" << endl;

        // Generar un tiempo de suspensión aleatorio entre 0 y 2 segundos
        int sleep_time = rand() % 2001; // rand() % 2001 genera un número entre 0 y 2000
        this_thread::sleep_for(chrono::milliseconds(sleep_time));

        // Incrementar la variable global Compartida
        lock_guard<mutex> guard(mtx); // Utiliza lock_guard para evitar deadlocks en caso de excepción
        Compartida++;
    } catch (const exception& e) {
        // Capturar y mostrar cualquier excepción ocurrida durante la ejecución del hilo
        cerr << "Excepción en hiloTipo1, id: " << id << ": " << e.what() << endl;
    }
}

// Función para los hilos de tipo 2
void hiloTipo2(int id) {
    try {
        // Mostrar mensaje identificando la instancia del hilo
        cout << "Instancia " << id << " del hilo 2" << endl;

        // Generar un tiempo de suspensión aleatorio entre 0 y 2 segundos
        int sleep_time = rand() % 2001; // rand() % 2001 genera un número entre 0 y 2000
        this_thread::sleep_for(chrono::milliseconds(sleep_time));

        // Leer y mostrar el valor de la variable global Compartida
        lock_guard<mutex> guard(mtx); // Utiliza lock_guard para evitar deadlocks en caso de excepción
        cout << "Valor de la variable Compartida: " << Compartida << endl;
    } catch (const exception& e) {
        // Capturar y mostrar cualquier excepción ocurrida durante la ejecución del hilo
        cerr << "Excepción en hiloTipo2, id: " << id << ": " << e.what() << endl;
    }
}

int main() {
    int N, M;

    try {
        // Inicializar la semilla para la generación de números aleatorios
        srand(static_cast<unsigned int>(time(0)));

        // Pedir al usuario que ingrese los valores de N y M
        cout << "Ingrese el número de hilos de tipo 1 (N): ";
        cin >> N;
        if (cin.fail()) {
            throw invalid_argument("Entrada inválida. Por favor ingrese números enteros.");
        }

        cout << "Ingrese el número de hilos de tipo 2 (M): ";
        cin >> M;
        if (cin.fail()) {
            throw invalid_argument("Entrada inválida. Por favor ingrese números enteros.");
        }

        // Arreglos para almacenar los hilos
        thread* hilosTipo1 = new thread[N];
        thread* hilosTipo2 = new thread[M];

        // Crear hilos de tipo 1
        for (int i = 0; i < N; ++i) {
            try {
                hilosTipo1[i] = thread(hiloTipo1, i);
            } catch (const exception& e) {
                // Capturar y mostrar cualquier excepción ocurrida durante la creación del hilo
                cerr << "Excepción al crear hilo de tipo 1, id: " << i << ": " << e.what() << endl;
            }
        }

        // Crear hilos de tipo 2
        for (int i = 0; i < M; ++i) {
            try {
                hilosTipo2[i] = thread(hiloTipo2, i);
            } catch (const exception& e) {
                // Capturar y mostrar cualquier excepción ocurrida durante la creación del hilo
                cerr << "Excepción al crear hilo de tipo 2, id: " << i << ": " << e.what() << endl;
            }
        }

        // Esperar a que todos los hilos de tipo 1 terminen su ejecución
        for (int i = 0; i < N; ++i) {
            if (hilosTipo1[i].joinable()) {
                hilosTipo1[i].join();
            }
        }

        // Esperar a que todos los hilos de tipo 2 terminen su ejecución
        for (int i = 0; i < M; ++i) {
            if (hilosTipo2[i].joinable()) {
                hilosTipo2[i].join();
            }
        }

        // Liberar memoria asignada dinámicamente para los hilos
        delete[] hilosTipo1;
        delete[] hilosTipo2;

        // Mensaje de finalización de la ejecución
        cout << "Se ha finalizado la ejecución." << endl;

    } catch (const exception& e) {
        // Capturar y mostrar cualquier excepción ocurrida en la función principal
        cerr << "Excepción en la función principal: " << e.what() << endl;
    }

    return 0;
}
