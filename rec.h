#pragma once

#include <string.h>
#include <malloc.h>

typedef struct rec {
	int inscription;
	char name[100];
	char course[100];
	float score;
} REC;

REC *rec_new(int inscription, char *name, char *course, float score);

size_t rec_size(REC *rec);

void rec_free(REC *rec);

void rec_update(REC *rec, int inscription, char *name, char *course, float score);

void rec_print(REC *rec);

