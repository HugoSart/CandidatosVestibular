#include <stdio.h>
#include <locale.h>
#include "rec.h"
#include "frec.h"

int main() {

	setlocale(LC_ALL, "");

	int r = 0;
	char file_name[100] = "";

	

	do {

		system("cls");
		
		printf("=============== GERENCIADOR DE MATRICULAS ===============\n\n");
		if (strlen(file_name) > 0) printf("  Arquivo aberto: %s\n\n", file_name);
		else		   printf("  Nenhum arquivo aberto\n\n");
		printf("  [1] - Abrir arquivos de registro existente\n");
		printf("  [2] - Importar por arquivo de texto\n");
		printf("  [3] - Adicionar registro\n");
		printf("  [4] - Remover registro\n");
		printf("  [5] - Buscar registro\n\n");
		printf("  [0] - Sair\n");
		printf("\nEscolha => ");	scanf("%d", &r);

		switch (r) {
			case 1:
				char name[100];
				system("cls");
				printf("=============== ABRIR ARQUIVO DE REGISTROS ===============\n\n");
				printf("  Nome do arquivo de registros \t=> ");	fflush(stdin);	 scanf("%s", name);
				char buffer[100];
				sprintf(buffer, "%s/%s.%s", IMPORT_FOLDER, name, IMPORT_FORMAT);
				if (!fopen(buffer, "rb"))	printf("\nERRO: Arquivo inválido.\n");
				else {
					printf("\nINFO: Arquivo aberto.\n");
					strcpy(file_name, name);
				}
				system("pause");
				break;
			case 2:
				char str1[100], str2[100];
				system("cls");
				printf("=============== IMPORTAR POR ARQUIVO DE TEXTO ===============\n\n");
				printf("  Nome do arquivo de texto \t=> ");		fflush(stdin);	 scanf("%s", str1);
				printf("  Nome do arquivo a ser criado \t=> ");	fflush(stdin);	 scanf("%s", str2);
				int a = frec_import(str1, str2);
				if (!a) printf("\nERROR: Arquivo inválido.\n");
				else    printf("\nINFO: Arquivo importado.\n");
				system("pause");
				break;
			case 3:
				if (!strcmp(file_name, "")) break;
				char name2[100], course[100];
				int ins;
				float score;
				system("cls");
				printf("=============== ADICIONAR REGISTRO ===============\n\n");
				printf("  Arquivo aberto: %s\n\n", file_name);
				printf("  Inscrição\t=> ");		scanf("%d", &ins);
				printf("  Name  \t=> ");		scanf("%s", name2);
				printf("  Curso \t=> ");		scanf("%s", course);
				printf("  Pontuação\t=> ");		scanf("%f", &score);
				frec_add(file_name, ins, name2, course, score);
				system("pause");
				break;
			case 4:
				if (!strcmp(file_name, "")) break;
				int ins2;
				system("cls");
				printf("=============== REMOVER REGISTRO ===============\n\n");
				printf("  Arquivo aberto: %s\n\n", file_name);
				printf("  Inscrição\t=> ");		scanf("%d", &ins2);
				if (frec_remove(file_name, ins2) == 0)	printf("\nERRO: Inscrição não encontada.\n");
				else									printf("\nINFO: Registro de inscrição %d removido.\n", ins2);
				
				system("pause");
				break;
			case 5:
				if (!strcmp(file_name, "")) break;
				int ins3;
				system("cls");
				printf("=============== BUSCAR REGISTRO ===============\n\n");
				printf("  Arquivo aberto: %s\n\n", file_name);
				printf("  Inscrição\t=> ");		scanf("%d", &ins3);
				REC rec = frec_search(file_name, ins3);
				if (rec.score == -1) {
					printf("\nERRO: Inscrição %d não encontrada.\n", ins3);
					system("pause");
				} else {
					printf("  Name  \t : %s\n", rec.name);
					printf("  Curso \t : %s\n", rec.course);
					printf("  Pontuação\t : %.2f\n\n", rec.score);
					system("pause");
				}
				break;
		}

	} while (r);

	return 0;
}