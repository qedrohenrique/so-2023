#include <stdio.h>
#include <string.h>
#include "list.c"

#define MAX_REP 1000
#define EXP_NUM 100
#define MV_SIZE 128
#define MR_SIZE 64
#define MS_SIZE 64

void fill_memory(List* mem, int mem_size, int page_id, int start){
	page_id = page_id + start;
    for(int i = start; i < mem_size + start; i++) {
        list_insert(mem, create_page(page_id));
        page_id += 1;
    }
}

Page trap_fifo(List *mr, List* ms, int id){
	Page ms_page = list_search_page(ms , id);
	Page aux_page = create_page(-1);
	Page mr_page = mr->head;

 	copy_page(ms_page, aux_page);
	copy_page(mr_page, ms_page);
	list_remove_head(mr);
	list_insert(mr, aux_page);

	return aux_page;
}

Page trap_second_chance(List *mr, List* ms, int id){
	Page ms_page = list_search_page(ms , id);
	Page aux_page = create_page(-1);
	Page mr_page = mr->head;

	int i = 0;

	while(i++ < mr->length){
		if(mr_page->ref == 1){
			list_insert(mr, create_page(mr_page->id)); // ao fazer isso, resetamos mr_page->ref e mr_page->mod para 0.
			list_remove_head(mr);
		}else{
			
			copy_page(mr_page, aux_page);
			copy_page(ms_page, mr_page);
			copy_page(aux_page, ms_page);

			return mr_page;
		}
		mr_page = mr_page->next;
	}

	mr_page = mr->head;
	copy_page(ms_page, aux_page);
	copy_page(mr_page, ms_page);
	list_remove_head(mr);
	list_insert(mr, aux_page);

	return aux_page;
}

Page trap_nur(List *mr, List* ms, int id){
	Page ms_page = list_search_page(ms , id);
	Page aux_page = create_page(-1);
	int i;
	// CLASSE 0

	Page mr_page = mr->head;
	List* aux_list = list_init();
	i = 0;
	while(i++ < mr->length){
		if(mr_page->ref == 0 && mr_page->mod == 0){
			list_insert(aux_list, create_page(mr_page->id));
		}
		mr_page = mr_page->next;
	}

	if(aux_list->length != 0){	
		Page page = aux_list->head;
		srand(0);
		int num = rand() % aux_list->length;
		for(int i = 0; i < num-1; i++) page = page->next;
		mr_page = list_search_page(mr, page->id);

		copy_page(mr_page, aux_page);
		copy_page(ms_page, mr_page);
		copy_page(aux_page, ms_page);
		return mr_page;
	}

	// CLASSE 1

	mr_page = mr->head;
	aux_list = list_init();
	i = 0;
	while(mr_page != NULL){
		if(mr_page->ref == 1 && mr_page->mod == 0){
			list_insert(aux_list, create_page(mr_page->id));
		}
		mr_page = mr_page->next;
	}

	if(aux_list->length != 0){
		Page page = aux_list->head;
		srand(0);
		int num = rand() % aux_list->length;
		for(int i = 0; i < num; i++) page = page->next;

		mr_page = list_search_page(mr, page->id);

		copy_page(mr_page, aux_page);
		copy_page(ms_page, mr_page);
		copy_page(aux_page, ms_page);
		return mr_page;
	}


	// CLASSE 2

	mr_page = mr->head;
	aux_list = list_init();
	i = 0;
	while(mr_page != NULL){
		if(mr_page->ref == 0 && mr_page->mod == 1){
			list_insert(aux_list, create_page(mr_page->id));
		}
		mr_page = mr_page->next;
	}

	if(aux_list->length != 0){
		Page page = aux_list->head;
		srand(0);
		int num = rand() % aux_list->length;
		for(int i = 0; i < num; i++) page = page->next;

		mr_page = list_search_page(mr, page->id);

		copy_page(mr_page, aux_page);
		copy_page(ms_page, mr_page);
		copy_page(aux_page, ms_page);

		return mr_page;
	}


	// CLASSE 3

	mr_page = mr->head;

		srand(0);
		int num = rand() % mr->length;
		for(int i = 0; i < num; i++) mr_page = mr_page->next;
		copy_page(mr_page, aux_page);
		copy_page(ms_page, mr_page);
		copy_page(aux_page, ms_page);	
		return mr_page;	
}

int main(int argc, char** argv){

	int std_dev = atoi(argv[1]);
	int mean = atoi(argv[2]);

	if(mean > MV_SIZE || mean < 0 || std_dev > MV_SIZE/2 || std_dev < 0 || mean + std_dev > MV_SIZE || mean - std_dev < 0){
		printf("Parametros invalidos.\n");
		return 0;
	}

	int i, exp_num = 0;
	double exp_mean = 0;

	for(exp_num = 0; exp_num < EXP_NUM; exp_num++){

		int page_miss = 0, page_id = 1; 

		List *MV, *MR, *MS;

		MV = list_init();
		MR = list_init();
		MS = list_init();

		fill_memory(MV, MV_SIZE, page_id, 0);
		fill_memory(MR, MR_SIZE, page_id, 0);
		fill_memory(MS, MS_SIZE, page_id, 64);

		/*
			Nesse ponto as memorias contem esses ids, entao, se uma MV de id 65 for acessada, um evento de page miss ocorreria.
			MV: 1 -> 128
			MR: 1 -> 64
			MS: 64 -> 128
		*/

		for(i = 0; i < MAX_REP; i++){
				int pag_virutal = get_random_page_id(i, exp_num, std_dev, mean);
				
				Page pag_real = list_search_page(MR, pag_virutal);
				
				if(pag_real == NULL){
					pag_real = trap_fifo(MR, MS, pag_virutal);
					page_miss++;
				}

				list_ref_page(MR, pag_real->id);
				if(i%10==0)list_mod_page(MR, pag_real->id);
				if(i%100==0)list_reset_ref_bits(MS);
		}

		printf("Page miss: %d\n", page_miss);
		exp_mean += page_miss;
	}

	printf("Exp mean: %f\n", exp_mean/EXP_NUM);

	return 0;
}
