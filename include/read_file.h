/*#include <stdio.h>
#include <stdlib.h>
#include <hash_map.h>
int read_file(char *path, hash_map_struct_t *h_map) {
	
	FILE *fptr;
	char content;
	fptr = fopen(path, "r");
	
	if(fptr == NULL) {
		printf("Cannot open file \n");
		exit(0);
	}
	
	content = fgetc(fptr);
	
	while(content != EOF) {
		printf("%c", content);
		content = fgetc(fptr);
	}	

	fclose(fptr);
	return 0;
}*/
