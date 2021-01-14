#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include "card.h"

int main(int argc, char **argv)
{
	int i = 0;
	int name_length[4];
	int quit = 0;
	int found = 0;
	char query[32];
	const char *rarity[] = {"common", "uncommon", "rare", "mythic"};
	errno = 0;

	// Open cards.bin and index.bin files to read
	FILE *index_in = fopen("index.bin", "rb");
	FILE *cards_in = fopen("cards.bin", "rb");

	if (index_in == NULL && errno == ENOENT) {
		fprintf(stderr, "./search: cannot open(\"index.bin\"): No such file or directory\n");
		exit(1);
	}
	if (cards_in == NULL && errno == ENOENT) {
		fprintf(stderr, "./search: cannot open(\"cards.bin\"): No such file or directory\n");
		exit(1);
	} 

	struct card **card = malloc(69420 * sizeof(struct card *));

	// While loop to store data into a struct and into an array
	while (fread(&name_length, sizeof(int), 1, index_in) != 0) {
        card[i] = malloc(sizeof(struct card));
        card[i]->name = calloc(32, sizeof(char *));
		card[i]->cost = calloc(32, sizeof(char *));
		card[i]->type = calloc(64, sizeof(char *));
		card[i]->text = calloc(512, sizeof(char *));
		card[i]->stats = calloc(16, sizeof(char *));

        fread(card[i]->name, *name_length, 1, index_in);
        fread(&card[i]->offset, sizeof(off_t), 1, index_in);

		i++;
    }

	// While loop to read user input and perform a binary search for that card name
	while (quit == 0) {
		found = 0;
		printf(">> ");

		// If the input is not from a terminal
		if (isatty(scanf("%[^\n]%*c", query)) == 0) {
			fprintf(stdout, "%s\n", query);
		}

		// If user input is 'q', then exit the program
		if (strcmp(query, "q") == 0) {
			quit = 1;
		} 
		else {
			// For loop to check if query is inside the struct
			for (int l = 0; l < i; l++) {
				if (strcmp(card[l]->name, query) == 0) {
					fseek(cards_in, card[l]->offset, SEEK_SET);

					// Obtain the rest of the card data and assemble the 'CARD' record
					int cost_length = 0;
					int type_length = 0;
					int text_length = 0;
					int stats_length = 0;

					fread(&card[l]->id, sizeof(int), 1, cards_in);
					fread(&cost_length, sizeof(int), 1, cards_in);
					fread(card[l]->cost, cost_length, 1, cards_in);
					fread(&card[l]->converted_cost, sizeof(int), 1, cards_in);
					fread(&type_length, sizeof(int), 1, cards_in);
					fread(card[l]->type, type_length, 1, cards_in);
					fread(&text_length, sizeof(int), 1, cards_in);
					fread(card[l]->text, text_length, 1, cards_in);
					fread(&stats_length, sizeof(int), 1, cards_in);
					fread(card[l]->stats, stats_length, 1, cards_in);
					fread(&card[l]->rarity, sizeof(int), 1, cards_in);

					// Print data
					int name_len = strlen(card[l]->name);
					int space = 52 - name_len;
					int space2 = 52 - type_length;

					printf("%-*s%*s\n", name_len, card[l]->name, space, card[l]->cost);
					printf("%-*s%*s\n", type_length, card[l]->type, space2, rarity[card[l]->rarity]);
					printf("----------------------------------------------------\n");
					printf("%s\n", card[l]->text);
					printf("----------------------------------------------------\n");
					printf("%52s\n", card[l]->stats);
					printf("\n");

					found = 1;
					break;
				}
			}

			// If query is not found, then print not found
			if (found == 0) {
				printf("./search: \'%s\' not found!\n", query);
			}
		}
	}
	
	// Free allocated memory
	for(int k = 0; k < i; k++) {
		free(card[k]->name);
		free(card[k]->cost);
		free(card[k]->type);
		free(card[k]->text);
		free(card[k]->stats);
		free(card[k]);
	}
	free(card);

	// Close files
	fclose(cards_in);
	fclose(index_in);

    return 0;
}
