//
// Created by coman on 1/16/2025.
//

#include "common.h"
#include "functions.h"



// Functia mySystem() preia comanda si o salveaza intr-un buffer in memorie, pentru a o trimite mai
// departe la functia care executa comenzi
int mySystem(const char* cmd) {
    // verificam daca lungimea comenzii preluate este 0 sau daca e null
    if(strlen(cmd) == 0 || (cmd == NULL)) return -1;

    // salvam comanda intr-un buffer de lungime maxima
    char buff_comanda[LG_MAX_COMANDA];
    strcpy(buff_comanda,cmd);

    // functia va interpreta si executa comanda transmisa
    executa(buff_comanda);

    // comanda a fost transmisa cu succes mai departe
    return 0;
}

// Functie care cauta in fisier combinatia user-parola primite ca parametri
int login(const char* user, const char* parola) {
    int fd; // file descriptor (-1 daca fisierul nu a fost deschis corect, 0 altfel)
    char line[LG_MAX_UTILIZATOR]; // linia citita din fisier
    const char *file_user, *file_pass;

    // deschidem fisierul cu parole, verificand eventualele erori
    fd = open(FILEINPUT, O_RDONLY);
    if (fd < 0) {
        perror("Eroare la deschiderea fisierului passwords.txt");
        return -1;
    }

    ssize_t bytes_read;  // numarul de bytes cititi din fisier, ne va ajuta pentru conditia de la while
    size_t i = 0; // numarul caracterului

    // citim din fisier, caracter cu caracter
    while ((bytes_read = read(fd, &line[i], 1)) > 0) {
        // daca ultimul caracter citit este '\n' sau i ajunge deja la limita maxima a unei linii,
        // prelucram datele
        if (line[i] == '\n' || i == LG_MAX_UTILIZATOR - 1) {
            line[i] = '\0'; // terminam linia

            // impartim linia in user si parola
            file_user = strtok(line, " ");
            file_pass = strtok(NULL, " ");

            // verificam daca user-ul si parola coincid
            if (file_user && file_pass && strcmp(user, file_user) == 0 && strcmp(parola, file_pass) == 0) {
                close(fd); // inchidem file descriptorul
                return 1; // login reusit
            }
            i = 0; // resetam indexul pentru linia urmatoare
        } else {
            i++; // incrementam "i" ca sa citim fiecare caracter de pe linie
        }
    }

    // inchidem fisierul
    close(fd);
    return 0; // login esuat
}

void executa(char *input) {
    // variabila va contine numele comenzii
    char *command = strtok(input, " ");

    // daca comanda este null, vom iesi din functie
    if (!command) return;

    // verificam ce comanda a fost introdusa
    if (strcmp(command, "tee") == 0) {
        // verificam daca a fost introdusa optiunea -a
        char *option = strtok(NULL, " ");
        int append_mode = 0;

        // modificam modul in functie de existenta optiunii -a
        if (option && strcmp(option, "-a") == 0) {
            append_mode = 1;
            option = strtok(NULL, " ");
        }

        // salvam numele fisierului:
        // daca append mode exista, atunci option contine numele fisierului
        // daca append mode nu exista, atunci avansam si citim numele fisierului
        // IMPORTANT: daca nu exista modul append, atunci numele fisierului va fi in option,
        //            dar daca exista modul append si option e null, se va avansa si se va citi urmatorul sir de caractere
        char *filename = append_mode ? option : option ? option : strtok(NULL, " ");

        // verificam daca a fost gasit un fisier
        if (!filename) {
            write(STDOUT_FILENO, "Folosire: tee [-a] <fisier>\n", 29);
            return;
        }
        // apelam functia pentru tee
        handle_tee(append_mode, filename);
    } else if(strcmp(command,"find") == 0) {
        char *path, *option1, *value1, *option2, *value2;

        path = strtok(NULL, " ");
        option1 = strtok(NULL, " ");
        value1 = strtok(NULL, " ");
        option2 = strtok(NULL, " ");
        value2 = strtok(NULL, " ");

        // Verifica daca toate argumentele sunt furnizate
        if (path == NULL || option1 == NULL || value1 == NULL) {
            const char *syntax_error_msg = "Sintaxa invalida. Foloseste: find <path> <option1> <value1> [<option2> <value2>]\n";
            write(STDOUT_FILENO, syntax_error_msg, strlen(syntax_error_msg));
            return ;
        }

        // Proceseaza opțiunile
        const char *name = NULL;
        const char *type = NULL;

        if (strcmp(option1, "-name") == 0) {
            name = value1;
        } else if (strcmp(option1, "-type") == 0) {
            type = value1;
        }

        if (option2 && value2) {
            if (strcmp(option2, "-name") == 0) {
                name = value2;
            } else if (strcmp(option2, "-type") == 0) {
                type = value2;
            }
        }

        // Apeleaza funcția find_command
        find_command(path, name, type);
    } else if(strcmp(command, "watch") == 0) {
        char *optiune = strtok(NULL, " ");
        char *interval = strtok(NULL, " ");
        char *cmd = strtok(NULL, "");

        if(optiune && strcmp(optiune, "-n") == 0 && interval && (cmd == NULL))
            watch_command(interval, NULL);
        if (optiune && strcmp(optiune, "-n") == 0 && interval && cmd) {
            watch_command(interval, cmd);
        }else {
            const char *syntax_error_msg = "Sintaxa invalida pentru watch. Foloseste: watch -n <interval> <comanda>\n";
            write(STDOUT_FILENO, syntax_error_msg, strlen(syntax_error_msg));
        }
    } else if(strcmp(command, "chmod") == 0) {
        char *args = input + 6;
        char *mode_str = strtok(args, " ");
        char *file = strtok(NULL, " ");

        if (mode_str && file) {
            handle_chmod(mode_str, file);
        } else {
            write(STDOUT_FILENO, "Folosire: chmod <codPerm> <fisier>\n", 36);
        }
    }
    else {
        write(STDOUT_FILENO, "Comanda necunoscuta\n", 20);
    }
}