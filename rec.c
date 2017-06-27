#include "rec.h"

REC *rec_new(int inscription, char *name, char *course, float score) {

	REC *rec = malloc(sizeof(REC));

	rec->inscription = inscription;
	strcpy(rec->name, name);
	strcpy(rec->course, course);
	rec->score = score;
	
	return rec;

}

size_t rec_size(REC *rec) {

	size_t size = sizeof(int) + sizeof(float) + 4;
	size += strlen(rec->name) + strlen(rec->course);

	return size;

}

void rec_free(REC *rec) {
	free(rec);
}

void rec_update(REC *rec, int inscription, char *name, char *course, float score) {

	rec->inscription = inscription;
	strcpy(rec->name, name);
	strcpy(rec->course, course);
	rec->score = score;

}

void rec_print(REC *rec) {
	printf("Registro -> Inscricao: %d\n", rec->inscription);
	printf("         -> Nome: %s\n", rec->name);
	printf("         -> Curso: %s\n", rec->course);
	printf("         -> Pontos: %f\n", rec->score);
}