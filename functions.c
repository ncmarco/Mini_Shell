//
// Created by coman on 1/16/2025.
//

#include "functions.h"

#include "common.h"


void handle_tee(int append_mode, char *filename) {
    // specifica optiunile de deschidere ale fisierului:
    // O_WRONLY - in modul write only
    // O_CREAT - fisierul va fi creat daca nu exista cu permisiunile 0644 - permisiuni de scriere si citire pentru utilizator
    // O_APPEND - va deschide fisierul in modul append, adica se va adauga continut la final
    // O_TRUNC - goleste continutul fisierului daca exista
    // daca append_mode este 1, atunci se vas deschide in modul append
    // altfel se va deschide in modul trunchiere
    int flags = O_WRONLY | O_CREAT | (append_mode ? O_APPEND : O_TRUNC);
    int fd = open(filename, flags, 0644);

    // in caz de eroare, se afiseaza un mesaj
    if (fd < 0) {
        perror("Eroare la deschiderea fisierului");
        return;
    }

    // variabila in care se va salva ceea ce vrem sa scriem in fisier si la STDOUT
    char buffer[1024];
    // byte-ul citit
    ssize_t bytes_read;

    // citim de la STDIN un sir de caractere
    bytes_read = read(STDIN_FILENO, buffer, sizeof(buffer));
        // scriem in fisier
        if (write(fd, buffer, bytes_read) < 0) {
            perror("Eroare la scrierea in fisier");
            close(fd);
            return;
        }

        // scriem la STDOUT
        if (write(STDOUT_FILENO, buffer, bytes_read) < 0) {
            perror("Eroare la scrierea pe STDOUT");
            close(fd);
            return;
        }
    // inchidem fisierul
    close(fd);
}


// Functie pentru a verifica daca numele unui fisier se potriveste cu un pattern care contine wildcard-ul '*'
int matches_pattern(const char *filename, const char *pattern) {
    // Cautam primul caracter '*' in pattern
    const char *wildcard = strchr(pattern, '*');
    // Daca pattern-ul nu contine '*', facem o comparatie directa intre filename si pattern
    if (!wildcard) {
        return strcmp(filename, pattern) == 0;
    }
    // Determinam lungimea partii dinainte de '*'
    size_t prefix_length = wildcard - pattern;
    // Verificam daca inceputul numelui fisierului se potriveste cu prefixul din pattern
    if (strncmp(filename, pattern, prefix_length) != 0) {
        return 0;
    }
    // Verificam partea de dupa '*'
    const char *suffix = wildcard + 1;
    const char *file_suffix = filename + strlen(filename) - strlen(suffix);
    // Verificam daca sfarsitul numelui fisierului se potriveste cu sufixul din pattern
    return strcmp(file_suffix, suffix) == 0;
}

int is_directory(const char *path) {
    DIR *dir = opendir(path);
    if (dir) {
        closedir(dir);
        return 1;
    }
    return 0;
}

