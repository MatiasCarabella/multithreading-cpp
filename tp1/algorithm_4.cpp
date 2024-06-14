#include <iostream>
#include <thread>
#include <string>
#include <chrono>
#include <random> // Para la generación de números aleatorios

using namespace std;

// Clase que encapsula el comportamiento de un proceso
class Proceso {
private:
    int id;
    int demora;
    int veces;

public:
    // Constructor para inicializar los datos del proceso
    Proceso(int id, int demora, int veces) 
        : id(id), demora(demora), veces(veces) {}

    // Método que representa el comportamiento del proceso
    void ejecutar() {
        for (int i = 0; i < veces; i++) {
            cout << "Soy el proceso " << id << "\n";
            this_thread::sleep_for(chrono::milliseconds(demora));
        }
    }
};

int main() {
    thread P[15];  // Array para almacenar los hilos
    Proceso* procesos[15];  // Array para almacenar las instancias de los procesos

    random_device rd; // Generador de números aleatorios
    mt19937 gen(rd());  // Motor Mersenne Twister para la generación de números aleatorios
    uniform_int_distribution<> dist_demora(100, 1000); // Distribución uniforme para la demora (100 a 1000 ms)
    uniform_int_distribution<> dist_veces(5, 20);      // Distribución uniforme para el número de veces (5 a 20)

    // Crear y lanzar los hilos
    for (int i = 0; i < 15; i++) {
        int demora = dist_demora(gen);  // Generar una demora aleatoria
        int veces = dist_veces(gen);    // Generar un número aleatorio de veces
        procesos[i] = new Proceso(i + 1, demora, veces);  // Crear una nueva instancia del proceso
        P[i] = thread(&Proceso::ejecutar, procesos[i]);   // Lanzar el método 'ejecutar' en un nuevo hilo
    }

    // Esperar a que todos los hilos terminen
    for (int i = 0; i < 15; i++) {
        P[i].join();
        delete procesos[i];  // Liberar la memoria de cada instancia del proceso
    }

    // Imprimir mensaje final una vez que todos los hilos han terminado
    cout << "Fin \n";
    return 0;
}
