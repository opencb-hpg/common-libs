
/**
 * graph.h
 *
 * Podemos dar la opcion a cambiar el tipo de grafo despues de crearlo,
 * es decir, si se permite el cambio automatico/manual de modo de grafo "si" "no" "depende"
 * 
 * 
 * //typedef int vertex_id; ??
 *
 * 
 * Hitos 2013-06-20:
 * 		Tipos de parametros (_s, _i, _...)
 * 		Elimina Goood
 * 		Parser salida Good	(con peso en aristas)
 * 		graph_go no recursivo
 * 
 * TODO:
 * 		Darle coherencia
 * 		check
 * 		Correcto sincronismo (sync_mode)
 * 
 * DUDAS:
 * 		#define GRAPH_STRICT			0b00010000
 * 		check no pasa el valgrind
 * 		add vertex sin nombre??
 *		opcion a dar el peso o no? add_edge_iw, add_edge_i, add_edge_sw, add_edge_w
 */

#ifndef _GRAPH_H_
#define _GRAPH_H_


#include "linked_list.h"
#include "array_list.h"
#include "containers.h"
#include "khash.h"

KHASH_MAP_INIT_STR(gr,int)


typedef struct edge {
	int src_id;
	int dst_id;
	void *data;
	float weight;
	//char* name;
	
} edge_t;

typedef int vertex_id_t;

typedef struct vertex {
	void *data;
	linked_list_t *src;
	linked_list_t *dst;
	linked_list_t *nd;
	char* name;
	vertex_id_t id;
} vertex_t;

typedef struct graph {
	khash_t(gr) *dict;
	array_list_t *vertices;
	linked_list_t *removed_vertices;
	int num_vertices;	//()
	int num_edges;		//()
	
	short int directed;	//si, no y mixto
	short int cyclic;
	short int multiple;
	short int strict;
	short int non_negative;
	//reemplazar todo por una mascara?
	
	int sync_mode;
} graph_t;



#define GRAPH_NON_DIRECTED			0b00000001
#define GRAPH_DIRECTED				0b00000010
#define GRAPH_MIXED_DIRECTED		0b00000011
#define GRAPH_CYCLIC				0b00000000
#define GRAPH_ACYCLIC				0b00000100
#define GRAPH_STRICT				0b00001000
#define GRAPH_NON_NEGATIVE_WEIGHT	0b00010000
//#define GRAPH_MULTIPLE			0b00010000

/**
 * Creation and initialization
 */
graph_t* graph_new(char mask, int initial_num_vertices, int SYNC_MODE);

/**
 * Destruction
 */

int graph_free(void (*vertex_data_callback) (void* vertex_data), void (*edge_data_callback) (void* edge_data), graph_t*);

int graph_clear(void (*vertex_data_callback) (void* vertex_data), void (*edge_data_callback) (void* edge_data), graph_t*);

/**
 * Vertex Functions
 */
int graph_add_vertex(char* name, void* vertex_data, graph_t*);

int graph_find_vertex(char* name, graph_t*);	// assumes only one ocurrence

int graph_exists_vertex_s(char* name, graph_t*);
int graph_exists_vertex_i(int id, graph_t*);
//contains??

int graph_reachable_vertex(int src, int dst, graph_t*);

vertex_t* graph_get_vertex_s(char* vertex_name, graph_t * graph_p);
vertex_t* graph_get_vertex_i(int vertex_id, graph_t * graph_p);

void* graph_get_vertex_data_s(char* name, graph_t*);
void* graph_get_vertex_data_i(int id, graph_t*);
/*
linked_list_t* graph_vertex_neighborhood_s(char* name, graph_t*);
linked_list_t* graph_vertex_neighborhood_i(int id, graph_t*);

linked_list_t* graph_vertex_neighborhood_out_s(char* name, graph_t*);
linked_list_t* graph_vertex_neighborhood_out_i(int id, graph_t*);

linked_list_t* graph_vertex_neighborhood_in_s(char* name, graph_t*);
linked_list_t* graph_vertex_neighborhood_in_i(int id, graph_t*);
//adjacent
*/

int graph_remove_vertex_s(char* vertex_name, void (*vertex_data_callback) (void* vertex_data),void (*edge_data_callback) (void* edge_data), graph_t*);
int graph_remove_vertex_i(int vertex_id, void (*vertex_data_callback) (void* vertex_data),void (*edge_data_callback) (void* edge_data), graph_t*);


/**
 * Edge Functions
 */
int graph_add_edge_s(char* src, char* dst, void* edge_data, char edge_type, graph_t*);
int graph_add_edge_i(int src, int dst, void* edge_data, char edge_type, graph_t*);
int graph_add_edge_sw(char* src, char* dst, void* edge_data, char edge_type, float weight, graph_t*);
int graph_add_edge_iw(int src, int dst, void* edge_data, char edge_type, float weight, graph_t*);

int graph_remove_edge_s(char* src, char* dst, char edge_type, void (*edge_data_callback) (void* edge_data), graph_t*);
int graph_remove_edge_i(int src, int dst, char edge_type, void (*edge_data_callback) (void* edge_data), graph_t*);
int graph_remove_edge_e(edge_t *edge_p, char edge_type, void (*edge_data_callback) (void* edge_data), graph_t*);

edge_t* graph_get_edge_s(char* src, char* dst, char edge_type, graph_t*);
edge_t* graph_get_edge_i(int src, int dst, char edge_type, graph_t*);

/**
 * Path Functions
 */
//linked_list_t* graph_path_get(int src, int dst, graph_t*);

/**
 * Others
 */
int graph_print(graph_t*);	
int graph_print_dot(char* file_name, graph_t*);
int graph_get_order (graph_t*);	// vertex number
int graph_get_size (graph_t*);	// edge number

#endif //_GRAPH_H_
