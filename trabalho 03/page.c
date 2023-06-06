#include <stdlib.h>

#define FRAME_SIZE 8

typedef struct TPage *Page;

typedef struct TPage{
	Page next;
	int mod;
	int ref;
	int page_frame_bits[FRAME_SIZE];
	int id;
} TPage;

Page create_page(int id) {
	Page new_page = (Page)malloc(sizeof(TPage));

	new_page->ref = 0;
	new_page->mod = 0;
	new_page->id = id;
	int i = 0;
    while (id > 0) {
        new_page->page_frame_bits[i] = id % 2;
        id = id / 2;
        i++;
    }

	return new_page;
}


void reset_ref_bit(Page page){
	page->ref = 0;
}

void reset_mod_bit(Page page){
	page->mod = 0;
}

Page get_random_page_id(int seed_i, int seed_exp, int std_dev, int mean){
	int lower = mean - std_dev;
	int upper = mean + std_dev;
	srand(seed_i + seed_exp);
	return (rand() % (upper - lower + 1)) + lower;
}

void print_page(Page page){
	printf("modificado: %d\n", page->mod);
	printf("referenciado: %d\n", page->ref);
	printf("id: %d\n", page->id);
	printf("bits: ");
	for(int i = 0; i < FRAME_SIZE; i++) printf("%d ", page->page_frame_bits[i]);
	printf("\n");
}

void copy_page(Page from_page, Page to_page){
	to_page->ref = from_page->ref;
	to_page->mod = from_page->mod;
	to_page->id = from_page->id;
	for(int i = 0; i < FRAME_SIZE; i++) to_page->page_frame_bits[i] = from_page->page_frame_bits[i];
}