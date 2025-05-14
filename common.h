//
// Created by coman on 1/16/2025.
//
#pragma once

#include <fcntl.h>
#include <io.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>


#ifndef COMMON_H
#define COMMON_H



#define LG_MAX_COMANDA 1024
#define LG_MAX_UTILIZATOR 256
#define FILEINPUT "passwords.txt"

int mySystem(const char*);
int login(const char*, const char*);
void executa(char*);

#endif //COMMON_H
