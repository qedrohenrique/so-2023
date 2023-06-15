#include "page.c"
#include <stdlib.h>

typedef struct {
  Page head;
  int length;
} List;

List *list_init() {
    List *list = (List*)malloc(sizeof(List));
    list->head = NULL;
    list->length = 0;
    return list;
}

int list_insert(List *list, Page item) {
    Page curr = list->head;

    if(curr == NULL) {
        list->head = item;
    } else {
        while(curr->next != NULL) curr = curr->next;
        curr->next = item;
    }

    list->length++;

    return 1;
}

int list_remove_any(List *list, Page item) {
    Page curr = list->head;

    if(curr == NULL) return 0;

    if(item == list->head) {
        if(list->length > 1) list->head = list->head->next;
        else list->head = NULL;
    } else {
        while(curr->next != item) curr = curr->next;
        curr->next = item->next;
    }

    item->next = NULL;
    list->length--;

    return 1;
}

int list_remove_head(List *list) {

    if(list->head == NULL) return 0;

    if(list->length > 1) list->head = list->head->next;
    else list->head = NULL;

    list->length--;

    return 1;
}

int list_has_item(List *list, int id) {
    Page curr = list->head;

    while(curr->next != NULL) {
        if(curr->id == id) return 1;
        curr = curr->next;
    }

    return (curr->id == id) ? 1 : 0;
}

Page list_search_page(List *list, int id) {
    Page curr = list->head;

    while(curr->next != NULL) {
        if(curr->id == id) return curr;
        curr = curr->next;
    }

    return (curr->id == id) ? curr : NULL;
}

void list_ref_page(List *list, int id){
    Page page = list_search_page(list, id);
    page->ref = 1;
}

void list_mod_page(List *list, int id){
    Page page = list_search_page(list, id);
    page->mod = 1;
}

void list_reset_ref_bits(List *list){
    Page curr = list->head;

    while(curr != NULL){
        curr->ref = 0;
        curr = curr->next;
    }
}

void list_print(List *list) {
    Page curr = list->head;

    while(curr != NULL) {
        printf("%d -> ", curr->id);
        curr = curr->next;
    }

    printf("null\n");
}

void list_find_and_print(List *list, int id){
    Page p = list_search_page(list, id);
    print_page(p);
}