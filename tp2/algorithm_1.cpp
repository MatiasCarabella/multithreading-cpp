#include <iostream>
#include <thread>
#include <mutex>
#include <chrono>
#include <random>

// Variable global compartida entre los hilos de tipo 1 y 2
int Compartida = 0;

// Mutex para sincronizar acceso a la variable Compartida
std::mutex mtx;

// Función para el hilo de tipo 1
void HiloTipo1(int id) {
    // Mostrar mensaje
    std::cout << "Instancia " << id << " del hilo 1" << std::endl;

    // Generar un tiempo de suspensión aleatorio entre 0 y 2 segundos
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> dis(0, 2000);
    int sleep_time = dis(gen);
    std::this_thread::sleep_for(std::chrono::milliseconds(sleep_time));

    // Incrementar la variable global Compartida
    mtx.lock();
    Compartida++;
    mtx.unlock();
}

// Función para el hilo de tipo 2
void HiloTipo2(int id) {
    // Mostrar mensaje
    std::cout << "Instancia " << id << " del hilo 2" << std::endl;

    // Generar un tiempo de suspensión aleatorio entre 0 y 2 segundos
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> dis(0, 2000);
    int sleep_time = dis(gen);
    std::this_thread::sleep_for(std::chrono::milliseconds(sleep_time));

    // Leer y mostrar el valor de la variable global Compartida
    mtx.lock();
    std::cout << "Valor de la variable Compartida: " << Compartida << std::endl;
    mtx.unlock();
}

int main() {
    int N, M;

    // Pedir al usuario que ingrese los valores de N y M
    std::cout << "Ingrese el número de hilos de tipo 1 (N): ";
    std::cin >> N;

    std::cout << "Ingrese el número de hilos de tipo 2 (M): ";
    std::cin >> M;

    // Arreglos para almacenar los hilos
    std::thread hilosTipo1[N];
    std::thread hilosTipo2[M];

    // Crear hilos de tipo 1
    for (int i = 0; i < N; ++i) {
        hilosTipo1[i] = std::thread(HiloTipo1, i);
    }

    // Crear hilos de tipo 2
    for (int i = 0; i < M; ++i) {
        hilosTipo2[i] = std::thread(HiloTipo2, i);
    }

    // Esperar a que todos los hilos terminen su ejecución
    for (int i = 0; i < N; ++i) {
        hilosTipo1[i].join();
    }

    for (int i = 0; i < M; ++i) {
        hilosTipo2[i].join();
    }

    // Mensaje de finalización
    std::cout << "Se ha finalizado la ejecución." << std::endl;

    return 0;
}