void find_command(const char *path, const char *name, const char *type) {
    sleep(5);
    // Deschide directorul specificat
    DIR *dir = opendir(path);
    if (!dir) {
        // Daca nu se poate deschide directorul, afiseaza un mesaj de eroare
        const char *error_msg = "Nu s-a putut deschide directorul\n";
        write(STDOUT_FILENO, error_msg, strlen(error_msg));
        return;
    }

    // Structuri pentru a citi continutul directorului, pentru a obtine informatii despre fisiere si buffer pentru a construi calea completa
    struct dirent *entry;
    //struct stat fileStat;
    char fullPath[PATH_MAX];

    // Parcurge fiecare intrare din director
    while ((entry = readdir(dir)) != NULL) {
        // Ignora intrarile speciale "." si ".."
        if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0) {
            continue;
        }

        // Creeaza calea completa pentru fisier/director
        // Se creeaza un pointer pentru a construi calea completa
        // Se creeaza pointer pentru a parcurge calea de baza si apoi copiem path in fullPath
        char *ptr = fullPath;
        const char *src = path;
        while (*src) {
            *ptr++ = *src++;
        }
        // Se adauga separatorul `/` intre path si numele fisierului, se continua cu numele fisierului sau directorului si se copiaza entry->d_name in fullPath apoi se adauga \0 la finalul stringului
        *ptr++ = '/';
        src = entry->d_name;
        while (*src) {
            *ptr++ = *src++;
        }
        *ptr = '\0';

        // Verifica potrivirea dupa nume
        int name_matches = 1;
        if (name) {
            name_matches = matches_pattern(entry->d_name, name);
        }

        // Verifica tipul fisierului
        int type_matches = 1;
        if (type) {
            if (entry->d_type == 0) {
                // Nu putem verifica tipul daca d_type este DT_UNKNOWN
                const char *type_error = "Tip necunoscut pentru fisier/director\n";
                write(STDOUT_FILENO, type_error, strlen(type_error));
                continue;
            }
            if ((strcmp(type, "f") == 0 && entry->d_type != 8) ||
                (strcmp(type, "d") == 0 && entry->d_type != 4)) {
                type_matches = 0;
                }
        }

        // Afiseaza numele fisierului daca toate conditiile sunt indeplinite
        if (name_matches) {
            write(STDOUT_FILENO, entry->d_name, strlen(entry->d_name));
            write(STDOUT_FILENO, "\n", 1);
        }
    }
    // Inchide directorul dupa ce toate intrarile au fost procesate
    closedir(dir);
}

void watch_command(const char *interval, const char *cmd) {
    // Convertim intervalul de timp din string in int
    int seconds = atoi(interval);
    if (seconds <= 0) {
        write(STDOUT_FILENO, "Interval invalid\n", 18);
        return;
    }

    // Bucla infinita care executa comanda periodic la intervalul specificat
    while (1) {
        // Afisam timpul curent
        time_t current_time = time(NULL);
        char *time_string = ctime(&current_time);
        time_string[strlen(time_string) - 1] = '\0'; // Eliminam newline-ul implicit
        write(STDOUT_FILENO, "Current time: ", 14);
        write(STDOUT_FILENO, time_string, strlen(time_string));
        write(STDOUT_FILENO, "\n", 1);
        if (cmd == NULL) {
            // Daca nu este specificata o subcomanda, afiseaza un mesaj de simulare
            const char *simulation_msg = "Nicio coamnda specificata. Vom testa doar watch\n";
            write(STDOUT_FILENO, simulation_msg, strlen(simulation_msg));
        }
        // Apelam functia my_system pentru a executa comanda specificata
        else {
        // Cream un buffer temporar pentru comanda care urmeaza a fi executata
        char temp_cmd[LG_MAX_COMANDA];

        // Copiem comanda in buffer
        strncpy(temp_cmd, cmd, LG_MAX_COMANDA - 1);

        // Ne asiguram ca sirul este terminat cu NULL
        temp_cmd[LG_MAX_COMANDA - 1] = '\0';
            mySystem(temp_cmd);
        }



        // Se asteapta timpul specificat inainte de a se executa din nou comanda
        sleep(seconds);

    }
}

// functie pentru schimbarea permisiunilor unui fisier
void handle_chmod(const char *mode_str, const char *file) {
    // convertim codul de permisiuni din string in octal
    char *endptr;
    long mode = strtol(mode_str, &endptr, 8);

    // verificam daca conversia a reusit
    if (*endptr != '\0' || mode < 0) {
        write(STDOUT_FILENO, "Cod de permisiune invalid\n", 27);
        return;
    }

    // incercam sa schimbam permisiunile fisierului
    if (chmod(file, (mode_t)mode) == -1) {
        const char *error_msg = "Eroare la schimbarea permisiunilor\n";
        write(STDOUT_FILENO, error_msg, strlen(error_msg));
    } else {
        write(STDOUT_FILENO, "Permisiunile au fost schimbate cu succes\n", 42);
    }
}
