#include <stdio.h>
#include <stdlib.h>
#include "frec.h"
#include "rec.h"

REC get_rec(FILE *);
char *get_field(FILE *);
void write_rec(FILE *, REC *);
void write_rec_nolen(FILE *, REC *);
time_t write_header(FILE *);
int frec_seek(FILE *, int);
short get_led(char[]);
void update_led(char[], short);
int led_prev(char[], int);
int led_next(char[], int);
int frec_seek(FILE *, int);
int led_last(char[]);

// FUNCIONADO
int frec_import(char file_name[], char *import_name) {

	FILE *file = fopen(file_name, "r");

	char buffer[100];

	// Verifica se o arquivo passado como parâmetro é válido
	if (file == NULL) {
		//fprintf(stderr, "ERROR: in function frec_import(FILE *file) -> file is null.\n");
		return 0;
	}

	// Declara e cria o arquivo de registros
	sprintf(buffer, "%s/%s.%s", IMPORT_FOLDER, import_name, IMPORT_FORMAT);
	FILE *frec = fopen(buffer, "wb");
	if (!(frec = fopen(buffer, "wb"))) {
		fprintf(stderr, "ERROR: in function frec_import(FILE *file, char *name) -> import file cannot be created.\n");
		return 0;
	}

	// ??? Ler caracteres desconhecidos ???
	fgetc(file);
	fgetc(file);
	fgetc(file);	

	write_header(frec);

	for (REC rec = get_rec(file); rec.inscription != EOF && rec.score != EOF; rec = get_rec(file)) {
		write_rec(frec, &rec);
	}

	fclose(frec);

	return 1;

}

// FUNCIONANDO
REC frec_search(char file_name[], int inscription) {

	char buffer[100];

	sprintf(buffer, "%s/%s.%s", IMPORT_FOLDER, file_name, IMPORT_FORMAT);
	FILE *frec;
	if (!(frec = fopen(buffer, "rb"))) {
		fprintf(stderr, "ERROR: in function frec_search(char *, int) -> import file cannot be created.\n");
		return;
	}
	buffer[0] = '\0';

	fseek(frec, header_size() , SEEK_SET);
	
	REC rec;

	int key;
	while (1) {
		short offset;
		fread(&offset, sizeof(short), 1, frec);

		int key;
		int r = fread(&key, sizeof(key), 1, frec);

		if (!r) {
			break;
		}

		if (key == inscription) {
			rec.inscription = inscription;
			fseek(frec, 1, SEEK_CUR);
			char *str = get_field(frec);
			strcpy(rec.name, str);

			free(str);

			str = get_field(frec);
			strcpy(rec.course, str);
			free(str);
			fread(&(rec.score), sizeof(float), 1, frec);
			fclose(frec);

			return rec;
		} else {
			fseek(frec, offset - sizeof(int), SEEK_CUR);
		}

	}

	fclose(frec);

	rec.inscription = EOF;
	strcpy(rec.name, "EOF");
	strcpy(rec.course, "EOF");
	rec.score = EOF;
	return rec;

}

// IMPLEMENTADO
int frec_remove(char file_name[], int inscription) {

	char buffer[100];

	sprintf(buffer, "%s/%s.%s", IMPORT_FOLDER, file_name, IMPORT_FORMAT);
	FILE *frec;
	if (!(frec = fopen(buffer, "r+b"))) {
		fprintf(stderr, "ERROR: in function frec_remove(char *, int) -> records file %s cannot be openned.\n", file_name);
		return -1;
	}
	buffer[0] = '\0';

	int rec_size = frec_seek(frec, inscription);

	if (rec_size == -1) return 0;

	short led = get_led(file_name);

	if (led == -1) {
		update_led(file_name, ftell(frec));
		int menos_um = -1;
		fwrite(&menos_um, sizeof(menos_um), 1, frec);
	} else {
		int offset = ftell(frec), last = LED_LAST;
		fwrite(&last, sizeof(int), 1, frec);
		fseek(frec, led_last(file_name), SEEK_SET);
		offset = -offset;
		fwrite(&offset, sizeof(int), 1, frec);
	}

	fclose(frec);

	return 1;

}

