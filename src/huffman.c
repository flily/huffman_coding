#include "huffman.h"

int chartab_item_init(chartab_item_t* item, int chval)
{
    if (NULL == item)
        return -1;
    
    item->chval = chval;
    item->count = 0;
    return 0;
}

chartab_t* chartab_create(size_t charset_size)
{
    size_t i = 0;
    chartab_t* tab = (chartab_t*) malloc(sizeof(chartab_t));
    if (NULL == tab)
        return NULL;
    
    tab->items = (chartab_item_t*)
        malloc(charset_size * sizeof(chartab_item_t));
    if (NULL == tab->items)
    {
        free(tab);
        return NULL;
    }
    
    tab->size = charset_size;
    for (i = 0; i < charset_size; i++)
        chartab_item_init(&tab->items[i], (int) i);
    
    return tab;
}

void chartab_free(chartab_t* tab)
{
    if (NULL == tab)
        return;
    
    if (NULL != tab->items)
        free(tab->items);
    
    free(tab);
    return;
}

size_t chartab_char_increase(chartab_t* tab, int chval)
{
    chartab_item_t* item = NULL;
    
    if (NULL == tab || NULL == tab->items)
        return 0;
    
    if (chval < 0 || tab->size <= (size_t) chval)
        return 0;
    
    item = &tab->items[chval];
    if (item->chval != chval)
        return 0;
    
    item->count += 1;
    return item->count;
}

size_t chartab_char_set_count(chartab_t* tab, int chval, size_t count)
{
    chartab_item_t* item = NULL;
    
    if (NULL == tab || NULL == tab->items)
        return 0;
    
    if (chval < 0 || tab->size <= (size_t) chval)
        return 0;
    
    item = &tab->items[chval];
    if (item->chval != chval)
        return 0;
    
    item->count = count;
    return item->count;
}

chartab_t* chartab_read_from_file(FILE* fp)
{
    chartab_t* tab = NULL;
    if (NULL == fp)
        return NULL;
    
    tab = chartab_create(HUFFMAN_ASCII_BYTE_CHARTAB_SIZE);
    if (NULL == tab)
        return NULL;
    
    while (!feof(fp))
    {
        int c = fgetc(fp);
        chartab_char_increase(tab, c);
    }
    
    return tab;
}

huffman_tree_node_t* huffman_tree_node_init(int chval, size_t count)
{
    huffman_tree_node_t* node = (huffman_tree_node_t*)
        malloc(sizeof(huffman_tree_node_t));
    
    if (NULL == node)
        return NULL;
    
    node->chval = chval;
    node->count = count;
    node->depth = 0;
    node->lchild = NULL;
    node->rchild = NULL;
    
    return node;
}

huffman_tree_node_t* huffman_tree_node_create(const chartab_item_t* item)
{
    if (NULL == item)
        return NULL;
    
    return huffman_tree_node_init(item->chval, item->count);
}

void huffman_tree_node_free(huffman_tree_node_t* node)
{
    if (NULL == node)
        return;
    
    free(node);
    return;
}

huffman_tree_node_t* huffman_tree_node_combine(
    const huffman_tree_node_t* node1,
    const huffman_tree_node_t* node2
)
{
    huffman_tree_node_t* node = NULL;
    
    if (NULL == node1 || NULL == node2)
        return NULL;
    
    node = huffman_tree_node_init(0, node1->count + node2->count);
    if (NULL == node)
        return NULL;
    
    return node;
}

int huffman_tree_node_compare(
    const huffman_tree_node_t* node1,
    const huffman_tree_node_t* node2
)
{
    if (node1->count < node2->count)
        return 1;
    
    if (node1->count > node2->count)
        return -1;
    
    /* node1->count == node2->count */
    if (-1 == node1->chval && node2->chval >= 0)
        return -1;

    if (node1->chval >= 0 && -1 == node2->chval)
        return 1;

    if (node1->chval < node2->chval)
        return 1;
    
    if (node1->chval > node2->chval)
        return -1;
    
    return 0;
}


