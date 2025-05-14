#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include "common.h"
#include "functions.h"

#define MAX_INPUT 1024

int main() {
    char user[50], parola[50], prompt[100];
    ssize_t bytes_read;

    // solicitam numele utilizatorului
    write(STDOUT_FILENO, "Introduceti numele de utilizator: ", 34);
    bytes_read = read(STDIN_FILENO, user, sizeof(user) - 1);
    if (bytes_read <= 0) return 1;
    user[bytes_read - 1] = '\0'; // eliminam '\n'

    // solicitam parola
    write(STDOUT_FILENO, "Introduceti parola: ", 20);
    bytes_read = read(STDIN_FILENO, parola, sizeof(parola) - 1);
    if (bytes_read <= 0) return 1;
    parola[bytes_read - 1] = '\0'; // eliminam '\n'

    // verificam autentificarea
    if (login(user, parola) != 1) {
        write(STDOUT_FILENO, "Autentificare esuata!\n", 22);
        return 1;
    }
    write(STDOUT_FILENO, "Autentificare reusita!\n", 23);

    // bucla principala
    char input[MAX_INPUT];
    while (1) {
        // construim promptul manual
        strcpy(prompt, user);
        strcat(prompt, "@bash > \0");

        // afisam promptul
        write(STDOUT_FILENO, prompt, strlen(prompt));

        // citim comanda de la utilizator
        bytes_read = read(STDIN_FILENO, input, sizeof(input) - 1);
        if (bytes_read <= 0) break;

        input[bytes_read - 1] = '\0'; // eliminam '\n'

        // daca utilizatorul introduce "exit", iesim din bucla
        if (strcmp(input, "exit") == 0) {
            write(STDOUT_FILENO, "La revedere!\n", 13);
            break;
        }

        // transmitem comanda catre mySystem
        mySystem(input);
    }

    return 0;
}