// SENDO IMPLEMENTADO
void frec_add(char file_name[], int inscription, char name[], char course[], float score) {

	char buffer[100];

	sprintf(buffer, "%s/%s.%s", IMPORT_FOLDER, file_name, IMPORT_FORMAT);
	FILE *frec;
	if (!(frec = fopen(buffer, "r+b"))) {
		fprintf(stderr, "ERROR: in function frec_add(char *, int, char *, char *, float) -> records file %s cannot be openned.\n", file_name);
		return;
	}
	buffer[0] = '\0';

	REC rec;
	rec.inscription = inscription;
	strcpy(rec.name, name);
	strcpy(rec.course, course);
	rec.score = score;

	if (get_led(file_name) == LED_LAST) {
		fseek(frec, 0, SEEK_END);
		write_rec(frec, &rec);
		fclose(frec);
		return;
	}

	int offset = led_fit(file_name, rec_size(&rec));

	if (offset == -1) {
		fseek(frec, 0, SEEK_END);
		write_rec(frec, &rec);
	} else {
		fseek(frec, offset, SEEK_SET);
		int p = led_prev(file_name, offset);
		int n = led_next(file_name, offset);

		if (offset == get_led(file_name)) {
			update_led(file_name, n);
			fseek(frec, offset, SEEK_SET);
			write_rec_nolen(frec, &rec);
		} else {
			fseek(frec, p, SEEK_SET);
			n = -n;
			fwrite(&n, sizeof(n), 1, frec);
			fseek(frec, offset, SEEK_SET);
			write_rec_nolen(frec, &rec);
		}
	}

	fclose(frec);

}

// IMPLEMENTADO
void frec_print(char file_name[]) {

	char buffer[100];

	sprintf(buffer, "%s/%s.%s", IMPORT_FOLDER, file_name, IMPORT_FORMAT);
	FILE *frec;
	if (!(frec = fopen(buffer, "rb"))) {
		fprintf(stderr, "ERROR: in function freg_search(char *file_name, int inscription) -> import file cannot be created.\n");
		return;
	}
	buffer[0] = '\0';
	
	char c;
	int i = 0;
	for (fread(&c, 1, 1, frec); i != 2; fread(&c, 1, 1, frec)) {
		if (c == '@') i++;
		printf("%c", c);
	}
	short h;
	fread(&h, sizeof(h), 1, frec);

	printf("%d#", get_led(file_name));
	printf("\n");

	REC rec;

	int key;
	while (1) {
		short offset;
		fread(&offset, sizeof(short), 1, frec);

		int key;
		int r = fread(&key, sizeof(key), 1, frec);

		if (!r) {
			printf("INFO: key %d not found.\n", key);
			break;
		}

		
		rec.inscription = key;
		fseek(frec, 1, SEEK_CUR);
		char *str = get_field(frec);
		strcpy(rec.name, str);
		free(str);

		//fseek(frec, 1, SEEK_CUR);
		str = get_field(frec);
		strcpy(rec.course, str);
		free(str);
			
		fseek(frec, 1, SEEK_CUR);
		float score;
		fread(&score, sizeof(rec.score), 1, frec);
		rec.score = score;

		if (rec.inscription < 0) printf("REMOVED -> ");
		printf("%d|%s|%s|%.2f\n", rec.inscription, rec.name, rec.course, rec.score);

		fseek(frec, offset - rec_size(&rec), SEEK_CUR);

	}

	fclose(frec);

	return rec;

}

// \/ \/ \/ Funções Auxiliares \/ \/ \/

int frec_seek(FILE *frec, int inscription) {

	char buffer[100];

	fseek(frec, header_size(), SEEK_SET);

	REC rec;

	int cur_offset = 0;

	int key;
	while (1) {
		short offset;
		fread(&offset, sizeof(short), 1, frec);

		cur_offset = ftell(frec);

		int key;
		int r = fread(&key, sizeof(key), 1, frec);

		if (!r) {
			break;
		}

		if (key == inscription) {
			fseek(frec, -sizeof(int), SEEK_CUR);
			return offset;
		} else {
			fseek(frec, offset - sizeof(int), SEEK_CUR);
		}

	}

	return -1;

}