huffman_tree_t* huffman_tree_init(size_t size)
{
    size_t i = 0;
    huffman_tree_t* tree = (huffman_tree_t*) malloc(sizeof(huffman_tree_t));
    if (NULL == tree)
        return NULL;
    
    tree->size = size;
    tree->root = NULL;
    tree->tab = (huffman_tree_node_t**)
        malloc(size * sizeof(huffman_tree_node_t*));
    if (NULL == tree->tab)
    {
        free(tree);
        return NULL;
    }
    
    for (i = 0; i < size; i++)
        tree->tab[i] = NULL;
    
    return tree;
}

void huffman_tree_free(huffman_tree_t* tree)
{
    if (NULL != tree)
    {
        if (NULL != tree->tab)
            free(tree->tab);
        
        free(tree);
    }
    
    return;
}

int huffman_tree_add_char(huffman_tree_t* tree, const chartab_item_t* item)
{
    int chval = 0;
    huffman_tree_node_t* node = NULL;
    
    if (NULL == tree || NULL == item)
        return -1;
    
    chval = item->chval;
    if (chval < 0 || tree->size < (size_t) chval)
        return -2;
    
    if (NULL == tree->tab)
        return -3;
    
    node = huffman_tree_node_create(item);
    if (NULL == node)
        return -4;
    
    tree->tab[chval] = node;
    return 0;
}

huffman_tree_t* huffman_tree_create(const chartab_t* tab)
{
    huffman_tree_t* tree = NULL;
    size_t i = 0;
    
    if (NULL == tab)
        return NULL;
    
    tree = huffman_tree_init(tab->size);
    for (i = 0; i < tab->size; i++)
    {
        chartab_item_t* item = &tab->items[i];
        if (0 > huffman_tree_add_char(tree, item))
        {
            huffman_tree_free(tree);
            return NULL;
        }
    }
    
    return tree;
}

void huffman_tree_nodes_print(huffman_tree_node_t** nodes, size_t size)
{
    size_t i = 0;
    if (NULL == nodes)
        return;
    
    printf(">>>>> ");
    for (i = 0; i < size; i++)
    {
        const huffman_tree_node_t* n = nodes[i];
        if (NULL == n)
        {
            printf("[(%d) NULL] ", (int) i);
            continue;
        }
        else
        {
            int is_print = 1;
            char c = (char) n->chval;
            
            if (!isprint(c))
            {
                is_print = 0;
                c = '.';
            }
            
            printf("<(%d)%s'%c'(%d)-%d>  ",
                   (int) i,
                   is_print ? "c" : "i",
                   c,
                   (int) n->chval,
                   (int) n->count
                   );
        }
    }
    printf("\n");
    return;
}

int huffman_tree_sort_onepass(huffman_tree_node_t** nodes, size_t length)
{
    int i = (int) length - 1;
    
    if (NULL == nodes)
        return -1;
    
    if (NULL == nodes[i])
        return -2;
    
    for (i = (int) length - 1; i > 0; i--)
    {
        int j = i - 1;
        int r = 0;
        /* node in Head side and Tail side */
        huffman_tree_node_t* nh = nodes[i];
        huffman_tree_node_t* nt = nodes[j];
        
        if (NULL == nt)
            return -2;
        
        r = huffman_tree_node_compare(nh, nt);
        if (r < 0)
        {
            huffman_tree_node_t* t = nodes[i];
            nodes[i] = nodes[j];
            nodes[j] = t;
        }
        
    }
    
    return 0;
}

int huffman_tree_sort(huffman_tree_node_t** nodes, size_t length)
{
    size_t s = length;
    
    if (NULL == nodes)
        return -1;
    
    for (s = length; s > 0; s--)
    {
        huffman_tree_sort_onepass(nodes, length);
    }
    
    return 0;
}

