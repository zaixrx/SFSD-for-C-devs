#include "io.c"

typedef struct {
	size_t id;
	char name[64];
	bool is_alive;
} Student;

int search(TNOF *file, size_t idx, size_t *i, size_t *j) {
	Block buf = {0};
	for (*i = 0; *i < file->H.n_blocks; ++*i) {
		read_block(file, *i, &buf);
		for (*j = 0; *j < buf.count; ++*j) {
			if (((Student*)buf.items[*i])->id == idx) return true;
		}
	}
	return false;
}

bool deletep(TNOF *file, size_t idx) {
	size_t i, j;
	Block buf1 = {0}, buf2 = {0};
	if (!search(file, idx, &i, &j)) {
		fprintf(stderr, "ERROR: entry in file not found\n");
		return false;
	}
	read_block(file, i, &buf1);
	read_block(file, file->H.n_blocks - 1, &buf2);
	memcpy(buf1.items[i], buf2.items[buf2.count - 1], sizeof(buf2));
	return write_block(file, i, &buf1);
}

bool deletel(TNOF *file, size_t idx) {
	size_t i, j;
	Block buf = {0};
	if (!search(file, idx, &i, &j)) {
		fprintf(stderr, "ERROR: entry in file not found\n");
		return false;
	}
	read_block(file, i, &buf);
	((Student*)buf.items[i])->is_alive = false;
	return write_block(file, i, &buf);
}

void insert(TNOF *file, Block *buffer, void *data) {
	if (buffer->count >= BLOCK_CAPACITY) {
		size_t i = alloc_block(file);
		write_block(file, i, buffer);
		buffer->count = 0;
	}
	buffer->items[buffer->count++] = data;
}

Student *read_student() {
	Student *s = (Student*)malloc(sizeof(Student));
	printf("Enter student key: ");
	scanf("%zu", &s->id);
	printf("Enter name: ");
	scanf("%s", s->name);
	getc(stdin);
	s->is_alive = true;
	return s;
}

char read_char() {
	char c = getc(stdin); 
	if (c == '\n') return '\0';
	getc(stdin);
	return c;
}

int should_continue() {
	printf("Continue? (y/N) ");
	return read_char() == 'y';
}

TNOF *load_into(const char *file_path) {
    	TNOF *file = open(file_path, 'N');
    	Block buffer = {0};
	for (;;) {
		Student *s = read_student();
		insert(file, &buffer, s);
		if (!should_continue()) break;
	}
    	if (buffer.count > 0) write_block(file, alloc_block(file), &buffer);
	return file;
}

void fragmentation(TNOF *file, size_t c1, size_t c2) {
	TNOF *f1 = open("fragment1.data", 'N');
	TNOF *f2 = open("fragment2.data", 'N');
	TNOF *f3 = open("fragment3.data", 'N');

	Block buf, buf1 = {0}, buf2 = {0}, buf3 = {0};
    	for (size_t i = 0; i < file->H.n_blocks; i++) {
		read_block(file, i, &buf);
		for (size_t j = 0; j < buf.count; j++)  {
			Student *s = (Student*)buf.items[j];
			if (!s->is_alive) continue;

			if (s->id < c1) {
				insert(f1, &buf1, s);
			} else if (s->id >= c1 && s->id < c2) {
				insert(f2, &buf2, s);
			} else if (s->id >= c2) {
				insert(f3, &buf3, s);
			}
		}
    	}

    	if (buf1.count > 0) write_block(f1, alloc_block(file), &buf1);
    	if (buf2.count > 0) write_block(f2, alloc_block(file), &buf2);
    	if (buf3.count > 0) write_block(f3, alloc_block(file), &buf3);

    	close(f1);
    	close(f2);
    	close(f3);
}

int main(void) {
	// disables line buffering on all standard streams
	setvbuf(stdout, NULL, _IONBF, 0);
	setvbuf(stdin , NULL, _IONBF, 0);
	setvbuf(stderr, NULL, _IONBF, 0);

	printf("=== START_Init === \n");
	const char *path = "students.data";
	TNOF *file = load_into(path);
	printf("=== END_Init === \n");

	printf("=== START_Fragmentation === \n");
	size_t c1, c2;
	printf("Enter the first key: "); scanf("%zu", &c1);
	printf("Enter the second key: "); scanf("%zu", &c2);
	fragmentation(file, c1, c2);
	printf("=== END_Fragmentation === \n");

	close(file);
}