REC get_rec(FILE *file) {

	char *str_ins = get_field(file), *name = get_field(file), *course = get_field(file), *str_score = get_field(file);

	if (str_ins == EOF || name == EOF || course == EOF || str_score == EOF) {
		REC rec;
		rec_update(&rec, EOF, "EOF", "EOF", EOF);
		return rec;
	}

	int inscription = strtol(str_ins, NULL, 10);
	float score = strtof(str_score, NULL);

	REC rec;
	rec_update(&rec, inscription, name, course, score);
	
	free(str_ins); free(name); free(course); free(str_score);
	return rec;

}

char *get_field(FILE *file) {

	char *buffer = malloc(sizeof(char)*100);

	int i = 0;
	for (char c = fgetc(file); c != '|'; c = fgetc(file)) {
		
		if (c == EOF) return EOF;
		
		buffer[i] = c;
		i++;
	}

	buffer[i] = '\0';

	return buffer;

}

void write_rec(FILE *frec, REC *rec) {

	size_t rec_size_size = rec_size(rec);
	
	char field_separator = FIELD_DELIMITER;
	char rec_separator = REC_DELIMITER;
	
	fwrite(&rec_size_size, 2, 1, frec);

	fwrite(&rec->inscription, sizeof(rec->inscription), 1, frec);
	fwrite(&field_separator, sizeof(char), sizeof(char), frec);

	fwrite(rec->name, strlen(rec->name), 1, frec);
	fwrite(&field_separator, sizeof(char), sizeof(char), frec);

	fwrite(rec->course, strlen(rec->course), 1, frec);
	fwrite(&field_separator, sizeof(char), sizeof(char), frec);

	fwrite(&rec->score, sizeof(rec->score), 1, frec);
	fwrite(&rec_separator, sizeof(char), sizeof(char), frec);

}

void write_rec_nolen(FILE *frec, REC *rec) {

	//size_t rec_size_size = rec_size(rec);

	char field_separator = FIELD_DELIMITER;
	char rec_separator = REC_DELIMITER;

	//fwrite(&rec_size_size, 2, 1, frec);

	fwrite(&rec->inscription, sizeof(rec->inscription), 1, frec);
	fwrite(&field_separator, sizeof(char), sizeof(char), frec);

	fwrite(rec->name, strlen(rec->name), 1, frec);
	fwrite(&field_separator, sizeof(char), sizeof(char), frec);

	fwrite(rec->course, strlen(rec->course), 1, frec);
	fwrite(&field_separator, sizeof(char), sizeof(char), frec);

	fwrite(&rec->score, sizeof(rec->score), 1, frec);
	fwrite(&rec_separator, sizeof(char), sizeof(char), frec);

}

time_t write_header(FILE *file) {
	char buffer[100];
	
	time_t now;
	time(&now);

	struct tm *now_tm;
	now_tm = localtime(&now);

	char date[20];
	strftime(date, strlen(date), HEADER_DATE_FORMAT, now_tm);

	sprintf(buffer, "%s%c%s%c", HEADER_TYPE, HEADER_FIELD_DELIMITER, date, HEADER_FIELD_DELIMITER);

	fwrite(buffer, strlen(buffer), 1, file);

	short led = LED_LAST;
	fwrite(&led, sizeof(led), 1, file);
	
	char c = HEADER_DELIMITER;
	fwrite(&c, sizeof(c), 1, file);

	return mktime(now_tm);
}

size_t header_size() {
	return strlen(HEADER_TYPE) + 1 + strlen(HEADER_DATE_FORMAT) + 2 + 1 + sizeof(short) + 1 ;
}

short get_led(char file_name[]) {

	char buffer[100];

	sprintf(buffer, "%s/%s.%s", IMPORT_FOLDER, file_name, IMPORT_FORMAT);
	FILE *file;

	if (!(file = fopen(buffer, "rb"))) {
		fprintf(stderr, "ERROR: in function get_led(char *) -> records file %s cannot be openned.\n", buffer);
		return LED_ERROR;
	}

	fseek(file, 2 + strlen(HEADER_TYPE) + sizeof(char) * 2 + strlen(HEADER_DATE_FORMAT), SEEK_SET);

	short ret;
	fread(&ret, sizeof(ret), 1, file);

	fclose(file);

	return ret;

}

