#ifndef __huffman__huffman_h__
#define __huffman__huffman_h__

#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>

#include "bitstream.h"

#define HUFFMAN_ASCII_BYTE_CHARTAB_SIZE 256

struct chartab_item_s
{
    int chval;
    size_t count;
};

typedef struct chartab_item_s chartab_item_t;

struct chartab_s
{
    size_t size;
    chartab_item_t* items;
};

typedef struct chartab_s chartab_t;

int chartab_item_init(chartab_item_t* item, int chval);

chartab_t* chartab_create(size_t charset_size);

void chartab_free(chartab_t* tab);

size_t chartab_char_increase(chartab_t* tab, int chval);

size_t chartab_char_set_count(chartab_t* tab, int chval, size_t count);

chartab_t* chartab_read_from_file(FILE* fp);

typedef struct huffman_tree_node_s huffman_tree_node_t;

struct huffman_tree_node_s
{
    int chval;
    size_t count;
    int depth;
    huffman_tree_node_t* lchild;
    huffman_tree_node_t* rchild;
};

struct huffman_tree_s
{
    huffman_tree_node_t* root;
    huffman_tree_node_t** tab;
    size_t size;
};

typedef struct huffman_tree_s huffman_tree_t;

huffman_tree_node_t* huffman_tree_node_init(int chval, size_t count);

huffman_tree_node_t* huffman_tree_node_create(const chartab_item_t* item);

void huffman_tree_node_free(huffman_tree_node_t* node);

huffman_tree_node_t* huffman_tree_node_combine(
    const huffman_tree_node_t* node1,
    const huffman_tree_node_t* node2
);

int huffman_tree_node_compare(
    const huffman_tree_node_t* node1,
    const huffman_tree_node_t* node2
);

huffman_tree_t* huffman_tree_init(size_t size);

void huffman_tree_free(huffman_tree_t* tree);

int huffman_tree_add_char(huffman_tree_t* tree, const chartab_item_t* item);

huffman_tree_t* huffman_tree_create(const chartab_t* tab);

void huffman_tree_nodes_print(huffman_tree_node_t** nodes, size_t size);

int huffman_tree_sort_onepass(huffman_tree_node_t** nodes, size_t length);

int huffman_tree_sort(huffman_tree_node_t** nodes, size_t length);

int huffman_tree_nonzero_char_count(huffman_tree_t* tree);

int huffman_tree_build(huffman_tree_t* tree);

int huffman_tree_show_code_recursive(
    huffman_tree_node_t* node,
    bincode_t* code
);

int huffman_tree_show_code(huffman_tree_t* tree);



#endif
