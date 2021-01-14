#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include "card.h"

int comparator(const void* a, const void* b) 
{ 
	struct card **cardA = (struct card **)a;
    struct card **cardB = (struct card **)b;
  	return strcmp((*cardA)->name, (*cardB)->name);
} 

int main(int argc, char **argv)
{
	char *inputFile = argv[1];
    FILE *input;
	errno = 0;
	char *line = malloc(4096);
	char *token;
	char *last_token;
	//const char *rarity[] = {"common", "uncommon", "rare", "mythic"};
	int i = 0;

	// Open file
	input = fopen(inputFile, "r");
    if (errno == ENOENT) {
		fprintf(stderr, "./parser: cannot open(\"%s\"): No such file or directory\n", inputFile);
		exit(1);
	}
	if (input == NULL) {
		printf("./parser: an input path is required!\n");
		exit(1);
		
	}

	struct card **card = malloc(69420 * sizeof(struct card *));

	// While loop to parse through the file
	fgets(line, 4096, input);
	while ((fgets(line, 4096, input))) {

		// Allocate memory for card[i]
		card[i] = malloc(sizeof(struct card));
		// Allocate memory for card[i]->name
		card[i]->name = malloc(32 * sizeof(card[i]->name));
		// Allocate memory for card[i]->cost
		card[i]->cost = malloc(32 * sizeof(card[i]->cost));
		// Allocate memory for card[i]->type
		card[i]->type = malloc(64 * sizeof(card[i]->type));
		// Allocate memory for card[i]->text
		card[i]->text = malloc(512 * sizeof(card[i]->text));
		// Allocate memory for card[i]->stats
		card[i]->stats = malloc(16 * sizeof(card[i]->stats));

        char *original = line;

		// While loop to find actual \n characters, and turn them into a '\n' (newline) character
		while(strstr(original, "\\n") != NULL) {
			char *p = strstr(original, "\\n");
	    	if (p != NULL) {
		    	memcpy(p, "\n", 1);
		    	memmove(p + 1, p + 2, strlen(p + 2) + 1);
			}	
		}

		// Seperate the string to get id
		token = strsep(&original, ",");	
		card[i]->id = atoi(token);

		// Seperate the string to get name
		token = strsep(&original, "\"");	
		token = strsep(&original, "\"");	
		strcpy(card[i]->name, token);
		
		// Seperate the string to get cost
		token = strsep(&original, ",");
		token = strsep(&original, ",");		
		// if statement to remove double-quotes from cost
		if (token[0] == '"') {
			token++;
			token[strlen(token) - 1] = '\0';
		}						
		strcpy(card[i]->cost, token);

		// Seperate the string to get converted_cost
		token = strsep(&original, ",");			
		card[i]->converted_cost = atoi(token);

		// Seperate the string to get type
		token = strsep(&original, "\"");	
		token = strsep(&original, "\"");
		strcpy(card[i]->type, token);
	
		// Seperate the string to get text
		token = strsep(&original, ",");
		last_token = strsep(&original, "\"");
		// if text field is empty and only has a comma, remove the comma
		if (last_token[0] == ',') {
			last_token[0] = '\0';  				
			strcpy(card[i]->text, last_token);

			// Seperate the string to get stats
			token = strsep(&original, "\"");
			if (token[0] == 'c' || token[0] == 'u' || token[0] == 'r' || token[0] == 'm') {
				strcpy(card[i]->stats, last_token);	
				// Seperate the string to get the rarity
		 		switch(token[0]) {
		     			case 'c':
		        		 	card[i]->rarity = common;
		       		  		break;
		       		     	case 'u':
		       			  	card[i]->rarity = uncommon;
		        			break;
		     			case 'r':
		       		  		card[i]->rarity = rare;
		       		  		break;
		     			default:
		       		  		card[i]->rarity = mythic;
		       		  		break;
		 		}
			} else {
				strcpy(card[i]->stats, token);
				// Seperate the string to get the rarity
				token = strsep(&original, "\"");
				token = strsep(&original, "\"");
		 		switch(token[0]) {
		     			case 'c':
		        		 	card[i]->rarity = common;
		       		  		break;
		       		     	case 'u':
		       			  	card[i]->rarity = uncommon;
		        			break;
		     			case 'r':
		       		  		card[i]->rarity = rare;
		       		  		break;
		     			default:
		       		  		card[i]->rarity = mythic;
		       		  		break;
		 		}
			}
		} 
		// else find the whole text field 
		else {
			char *final_token = malloc(4096 * sizeof(char));
			strcpy(final_token, "");
			int complete = 0;

			// loop to get the whole text field
			while (complete == 0) {
				char *temp;
				char *temp_token;
				token = strsep(&original, "\"");

				char *temp1 = strdup(original);
				temp = temp1;
				temp_token = strsep(&temp, "\"");
				
				// if the next token is a comma, then text field is complete
				if (temp_token[0] == ',') {
					strcat(final_token, token);
					complete = 1;
				} 
				// else if the next token is empty
				else if (temp_token[0] == '\0') {
					strcat(final_token, token);
					strncat(final_token, "\"", 2);
					token = strsep(&original, "\"");
				}
				free(temp1);
			}

			strcpy(card[i]->text, final_token);

			free(final_token);

			// Seperate the string to get stats
			token = strsep(&original, "\"");
			token = strsep(&original, "\"");

			// if first char is a c, u, r, or m, then stats field is empty,
			// then rarity field is obtained
			if (token[0] == 'c' || token[0] == 'u' || token[0] == 'r' || token[0] == 'm') {
				char *empty = "";
				strcpy(card[i]->stats, empty);

				switch(token[0]) {
             			case 'c':
                		 	card[i]->rarity = common;
               		  		break;
               		     	case 'u':
               			  	card[i]->rarity = uncommon;
                			break;
             			case 'r':
               		  		card[i]->rarity = rare;
               		  		break;
             			default:
               		  		card[i]->rarity = mythic;
               		  		break;
         			}

			} 
			// else get stats field and rarity field
			else {
				strcpy(card[i]->stats, token);

				// Seperate the string to get the rarity
				token = strsep(&original, "\"");
				if (token[0] == ',') {
					token = strsep(&original, "\"");
				}
				switch(token[0]) {
             			case 'c':
                		 	card[i]->rarity = common;
               		  		break;
               		     	case 'u':
               			  	card[i]->rarity = uncommon;
                			break;
             			case 'r':
               		  		card[i]->rarity = rare;
               		  		break;
             			default:
               		  		card[i]->rarity = mythic;
               		  		break;
         			}
			}
		}

		// for loop to check if the card name is a duplicate from a card already read
		for (int k = 0; k < i; k++) {
			// if the card name is similar
			if (strcmp(card[i]->name, card[k]->name) == 0) {
				// if current id is smaller than k_id
				if (card[i]->id < card[k]->id || card[i]->id == card[k]->id ) {
					free(card[i]->name);
					free(card[i]->cost);
					free(card[i]->type);
					free(card[i]->text);
					free(card[i]->stats);
					free(card[i]);
					i = i - 1;
				} 
				// else if current id is greater than k_id
				else if (card[i]->id > card[k]->id) {

					for (int h = k; h < i; h++) {
						card[h]->id = card[h + 1]->id;
						memmove(card[h]->name, card[h + 1]->name, strlen(card[h + 1]->name) + 1);
						memmove(card[h]->cost, card[h + 1]->cost, strlen(card[h + 1]->cost) + 1);
						card[h]->converted_cost = card[h + 1]->converted_cost;
						memmove(card[h]->type, card[h + 1]->type, strlen(card[h + 1]->type) + 1);
						memmove(card[h]->text, card[h + 1]->text, strlen(card[h + 1]->text) + 1);
						memmove(card[h]->stats, card[h + 1]->stats, strlen(card[h + 1]->stats) + 1);
						card[h]->rarity = card[h + 1]->rarity;
					}
					free(card[i]->name);
					free(card[i]->cost);
					free(card[i]->type);
					free(card[i]->text);
					free(card[i]->stats);
					free(card[i]);
					i = i - 1;

				}
			}
		}

		// Increment i every loop
		i++;	

    	}

	/*
	for (int l = 0; l < i; l++) {
		int name_length = strlen(card[l]->name);
		int space = 52 - name_length;

		int type_length = strlen(card[l]->type);
		int space2 = 52 - type_length;

		printf("%-*s%*s\n", name_length, card[l]->name, space, card[l]->cost);
		printf("%-*s%*s\n", type_length, card[l]->type, space2, rarity[card[l]->rarity]);
		printf("----------------------------------------------------\n");
		printf("%s\n", card[l]->text);
		printf("----------------------------------------------------\n");
		printf("%52s\n", card[l]->stats);
		printf("\n");
	}
	*/

	// Open cards.bin and index.bin files to write
	FILE *cards_out = fopen("cards.bin", "wb");
	FILE *index_out = fopen("index.bin", "wb");
	
	// For loop to write binary data into cards.bin
	for (int l = 0; l < i; l++) {
		off_t offset = ftell(cards_out);
		card[l]->offset = offset;

		int cost_length = strlen(card[l]->cost);
		int type_length = strlen(card[l]->type);
		int text_length = strlen(card[l]->text);
		int stats_length = strlen(card[l]->stats);

		fwrite(&card[l]->id, sizeof(int), 1, cards_out);
		fwrite(&cost_length, sizeof(int), 1, cards_out);
		fwrite(card[l]->cost, sizeof(char), strlen(card[l]->cost), cards_out);
		fwrite(&card[l]->converted_cost, sizeof(int), 1, cards_out);
		fwrite(&type_length, sizeof(int), 1, cards_out);
		fwrite(card[l]->type, sizeof(char), strlen(card[l]->type), cards_out);
		fwrite(&text_length, sizeof(int), 1, cards_out);
		fwrite(card[l]->text, sizeof(char), strlen(card[l]->text), cards_out);
		fwrite(&stats_length, sizeof(int), 1, cards_out);
		fwrite(card[l]->stats, sizeof(char), strlen(card[l]->stats), cards_out);
		fwrite(&card[l]->rarity, sizeof(int), 1, cards_out);
	}

	// Use qsort function to sort struct alphabetically
	qsort(card, i, sizeof(struct card **), comparator);

	// For loop to write binary data into index.bin
	for (int l = 0; l < i; l++) {
		int name_length = strlen(card[l]->name);

		fwrite(&name_length, sizeof(int), 1, index_out);
		fwrite(card[l]->name, sizeof(char), strlen(card[l]->name), index_out);
		fwrite(&card[l]->offset, sizeof(off_t), 1, index_out);
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
	free(line);

    // Close files
    fclose(input);
	fclose(cards_out);
	fclose(index_out);

    return 0;
}