void update_led(char file_name[], short led) {

	char buffer[100];

	sprintf(buffer, "%s/%s.%s", IMPORT_FOLDER, file_name, IMPORT_FORMAT);
	FILE *file;
	if (!(file = fopen(buffer, "r+b"))) {
		fprintf(stderr, "ERROR: in function update_led -> import file cannot be created.\n");
		return;
	}

	fseek(file, 2 + strlen(HEADER_TYPE) + sizeof(char) * 2 + strlen(HEADER_DATE_FORMAT), SEEK_SET);

	fwrite(&led, sizeof(led), 1, file);

	fclose(file);

}

int led_last(char file_name[]) {
	
	char buffer[100];

	sprintf(buffer, "%s/%s.%s", IMPORT_FOLDER, file_name, IMPORT_FORMAT);
	FILE *frec;
	if (!(frec = fopen(buffer, "rb"))) {
		fprintf(stderr, "ERROR: in function led_last(char *) -> import file cannot be created.\n");
		return LED_ERROR;
	}
	buffer[0] = '\0';	

	int led = get_led(file_name);
	
	printf("led -> ");
	while (1) {

		printf("%d ", led);

		fseek(frec, led, SEEK_SET);
		fread(&led, sizeof(int), 1, frec);

		if (led == LED_LAST) {
			printf("%d", led);
			break;
		}
		if (led < 0) led = -led;

	}

	printf("\n");

	int offset = ftell(frec) - sizeof(int);
	fclose(frec);

	return offset;

}

int led_prev(char file_name[], int offset) {

	char buffer[100];

	sprintf(buffer, "%s/%s.%s", IMPORT_FOLDER, file_name, IMPORT_FORMAT);
	FILE *frec;
	if (!(frec = fopen(buffer, "rb"))) {
		fprintf(stderr, "ERROR: in function led_last(char *) -> import file cannot be created.\n");
		return LED_ERROR;
	}
	buffer[0] = '\0';

	int led = get_led(file_name);

	while (1) {

		int offset_prev = led;

		fseek(frec, led, SEEK_SET);
		fread(&led, sizeof(int), 1, frec);

		if (led == LED_LAST) break;
		if (led < 0) led = -led;

		if (offset == led) {
			offset = offset_prev;
			if (offset != -1 && offset < 0) offset = -offset;
			break;
		}

	}

	fclose(frec);

	return offset;

}

int led_next(char file_name[], int offset) {

	char buffer[100];

	sprintf(buffer, "%s/%s.%s", IMPORT_FOLDER, file_name, IMPORT_FORMAT);
	FILE *frec;
	if (!(frec = fopen(buffer, "rb"))) {
		fprintf(stderr, "ERROR: in function led_next(char *) -> import file cannot be created.\n");
		return LED_ERROR;
	}
	buffer[0] = '\0';

	int led = get_led(file_name);

	int foi = 0;


	if (led == offset) foi = 1;

	while (1) {

		fseek(frec, led, SEEK_SET);
		fread(&led, sizeof(int), 1, frec);

		if (led == LED_LAST) break;
		if (led < 0) led = -led;

		if (foi == 1) {
			break;
		}

		if (offset == led) {
			foi = 1;
		}

	}

	fclose(frec);

	return led;

}

int led_fit(char file_name[], unsigned short num) {

	char buffer[100];

	sprintf(buffer, "%s/%s.%s", IMPORT_FOLDER, file_name, IMPORT_FORMAT);
	FILE *frec;
	if (!(frec = fopen(buffer, "rb"))) {
		fprintf(stderr, "ERROR: in function led_last(char *) -> import file cannot be created.\n");
		return LED_ERROR;
	}
	buffer[0] = '\0';

	int offset = 0;

	int led = get_led(file_name);

	while (led != -1) {

		fseek(frec, led, SEEK_SET);

		fseek(frec, -sizeof(unsigned short), SEEK_CUR);
		unsigned short size = 0;
		fread(&size, sizeof(size), 1, frec);

		if (size >= num) return ftell(frec);

		fread(&led, sizeof(int), 1, frec);

		if (led == LED_LAST) break;
		if (led < 0) led = -led;

	}

	return -1;

}