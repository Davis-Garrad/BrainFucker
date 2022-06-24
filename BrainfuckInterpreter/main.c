#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

// for sanity
#define byte char

unsigned max_bytes		= 1024;
unsigned byte* memory	= NULL;
unsigned int   cur_cell = 0;

void stacktrace() {
	printf("\n STACK: \n");
	printf("%6s  $-3s : %-4s", "index", "cha", "int");
	for(unsigned int i = 0; i < max_bytes; i++) {
		printf("%6d  %-3c : %-4d\n", i, memory[i], memory[i]);
	}
}

int are_eq(char* s1, char* s2) {
	return strcmp(s1, s2) == 0;
}

char* read_source(int argc, char** argv) {
	char* stream_content = NULL;

	for(unsigned int i = 0; i < argc; i++) {
		if(are_eq(argv[i], "-s")) {
			// read the stream
			if(i < argc - 1) {
				i++;
				unsigned int size		  = strlen(argv[i]);
				char*		 temp_content = malloc(size * sizeof(char));

				unsigned int true_size = 0;
				unsigned int cursor	   = 0;
				char		 read	   = 'a'; // Just so we know it's *something*

				while(read != '\0' && cursor < size) {
					sscanf(argv[i] + cursor * sizeof(char), "%c", &read);
					if(read == '<' || read == '>' || read == '+' || read == '-' || read == ',' || read == '.' ||
					   read == '[' || read == ']') {
						// It's an allowed character, read it in baby!
						temp_content[true_size] = read;
						true_size++;
					}
					cursor++;
				}
				temp_content[true_size] = '\0';

				// Now shrink temp content
				stream_content = malloc((true_size + 1) * sizeof(char));
				strcpy(stream_content, temp_content);
			}
		} else if(are_eq(argv[i], "-f")) {
			// read the filename
			char* filename = NULL;
			if(i < argc - 1) {
				i++;
				filename = malloc((strlen(argv[i]) + 1) * sizeof(char));
				strcpy(filename, argv[i]);
			}
			// read from the file
			FILE* fileptr = fopen(filename, "r");

			// Get the file size and initialize stream_content
			fseek(fileptr, 0, SEEK_END);
			unsigned int size = ftell(fileptr);
			fseek(fileptr, 0, SEEK_SET);
			char* temp_content = malloc(size * sizeof(char));

			unsigned int true_size = 0;
			unsigned int cursor	   = 0;
			char		 read	   = 'a'; // Just so we know it's *something*
			while(read != '\0' && cursor < size) {
				fscanf(fileptr, "%c", &read);
				if(read == '<' || read == '>' || read == '+' || read == '-' || read == ',' || read == '.' ||
				   read == '[' || read == ']') {
					// It's an allowed character, read it in baby!
					temp_content[true_size] = read;
					true_size++;
				}
				cursor++;
			}
			temp_content[true_size] = '\0';
			fclose(fileptr);

			// Now shrink temp content
			stream_content = malloc((true_size + 1) * sizeof(char));
			strcpy(stream_content, temp_content);
		} else {
			printf("Argument ignored: %s\n", argv[i]);
		}
	}

	return stream_content;
}

void run(char* source, unsigned int src_len, bool loop) {
	// If it is a loop, it runs until the "rest cell" is zero.
	// If it is not a loop, runs once. Simpley

	do {
		// Interpret
		for(unsigned int i = 0; i < src_len; i++) {
			char c = source[i];
			switch(c) {
				case '.':
					printf("%c", memory[cur_cell]);
					break;
				case ',':
					scanf("%c", &memory[cur_cell]);
					break;
				case '<':
					if(cur_cell > 0)
						cur_cell--;
					break;
				case '>':
					if(cur_cell < max_bytes - 1)
						cur_cell++;
					break;
				case '+':
					memory[cur_cell]++;
					break;
				case '-':
					memory[cur_cell]--;
					break;
				case '[': {
					unsigned int len   = 1; // At least two. [] is smallest
					char		 next  = source[i];
					unsigned int count = 1; // Count of openings.
					while(count != 0) {
						next = source[i + len];
						len++;
						if(next == '[') {
							// we need to look for an extra closing bracket
							count++;
						} else if(next == ']') {
							count--;
						}
					}
					len -= 2; // for beginning and end brackets
					// Now we have the starting position and length, let's copy and run the sub-program
					char* substr = malloc((len + 1) * sizeof(char));
					for(unsigned int j = 0; j < len; j++) {
						substr[j] = source[i + j + 1];
					}
					substr[len] = '\0';
					run(substr, len, true);
					i += len + 1;
					break;
				}
				case ']':
					// Nothing here.
					break;
				default:
					//printf("Default interpretation - you broke something. \'%c\'", c);
					break;
			}
		}
		if(memory[cur_cell] == 0 || !loop)
			return;
	} while(true);

	return;
}

int main(int argc, char** argv) {
	// Takes arguments from command line input, and runs the given (brainfucky) source

	/// Initialize

	char*		 source	 = read_source(argc, argv);
	unsigned int src_len = strlen(source);

	printf("Program source read\n");

	// allocate memory (individual bytes)
	memory = malloc(max_bytes * sizeof(byte));
	for(unsigned int i = 0; i < max_bytes; i++) {
		memory[i] = 0;
	}

	printf("Program memory allocated (%d bytes)\n", max_bytes);

	printf("Starting program...\n\n");

	/// Interpret/Run
	run(source, src_len, false);

	printf("\n\nProgram completed.\n\n");

	return 0;
}
