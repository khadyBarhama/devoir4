#include <stdio.h>
#include <pthread.h>
#include <semaphore.h>
#include <stdlib.h>
#include <unistd.h>

#define N 10  // Limite de l'incrémentation et de la décrémentation

int counter = 0;
sem_t sem_1, sem_2;
FILE *output_file;  // Fichier pour sauvegarder les valeurs de counter

// Fonction pour incrémenter
void* increment(void* arg) {
    for (int i = 1; i <= N; i++) {
        sem_wait(&sem_1);
        counter++;
        fprintf(output_file, "%d\n", counter);  // Sauvegarder la valeur de counter
        fflush(output_file);  // S'assurer que les données sont écrites immédiatement
        sem_post(&sem_2);
    }
    sem_post(&sem_2);  // Pour s'assurer que le thread 2 termine
    return NULL;
}

// Fonction pour décrémenter
void* decrement(void* arg) {
    for (int i = 1; i <= N; i++) {
        sem_wait(&sem_2);
        counter--;
        fprintf(output_file, "%d\n", counter);  // Sauvegarder la valeur de counter
        fflush(output_file);  // S'assurer que les données sont écrites immédiatement
        sem_post(&sem_1);
    }
    sem_post(&sem_1);  // Pour s'assurer que le thread 1 termine
    return NULL;
}

int main() {
    pthread_t thread_1, thread_2;

    // Ouvrir le fichier pour écrire les valeurs de counter
    output_file = fopen("data.txt", "w");
    if (output_file == NULL) {
        perror("Erreur d'ouverture du fichier");
        return 1;
    }

    // Initialisation des sémaphores
    sem_init(&sem_1, 0, 1);  // Le sémaphore de thread 1 commence avec 1
    sem_init(&sem_2, 0, 0);  // Le sémaphore de thread 2 commence avec 0

    // Création des threads
    pthread_create(&thread_1, NULL, increment, NULL);
    pthread_create(&thread_2, NULL, decrement, NULL);

    // Attente de la fin des threads
    pthread_join(thread_1, NULL);
    pthread_join(thread_2, NULL);

    // Fermer le fichier après avoir écrit toutes les valeurs
    fclose(output_file);

    // Exécuter gnuplot pour générer la courbe
    FILE *gnuplot = popen("gnuplot -persistent", "w");
    if (gnuplot == NULL) {
        perror("Erreur d'ouverture de gnuplot");
        return 1;
    }

    // Commande gnuplot pour tracer les données
    fprintf(gnuplot, "set title 'Incrémentation et Décrémentation avec Threads'\n");
    fprintf(gnuplot, "set xlabel 'Étapes'\n");
    fprintf(gnuplot, "set ylabel 'Valeur du compteur'\n");
    fprintf(gnuplot, "plot 'data.txt' with lines title 'Counter'\n");

    // Fermer la session gnuplot
    fclose(gnuplot);

    // Destruction des sémaphores
    sem_destroy(&sem_1);
    sem_destroy(&sem_2);

    return 0;
}