int huffman_tree_nonzero_char_count(huffman_tree_t* tree)
{
    int count = 0;
    int i = 0;
    
    if (NULL == tree)
        return -1;
    
    if (NULL == tree->tab)
        return 0;
    
    for (i = 0; i < tree->size; i++)
    {
        huffman_tree_node_t* node = tree->tab[i];
        if (NULL != node)
        {
            if (node->count > 0)
                count++;
        }
    }
    
    return count;
}

int huffman_tree_build(huffman_tree_t* tree)
{
    huffman_tree_node_t** tab = NULL;
    int retval = 0;
    int ch_count = 0, last_chars = 0;
    int i = 0, j = 0;
    
    if (NULL == tree || NULL == tree->tab)
        return -1;
    
    ch_count = huffman_tree_nonzero_char_count(tree);
    tab = (huffman_tree_node_t**)
        malloc(ch_count * sizeof(huffman_tree_node_t*));
    
    if (NULL == tab)
        return -2;
    
    for (i = 0; i < ch_count; i++)
        tab[i] = NULL;
    
    j = 0;
    for (i = 0; i < tree->size; i++)
    {
        huffman_tree_node_t* node = tree->tab[i];
        if (NULL != node && node->count > 0)
        {
            printf("create node [%d](%d, %zu)\n", j, node->chval, node->count);
            tab[j++] = huffman_tree_node_init(node->chval, node->count);
        }
    }
    
    printf("First nodes\n");
    huffman_tree_nodes_print(tab, ch_count);
    
    for (last_chars = ch_count; last_chars > 1; last_chars--)
    {
        int last_i1 = last_chars - 1;
        int last_i2 = last_chars - 2;
        huffman_tree_node_t* last_n1 = NULL;
        huffman_tree_node_t* last_n2 = NULL;
        huffman_tree_node_t* new_last = NULL;
        
        huffman_tree_sort(tab, last_chars);
        printf("Sorted %d\n", last_chars);
        huffman_tree_nodes_print(tab, last_chars);
        
        last_n1 = tab[last_i1];
        last_n2 = tab[last_i2];
        new_last = huffman_tree_node_init(-1, last_n1->count + last_n2->count);
        if (NULL == new_last)
        {
            break;
        }
        
        new_last->lchild = last_n2;
        new_last->rchild = last_n1;
        tab[last_i2] = new_last;
    }
    
    printf("Final\n");
    huffman_tree_nodes_print(tab, last_chars);
    tree->root = tab[0];
    return retval;
}

int huffman_tree_show_code_recursive(
    huffman_tree_node_t* node,
    bincode_t* code
)
{
    if (NULL == node || NULL == code)
        return -1;
    
    if (NULL == node->lchild && NULL == node->rchild)
    {
        int bufsize = HUFFMAN_ASCII_BYTE_CHARTAB_SIZE + 16;
        char code_str[HUFFMAN_ASCII_BYTE_CHARTAB_SIZE + 16] = "";
        char ch = '.';
        
        if (isprint(node->chval))
            ch = (char) node->chval;

        bincode_get_string(code, code_str, bufsize);
        printf("(%d)'%c' [%d]: %s\n",
               (int) node->chval,
               ch,
               (int) node->count,
               code_str
        );
    }
    else
    {
        bincode_bit_append(code, BINCODE_0);
        huffman_tree_show_code_recursive(node->lchild, code);
        bincode_bit_pop(code);
        
        bincode_bit_append(code, BINCODE_1);
        huffman_tree_show_code_recursive(node->rchild, code);
        bincode_bit_pop(code);
    }
    
    return 0;
}

int huffman_tree_show_code(huffman_tree_t* tree)
{
    bincode_t* code = NULL;
    
    if (NULL == tree)
        return -1;
    
    if (NULL == tree->root)
    {
        printf("Tree not built yet\n");
        return 0;
    }
    
    code = bincode_create(DEFAULT_BINCODE_SIZE);
    huffman_tree_show_code_recursive(tree->root, code);
    bincode_free(code);
    
    return 0;
}

