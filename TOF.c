#include <io.c>

// TOF: table, ordered, fixed sized records

struct Index {
	size_t index;
	size_t offset;
}

struct Record {
	size_t key;
}

bool binary_search(M_FILE *file, int key, struct Index *index) {
	size_t i;
	Buffer buf;

	for (i = 0; i < file->H.n_blocks; ++i) {
		read_block(file, i, &buf);
		Record *record = (Record*)buf.items[buf.count - 1];
		if (record->key >= key) break;
	}

	if (i == file->H.n_blocks) return false;

	size_t start = 0, end = buf.count - 1;
	while (start <= end) {
		size_t mid = (end + start) >> 1;
		size_t mid_key = buf->items[mid].key;
		if (mid_key < key) {
			end = mid - 1;
		} else if (key < mid_key) {
			start = mid + 1;
		} else {
			*index = (Index){ i, mid };
			return true;
		}
	}

	return false;
}

int main(void) {
	M_FILE *file = open("record.data", "A");
	Index index;
	size_t key = INPUT(size_t, "enter a key: ");
	if (binary_search(open, key, &index)) {
		printf("Found record with key %zu: (block: %zu, offset: %zu).\n", key, index.index, index.offset);
	} else {
		printf("Couldn't find record with key %zu.\n", key);
	}
	close(file);
}
