#include <iostream> // Para entrada/salida estándar (cout, cin, cerr)
#include <vector> // Para usar contenedores vector
#include <cstdlib> // Para funciones de generación de números aleatorios (rand, srand)
#include <unistd.h> // Para funciones POSIX como fork(), pipe(), close()
#include <sys/wait.h> // Para la función waitpid(), usada para esperar a los procesos hijos
#include <ctime> // Para la función time(), usada para inicializar la semilla aleatoria
#include <limits> // Para numeric_limits, usado en la validación de entrada
#include <iomanip> // Para manipular la salida formateada (setw, setprecision)

using namespace std;

// Constantes para los extremos de los pipes
const int READ_END = 0;
const int WRITE_END = 1;

// Struct para mapear los atributos de cualquier jugador
struct Jugador {
    int id;         // Identificador único del jugador
    float puntos;   // Puntuación actual
    bool plantado;  // Flag que indica si se plantó
    bool abandonado; // Flag que indica si abandonó
};

// Proceso jugador
void jugador(int id, int pipe_lectura, int pipe_escritura) {
    srand(time(NULL) + id);  // Inicializar semilla aleatoria única para cada jugador
    float puntos = 0;
    bool plantado = false;

    while (!plantado && puntos <= 7.5) {
        // Esperar carta del repartidor
        float carta;
        read(pipe_lectura, &carta, sizeof(float));

        puntos += carta;

        int decision;
        if (puntos > 7.5) {
            // Si se pasa de 7.5, abandona automáticamente
            decision = 2; // 2: abandonar
        } else {
            // Decidir aleatoriamente entre plantarse (1) o pedir carta (0)
            decision = rand() % 2;
        }

        if (decision == 1) {
            plantado = true;
        }

        // Enviar decisión al repartidor
        write(pipe_escritura, &decision, sizeof(int));

        if (decision == 2) break;  // Abandonar
    }

    // Cerrar pipes al finalizar
    close(pipe_lectura);
    close(pipe_escritura);
}

// Proceso orquestador del juego
void iniciar_juego(int num_jugadores, vector<int> pipes_lectura, vector<int> pipes_escritura) {
    vector<Jugador> jugadores(num_jugadores);
    // Cartas simplificadas: 1-7 y tres 0.5 (8, 9 y 10)
    vector<float> baraja = {1, 2, 3, 4, 5, 6, 7, 0.5, 0.5, 0.5};  
    srand(time(NULL));  // Inicializar semilla aleatoria para el repartidor

    // Inicializar jugadores
    for (int i = 0; i < num_jugadores; ++i) {
        jugadores[i] = {i, 0, false, false};
    }

    bool juego_terminado = false;
    while (!juego_terminado) {
        for (int i = 0; i < num_jugadores; ++i) {
            if (!jugadores[i].plantado && !jugadores[i].abandonado) {
                // Repartir carta
                float carta = baraja[rand() % baraja.size()];
                write(pipes_escritura[i], &carta, sizeof(float));

                // Recibir decisión del jugador
                int decision;
                read(pipes_lectura[i], &decision, sizeof(int));

                jugadores[i].puntos += carta;

                // Actualizar estado del jugador según su decisión
                if (decision == 1) {
                    jugadores[i].plantado = true;
                } else if (decision == 2) {
                    jugadores[i].abandonado = true;
                }
            }
        }

        // Verificar si el juego ha terminado
        juego_terminado = true;
        for (const auto& jugador : jugadores) {
            if (!jugador.plantado && !jugador.abandonado) {
                juego_terminado = false;
                break;
            }
        }
    }

    // Determinar ganador y mostrar resultados
    float mejor_puntuacion = -1;
    int ganador_id = -1;

    // Imprimir tabla de resultados
    cout << "\nResultados del juego:\n";
    cout << "-----------------------------\n";
    cout << "Jugador | Puntos | Estado\n";
    cout << "-----------------------------\n";

    for (const auto& jugador : jugadores) {
        cout << setw(7) << jugador.id << " | " 
                  << setw(6) << fixed << setprecision(1) << jugador.puntos << " | ";

        if (jugador.abandonado) {
            cout << "Abandonó";
        } else {
            cout << "Se plantó";
            // Actualizar ganador si es necesario
            if (jugador.puntos > mejor_puntuacion) {
                mejor_puntuacion = jugador.puntos;
                ganador_id = jugador.id;
            }
        }
        cout << endl;
    }

    cout << "-----------------------------\n";

    // Anunciar ganador
    if (ganador_id != -1) {
        cout << "\n¡El ganador es el Jugador " << ganador_id 
                  << " con " << mejor_puntuacion << " puntos!" << endl;
    } else {
        cout << "\nNo hay ganador. Todos los jugadores se pasaron o abandonaron." << endl;
    }

    // Cerrar pipes
    for (int pipe : pipes_lectura) close(pipe);
    for (int pipe : pipes_escritura) close(pipe);
}

