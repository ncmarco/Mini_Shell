//
// Created by coman on 1/16/2025.
//
#pragma once

#ifndef FUNCTIONS_H
#define FUNCTIONS_H

#include <unistd.h>
#include <fcntl.h>
#include <dirent.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <time.h>

#define BUFFER_SIZE 512
#define PATH_MAX 1024

void handle_tee(int, char *);

int matches_pattern(const char *, const char *);
void find_command(const char*, const char *, const char *);

void watch_command(const char*, const char*);

void handle_chmod(const char *, const char *);

#endif //FUNCTIONS_H
