#pragma once

#include <stdio.h>
#include "rec.h"

#define IMPORT_FOLDER "Import"
#define IMPORT_FORMAT "dat"

#define HEADER_TYPE "FREC"
#define HEADER_DATE_FORMAT "%d/%m/%Y-%H:%M:%S"
#define HEADER_DELIMITER ((const char)'#')
#define HEADER_FIELD_DELIMITER ((const char)'@')

#define FIELD_DELIMITER ((const char)'|')
#define REC_DELIMITER   ((const char)'|')
#define REC_SIZE_SIZE   ((const unsigned int) 2)

#define LED_ERROR -2
#define LED_LAST  -1

/// Struct que corresponde a um arquivo de registros
typedef struct frec {
	char file_name[50];
	time_t date;
} frec_t;

/// Carrega um arquivo de registros para o programa
frec_t frec_load(FILE *file);

/// Converte um arquivo formatado para um arquivo de registros
int frec_import(char file_name[], char *import_name);

/// Adiciona um registro no arquivo de registrosv
void frec_add(char file_name[], int inscription, char name[], char course[], float score);

/// Procura um registro pela chave de inscrição 
REC frec_search(char file_name[], int inscription);

int frec_remove(char file_name[], int inscription);

void frec_print(char file_name[]);