int main() {
    int num_jugadores;
    
    // Solicitar el número de jugadores por consola
    do {
        cout << "Ingrese el número de jugadores (2-10): ";
        cin >> num_jugadores;
        
        // Manejar entrada inválida
        if (cin.fail()) {
            cin.clear(); // Limpiar el estado de error
            cin.ignore(numeric_limits<streamsize>::max(), '\n'); // Descartar la entrada inválida
            num_jugadores = 0; // Asegurarse de que el bucle continúe
        }
        
        if (num_jugadores < 2 || num_jugadores > 10) {
            cout << "Número de jugadores no válido. Por favor, ingrese un número entre 2 y 10." << endl;
        }
    } while (num_jugadores < 2 || num_jugadores > 10);

    // Vectores para almacenar los pipes de comunicación
    vector<int> pipes_jugadores_a_repartidor[2];
    vector<int> pipes_repartidor_a_jugadores[2];
    vector<pid_t> pids_jugadores;

    // Crear pipes y procesos jugadores
    for (int i = 0; i < num_jugadores; ++i) {
        int pipe_j_to_r[2];
        int pipe_r_to_j[2];
        
        // Crear pipes
        if (pipe(pipe_j_to_r) == -1 || pipe(pipe_r_to_j) == -1) {
            cerr << "Error al crear pipes" << endl;
            return 1;
        }

        // Almacenar descriptores de pipes
        pipes_jugadores_a_repartidor[READ_END].push_back(pipe_j_to_r[READ_END]);
        pipes_jugadores_a_repartidor[WRITE_END].push_back(pipe_j_to_r[WRITE_END]);
        pipes_repartidor_a_jugadores[READ_END].push_back(pipe_r_to_j[READ_END]);
        pipes_repartidor_a_jugadores[WRITE_END].push_back(pipe_r_to_j[WRITE_END]);

        // Crear proceso hijo (jugador)
        pid_t pid = fork();

        if (pid == -1) {
            cerr << "Error al crear proceso hijo" << endl;
            return 1;
        } else if (pid == 0) {  // Proceso hijo (jugador)
            close(pipe_j_to_r[READ_END]);
            close(pipe_r_to_j[WRITE_END]);
            jugador(i, pipe_r_to_j[READ_END], pipe_j_to_r[WRITE_END]);
            return 0;
        } else {  // Proceso padre
            close(pipe_j_to_r[WRITE_END]);
            close(pipe_r_to_j[READ_END]);
            pids_jugadores.push_back(pid);
        }
    }

    // Proceso repartidor (padre)
    iniciar_juego(num_jugadores, pipes_jugadores_a_repartidor[READ_END], pipes_repartidor_a_jugadores[WRITE_END]);

    // Esperar a que terminen todos los procesos hijos
    for (pid_t pid : pids_jugadores) {
        waitpid(pid, NULL, 0);
    }

    return 0;
}