#include "io.c"

typedef struct {
	bool is_deleted;
} Record;

void GC_pass(TNOF *f) {
	Block b_iter = {0};
	Block b_load = {0};
	size_t n_blocks = 0;
	for (size_t i = 0; i < f->H.n_blocks; ++i) {
		read_block(f, i, &b_iter);
		for (size_t j = 0; j < b_iter.count; ++j) {
			Record *r = (Record*)b_iter.items[j];
			if (r->is_deleted) continue; // skipi 3abd l7amid
			if (b_load.count > ARR_LEN(b_load.items)) {
				write_block(f, n_blocks++, &b_load);
				b_load.count = 0;
			}
			b_load.items[b_load.count++] = r;
		}
	}
	if (b_load.count > 0) {
		write_block(f, n_blocks++, &b_load);
		b_load.count = 0;
	}
	f->H.n_blocks = n_blocks;
}

int main(void) {
	// TODO: test
}
