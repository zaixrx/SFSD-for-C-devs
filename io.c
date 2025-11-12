#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <stdbool.h>

#define ERR_OUT(...) \
	do { \
		fprintf(stderr, "ERROR: " __VA_ARGS__); \
		exit(EXIT_FAILURE); \
	} while(0)

#define FORCE_WRITE(stream, ptr, size) \
	do { \
		fwrite(ptr, size, 1, stream); \
		fflush(stream); \
	} while(0)

#define ARR_LEN(arr) sizeof((arr)) / sizeof(*(arr))
typedef struct {
	void *items[2];
	size_t count;
} Block;

typedef struct {
    	FILE *F;
	struct {
		size_t n_blocks;
		size_t n_ops;
	} H;
} M_FILE;

size_t N_readACC = 0;
size_t N_writeACC = 0;

M_FILE *open(const char *file_path, char mode) {
   	M_FILE *file = (M_FILE*)malloc(sizeof(M_FILE));
   	if (!file) ERR_OUT("failed to malloc: %s", strerror(errno));
    	memset(file, 0, sizeof(*file)); // I know there is calloc

	// Append (open existing)
    	if (mode == 'A' || mode == 'a') {
    		file->F = fopen(file_path, "r+b");
    	    	if (!file->F) ERR_OUT("failed to open file for reading: %s", strerror(errno));
    	    	rewind(file->F);
    	    	fread(&file->H, sizeof(file->H), 1, file->F);
    	} else if (mode == 'N' || mode == 'n') {    // New file
		file->F = fopen(file_path, "w+b");
    	    	if (!file->F) ERR_OUT("failed to open file for writing: %s", strerror(errno));
    	    	rewind(file->F);
    	    	FORCE_WRITE(file->F, &file->H, sizeof(file->H));
    	} else {
		ERR_OUT("unrecongnized file format");
    	}

    	return file;
}

void close(M_FILE *file) {
    	rewind(file->F);
	FORCE_WRITE(file->F, &file->H, sizeof(file->H));
    	fclose(file->F);
    	free(file);
}

size_t alloc_block(M_FILE *file) {
	return file->H.n_blocks = file->H.n_blocks + 1;
}

bool read_block(M_FILE *file, size_t i, Block *buffer) {
    	if (i <= file->H.n_blocks) {
		fseek(file->F, i * sizeof(Block) + sizeof(file->H), SEEK_SET);
        	fread(buffer, sizeof(Block), 1, file->F);
        	N_readACC++;
		return true;
    	} else {
		fprintf(stderr, "ERROR: block index %zu out of range\n", i);
		return false;
    	}
}

bool write_block(M_FILE *file, size_t i, Block *buffer) {
    	if (i <= file->H.n_blocks) {
		fseek(file->F, i * sizeof(Block) + sizeof(file->H), SEEK_SET);
		FORCE_WRITE(file->F, buffer, sizeof(*buffer));
		N_writeACC++;
		return true;
    	} else {
		fprintf(stderr, "ERROR: block index %zu out of range\n", i);
		return false;
    	}
}

#define INPUT(type, prompt) _Generic((type)0, \
    size_t: input_size_t(prompt), \
    char*: input_string(prompt) \
)

size_t input_size_t(const char* prompt) {
	size_t val;
 	printf("%s", prompt);
    	if (scanf("%zu", &val) != 1) {
        	ERR_OUT("invalid size_t input");
	}
	return val;
}

char* input_string(const char* prompt) {
	static char buffer[256];
	printf("%s", prompt);
    	if (scanf("%255s", buffer) != 1) {
		ERR_OUT("invalid string input");
	}
	return buffer;
}
