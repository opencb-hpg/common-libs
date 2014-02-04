#include "graph.h"

//TODO
//  Test if graph_p != NULL

#define GRAPH_MAX_NAME_LENGTH 256


/**
 * Creation and initialization
 */
graph_t* graph_new(char mask, int initial_num_vertices, int SYNC_MODE)
{
	graph_t *g = (graph_t*)malloc(sizeof(graph_t));
	
	if((mask & GRAPH_MIXED_DIRECTED) == GRAPH_MIXED_DIRECTED)
		g->directed = GRAPH_MIXED_DIRECTED;
	else if(mask & GRAPH_DIRECTED)
		g->directed = GRAPH_DIRECTED;
	else
		g->directed = GRAPH_NON_DIRECTED;	//default value
	
	g->cyclic = (mask & GRAPH_ACYCLIC)? 0: 1;
	//g->multiple = (mask & GRAPH_MULTIPLE)? 1: 0;
	g->multiple = 0;
	g->strict = mask & GRAPH_STRICT;
	g->non_negative = mask & GRAPH_NON_NEGATIVE_WEIGHT;
	
	g->num_edges = 0;
	g->num_vertices = 0;
	
	g->sync_mode = SYNC_MODE;
	g->vertices = array_list_new(initial_num_vertices,1.5,SYNC_MODE);
	g->removed_vertices = linked_list_new (SYNC_MODE);
	g->dict = kh_init(gr);
	
	return g;
	
}
/**
 * Destruction
 */

int graph_free(void (*vertex_data_callback) (void* vertex_data), void (*edge_data_callback) (void* edge_data), graph_t* graph_p)
{
	graph_clear(vertex_data_callback,edge_data_callback,graph_p);
	array_list_free(graph_p->vertices, NULL);
	linked_list_free(graph_p->removed_vertices, NULL);
	kh_destroy(gr, graph_p->dict);
	free(graph_p);
}


int graph_clear(void (*vertex_data_callback) (void* vertex_data), void (*edge_data_callback) (void* edge_data), graph_t* graph_p)
{
	int i;
	vertex_t *v;
	edge_t* e;
	linked_list_iterator_t *iter;
	
	if(graph_p->num_vertices)
		iter = (linked_list_iterator_t *)malloc(sizeof(linked_list_iterator_t));
	
	for (i = 0; i < graph_p->num_vertices; i++)
	{
		
		v = (vertex_t*)array_list_get(i, graph_p->vertices);
		if(v->src == NULL){
			free(v);
			continue;
		}
		if(edge_data_callback)
		{
			iter = linked_list_iterator_init(v->dst, iter);	// free dst data
			e = (edge_t*) linked_list_iterator_curr(iter);
			
			while (e != NULL)
			{
				edge_data_callback(e->data);
				e = (edge_t*)linked_list_iterator_next(iter);
			}
			
			
			iter = linked_list_iterator_init(v->nd, iter);	// free nd data and edge (only when e->src_id==-1, else sets to -1)
			e = (edge_t*) linked_list_iterator_curr(iter);
			while (e != NULL)
			{
				if(e->src_id != -1)
					e->src_id = -1;
				else{
					edge_data_callback(e->data);
					free(e);
				}
				e = (edge_t*)linked_list_iterator_next(iter);
				
			}
		}
		else
		{
			iter = linked_list_iterator_init(v->nd, iter); // free nd edge (only when e->src_id==-1, else sets to -1)
			e = (edge_t*) linked_list_iterator_curr(iter);
			while (e != NULL)
			{
				if(e->src_id != -1)
					e->src_id = -1;
				else
					free(e);
				
				e = (edge_t*)linked_list_iterator_next(iter);
			}
		}
		
		linked_list_free(v->src, NULL);
		linked_list_free(v->dst, free);	// free dst edge
		linked_list_free(v->nd, NULL);
		
		free(v->name);
		if(vertex_data_callback)
			vertex_data_callback(v->data);
			
		free(v);
	
		
	}
	if(graph_p->num_vertices)
		linked_list_iterator_free(iter);
	array_list_clear(graph_p->vertices, NULL);
	linked_list_clear(graph_p->removed_vertices, NULL);
	
	kh_clear(gr, graph_p->dict);
	graph_p->num_edges = 0;
	graph_p->num_vertices = 0;

}

/**
 * Vertex Functions
 */
 

int graph_find_vertex(char* name, graph_t* graph_p){
	
	khiter_t k = kh_get(gr,graph_p->dict,name);
	
	if(k == kh_end(graph_p->dict))	//If it was not found, ret -1
		return -1;
	else
		return kh_value(graph_p->dict, k);
}

vertex_t* graph_get_vertex_s(char* vertex_name, graph_t * graph_p)
{
	int id = graph_find_vertex(vertex_name, graph_p);
	if(id < 0)
		return NULL;
	else
		return graph_get_vertex_i(id, graph_p);
}
vertex_t* graph_get_vertex_i(int vertex_id, graph_t * graph_p)
{
	vertex_t* v = NULL;
	
	if (vertex_id < graph_p->num_vertices && vertex_id >= 0)
	{
		v = (vertex_t*)array_list_get(vertex_id, graph_p->vertices);
		if (v != NULL)
			if (v->src == NULL)	// v is in the removed_vertex list
				v = NULL;
	}
	return v;
}

int graph_exists_vertex_s(char* name, graph_t* graph_p)
{
	int id = graph_find_vertex(name, graph_p);
	if(id >= 0)
	{
		return graph_exists_vertex_i(id, graph_p);
	}
	else
		return -1;
}

int graph_exists_vertex_i(int id, graph_t* graph_p)
{
	if( (id < graph_p->num_vertices || id >= 0 ) && graph_get_vertex_i(id, graph_p) != NULL)
		return 0;
	else
		return -1;
}


int graph_reachable_vertex(int src, int dst, graph_t* graph_p){
	
	if(src == dst)
		return 0;
	
	linked_list_t* queue = linked_list_new(graph_p->sync_mode);
	vertex_t* v, *v_dst;
	edge_t *e;
	linked_list_iterator_t *iter = (linked_list_iterator_t*)malloc(sizeof(linked_list_iterator_t));
	int i, fin = 0, dst_id;
	char *c = (char*)calloc(graph_p->num_vertices, sizeof(char));

	v = graph_get_vertex_i(src, graph_p);
	v_dst = graph_get_vertex_i(dst, graph_p);
	if(v != NULL && v_dst != NULL)
	{
		linked_list_insert_last(v, queue);
		c[v->id] = 1;
		while(v = (vertex_t*)linked_list_remove_first(queue))
		{
			linked_list_iterator_init(v->dst, iter);
			e = (edge_t*)linked_list_iterator_curr(iter);
			while(e != NULL)
			{
				if(c[e->dst_id]==0){
					if(e->dst_id == dst){
						fin = 1;
						break;
					}
					linked_list_insert_last(graph_get_vertex_i(e->dst_id, graph_p),queue);
					c[e->dst_id] = 1;
				}
				e=(edge_t*)linked_list_iterator_next(iter);
			}
			if(!fin){
				linked_list_iterator_init(v->nd, iter);
				e = (edge_t*)linked_list_iterator_curr(iter);
				while(e != NULL)
				{
					dst_id = (e->dst_id != v->id)? e->dst_id: e->src_id; 
					if(c[dst_id]==0){
						if(dst_id == dst){
							fin = 1;
							break;
						}
						linked_list_insert_last(graph_get_vertex_i(dst_id, graph_p),queue);
						c[dst_id] = 1;
					}
					e=(edge_t*)linked_list_iterator_next(iter);
				}
			}
		}
	}
	linked_list_iterator_free(iter);
	linked_list_free(queue, NULL);
	free(c);
	return fin-1;
}


/*!
 * @abstract
 * 
 * @return	>= 0 : vertex index
 * 			-1   : Error at array_list_insert
 * 			-2   : Error at kh_put. It already exists on the hash_table
 * 			-3   : Error name == NULL
 */
int graph_add_vertex(char* name, void* vertex_data, graph_t* graph_p){
	
	vertex_t *v;
	
	if (name==NULL)
		return -3;
	
	if (linked_list_size(graph_p->removed_vertices))
	{
		v = linked_list_get_first(graph_p->removed_vertices);
		linked_list_remove_first(graph_p->removed_vertices);
	}
	else
	{
		v = (vertex_t*)malloc(sizeof(vertex_t));
		if(array_list_insert(v, graph_p->vertices) == 0){
			free(v);
			return -1;
		}
		
		graph_p->num_vertices++;
		v->id = array_list_size(graph_p->vertices)-1;
	}
	//int id = graph_find_vertex(name, g); // Even if it already exists, it has to create a new one
	
	 
	int ret;
	khiter_t k = kh_put(gr, graph_p->dict, name, &ret);
	if (!ret){
		free(v);
		kh_del(gr, graph_p->dict, k);
		return -2;
	}
	//Created && appended OK

	
	v->data = vertex_data;
	
	v->src = linked_list_new(graph_p->sync_mode);
	v->dst = linked_list_new(graph_p->sync_mode);
	v->nd = linked_list_new(graph_p->sync_mode);
	
	int length = strnlen(name,GRAPH_MAX_NAME_LENGTH)+1;
	v->name = (char*)malloc(length);
	strncpy(v->name, name, length);
	
	kh_value(graph_p->dict,k) = v->id;
	
	return v->id;
}


/**
 * @return	0	OK
 * 			-1	Not existing vertex
 */
int graph_remove_vertex_s(char* vertex_name, void (*vertex_data_callback) (void* vertex_data),void (*edge_data_callback) (void* edge_data), graph_t* graph_p)
{
	int vertex_id = graph_find_vertex(vertex_name,graph_p);
	return graph_remove_vertex_i(vertex_id, vertex_data_callback, edge_data_callback, graph_p);
}
int graph_remove_vertex_i(int vertex_id, void (*vertex_data_callback) (void* vertex_data),void (*edge_data_callback) (void* edge_data), graph_t* graph_p)
{
	if(vertex_id < 0 || vertex_id >= graph_p->num_vertices)
		return -1;
	
	vertex_t *v = graph_get_vertex_i(vertex_id, graph_p);
	if(v == NULL)
		return 0;
	linked_list_iterator_t *iter = linked_list_iterator_new(v->src);
	edge_t *e = (edge_t*)linked_list_iterator_curr(iter);
	while(e != NULL)
	{
		linked_list_iterator_next(iter);
		graph_remove_edge_e(e, GRAPH_DIRECTED, edge_data_callback, graph_p);
		e = (edge_t*)linked_list_iterator_curr(iter);
	}
	
	iter = linked_list_iterator_init(v->dst, iter);
	e = linked_list_iterator_curr(iter);
	while(e != NULL)
	{
		linked_list_iterator_next(iter);
		graph_remove_edge_e(e, GRAPH_DIRECTED, edge_data_callback, graph_p);
		e = (edge_t*)linked_list_iterator_curr(iter);
	}
	
	iter = linked_list_iterator_init(v->nd, iter);
	e = linked_list_iterator_curr(iter);
	while(e != NULL)
	{
		linked_list_iterator_next(iter);
		graph_remove_edge_e(e, GRAPH_NON_DIRECTED, edge_data_callback, graph_p);
		e = (edge_t*)linked_list_iterator_curr(iter);
	}
	linked_list_iterator_free(iter);
	
	linked_list_free(v->src, NULL);
	linked_list_free(v->dst, NULL);	// free dst edge
	linked_list_free(v->nd, NULL);
	v->src = v->dst = v->nd = NULL;
	
	kh_del(gr,graph_p->dict, kh_get(gr,graph_p->dict, v->name));
	
	free(v->name);
	v->name = NULL;
	if(vertex_data_callback)
		vertex_data_callback(v->data);
	v->data = NULL;
	linked_list_insert(v, graph_p->removed_vertices);
	
	return 0;
}

/**
 * Edge Functions
 */

int graph_add_edge_i(int src, int dst, void* edge_data, char edge_type, graph_t* graph_p)
{
	return graph_add_edge_iw(src, dst, edge_data, edge_type, 1, graph_p);
}
int graph_add_edge_s(char* src, char* dst, void* edge_data, char edge_type, graph_t* graph_p)
{
	return graph_add_edge_sw(src, dst, edge_data, edge_type, 1, graph_p);
}
int graph_add_edge_sw(char* src, char* dst, void* edge_data, char edge_type, float weight, graph_t* graph_p)
{
	int s = graph_find_vertex(src,graph_p);
	int d = graph_find_vertex(dst,graph_p);
	return graph_add_edge_iw(s, d, edge_data, edge_type, weight, graph_p);
}
 
/**
 * 
 * 
 * @return 0    : OK
 * 			-1   : src or dst are not in graph
 * 			-2   : edge_type non supported
 * 			-3   : edge_type non compatible with the graph directed type
 * 			-4   : edge breaks acyclity
 * 			-5   : edge breaks multiplicity
 * 			-6   : Weight must be possitive
 */
int graph_add_edge_iw(int src, int dst, void* edge_data, char edge_type, float weight, graph_t* graph_p){
	
	//if(array_list_size(graph_p->vertices) <= src || array_list_size(graph_p->vertices) <= dst || src < 0 || dst < 0)
	//	return -1;
	if (graph_get_vertex_i(src, graph_p) == NULL || graph_get_vertex_i(src, graph_p) == NULL)
		return -1;
	
	
	edge_t * e;
	
	//if(!graph_p->multiple)
		if(graph_get_edge_i(src,dst,edge_type, graph_p) != NULL)
			return -5;
	
	if(!graph_p->cyclic && graph_p->strict){//if acyclic
		if(edge_type == GRAPH_NON_DIRECTED)
			if(graph_reachable_vertex(src, dst, graph_p)==0)
				return -4;
		if(graph_reachable_vertex(dst, src, graph_p)==0)
			return -4;
	}

	if(graph_p->non_negative && weight < 0)	//FIXME weight < 0 || weight <= 0 ??
		return -6;

	//if(graph_exists_vertex_i(src, graph_p) < 0 || graph_exists_vertex_i(dst, graph_p) < 0 )
	//	return -6;
	
	e = (edge_t*)malloc(sizeof(edge_t));
	e->src_id = src;
	e->dst_id = dst;
	e->data = edge_data;
	e->weight = weight;
	
	switch(edge_type)// TODO comprobacion multiple...
	{
		case GRAPH_DIRECTED:
			if(graph_p->directed == GRAPH_NON_DIRECTED && graph_p->strict == GRAPH_STRICT){
				free(e);
				return -3;
			}

			linked_list_insert(e,(graph_get_vertex_i(src,graph_p))->dst);
			linked_list_insert(e,(graph_get_vertex_i(dst,graph_p))->src);
			break;
		case GRAPH_NON_DIRECTED:
			if(graph_p->directed == GRAPH_DIRECTED && graph_p->strict == GRAPH_STRICT){
				free(e);
				return -3;
			}
			linked_list_insert(e,(graph_get_vertex_i(src,graph_p))->nd);
			linked_list_insert(e,(graph_get_vertex_i(dst,graph_p))->nd);

			break;
		default:
			free(e);
			return -2;
	}
	graph_p->num_edges++;
	return 0;

}



edge_t* graph_get_edge_s(char* src, char* dst, char edge_type, graph_t* graph_p)
{
	int s = graph_find_vertex(src,graph_p);
	int d = graph_find_vertex(dst,graph_p);
	if(s >= 0 && d >= 0)	//opt
		return graph_get_edge_i(s,d,edge_type, graph_p);
	else
		return NULL;
}


edge_t* graph_get_edge_i(int src, int dst, char edge_type, graph_t* graph_p)
{
	linked_list_iterator_t *iter;
	
	if(graph_exists_vertex_i(src, graph_p) < 0 || graph_exists_vertex_i(dst, graph_p) < 0 )
		return NULL;
	
	vertex_t *v = graph_get_vertex_i(src,graph_p);
	edge_t *e;
	if(edge_type == GRAPH_DIRECTED)
	{
		iter = linked_list_iterator_new(v->dst);
		e = (edge_t*)linked_list_iterator_curr(iter);
		while(e != NULL)
		{
			if(e->dst_id == dst)
			{
				linked_list_iterator_free(iter);
				return e;
			}
			e = (edge_t*)linked_list_iterator_next(iter);
		}
		linked_list_iterator_free(iter);
	}
	else if(edge_type == GRAPH_NON_DIRECTED){
		int nd_dst;
		
		iter = linked_list_iterator_new(v->nd);
		e = (edge_t*)linked_list_iterator_curr(iter);
		while(e != NULL)
		{
			nd_dst = (e->src_id==src)? e->dst_id: e->src_id;
			if(nd_dst == dst)
			{
				linked_list_iterator_free(iter);
				return e;
			}
			e = (edge_t*)linked_list_iterator_next(iter);
		}
		linked_list_iterator_free(iter);
	}
	return NULL;
}


/*linked_list_t* graph_get_edge(int src, int dst, char edge_type, graph_t* graph_p)
{
	linked_list_t *l = linked_list_new(graph_p->sync_mode);
	if(array_list_size(graph_p->vertices) <= src || array_list_size(graph_p->vertices) <= dst || src < 0 || dst < 0)
		return l;
	
	linked_list_iterator_t *iter;
	
	vertex_t *v = (vertex_t*)graph_get_vertex_i(src,graph_p);
	edge_t *e;
	if(edge_type & GRAPH_DIRECTED)
	{
		iter = linked_list_iterator_new(v->dst);
		e = (edge_t*)linked_list_iterator_curr(iter);
		while(e != NULL)
		{
			if(e->dst_id == dst)
			{
				linked_list_insert(e->data,l);
			}
			e = (edge_t*)linked_list_iterator_next(iter);
		}
		linked_list_iterator_free(iter);
	}
	if(edge_type & GRAPH_NON_DIRECTED)
	{
		int nd_dst;
		
		iter = linked_list_iterator_new(v->nd);
		e = (edge_t*)linked_list_iterator_curr(iter);
		while(e != NULL)
		{
			nd_dst = (e->src_id==src)? e->dst_id: e->src_id;
			if(nd_dst == dst)
			{
				linked_list_insert(e->data,l);
			}
			e = (edge_t*)linked_list_iterator_next(iter);
		}
		linked_list_iterator_free(iter);
	}
	return l;
}
*/


/**
 * @return	0	OK
 * 			-1	Edge doesn't exist
 * 			-2	Corrupted edge
 */
int graph_remove_edge_s(char* src, char* dst, char edge_type, void (*edge_data_callback) (void* edge_data), graph_t* graph_p)
{
	edge_t* e = graph_get_edge_s( src, dst, edge_type, graph_p);
	if(e!=NULL)
		return graph_remove_edge_e(e,edge_type, edge_data_callback, graph_p);
	else
		return -1;
}
int graph_remove_edge_i(int src, int dst, char edge_type, void (*edge_data_callback) (void* edge_data), graph_t* graph_p)
{
	
	edge_t *e = graph_get_edge_i( src, dst, edge_type, graph_p);
	
	if(e!=NULL)
		return graph_remove_edge_e(e,edge_type, edge_data_callback, graph_p);
	else
		return -1;
}
int graph_remove_edge_e(edge_t *edge_p, char edge_type, void (*edge_data_callback) (void* edge_data), graph_t* graph_p)
{
	if(edge_p == NULL)
		return -1;
	
	
	linked_list_iterator_t *iter;
	vertex_t *v_src = graph_get_vertex_i(edge_p->src_id, graph_p);
	vertex_t *v_dst = graph_get_vertex_i(edge_p->dst_id, graph_p);
	if(v_src == NULL || v_dst == NULL)
		return -2;
	if(edge_type & GRAPH_DIRECTED){
		linked_list_remove(edge_p, v_src->dst);
		linked_list_remove(edge_p, v_dst->src);
	}
	else if(edge_type & GRAPH_NON_DIRECTED)
	{
		linked_list_remove(edge_p, v_src->nd);
		linked_list_remove(edge_p, v_dst->nd);
	}
	if(edge_data_callback)
		edge_data_callback(edge_p->data);
	free(edge_p);
	graph_p->num_edges--;
	return 0;
}




/**
 * Others
 */

int graph_print(graph_t* graph_p)
{
	int i;
	vertex_t *v;
	edge_t* e;
	linked_list_iterator_t *iter;
	if(graph_p->num_vertices)
		iter = (linked_list_iterator_t *)malloc(sizeof(linked_list_iterator_t));
	
	for (i = 0; i < graph_p->num_vertices; i++)
	{
		v = graph_get_vertex_i(i, graph_p);
		if(v == NULL)
			continue;
		printf("%s _____ id(%d)\n   src\n", v->name, v->id);
		
		iter = linked_list_iterator_init(v->src, iter);
		e = (edge_t*) linked_list_iterator_curr(iter);
		while (e != NULL)
		{
			printf ("\t%s <- %s\n"
					, (graph_get_vertex_i(e->dst_id, graph_p))->name
					, (graph_get_vertex_i(e->src_id, graph_p))->name);
			e = (edge_t*)linked_list_iterator_next(iter);
		}
		printf("   dst\n");
		
		iter = linked_list_iterator_init(v->dst, iter);
		e = (edge_t*) linked_list_iterator_curr(iter);
		while (e != NULL)
		{
			printf ("\t%s -> %s\n"
					, (graph_get_vertex_i(e->src_id, graph_p))->name
					, (graph_get_vertex_i(e->dst_id, graph_p))->name);
			e = (edge_t*)linked_list_iterator_next(iter);
		}
		printf("   nd\n");
		
		iter = linked_list_iterator_init(v->nd, iter);
		e = (edge_t*) linked_list_iterator_curr(iter);
		while (e != NULL)
		{
			printf ("\t%s -- %s\n"
					, (graph_get_vertex_i(e->src_id, graph_p))->name
					, (graph_get_vertex_i(e->dst_id, graph_p))->name);
			e = (edge_t*)linked_list_iterator_next(iter);
		}
		printf("\n");
		printf("\n");
	}
	
	if(graph_p->num_vertices)
		linked_list_iterator_free(iter);
}



int graph_print_dot(char* file_name, graph_t* graph_p)
{
	FILE *f;
	int i;
	vertex_t *v;
	edge_t* e;
	linked_list_iterator_t *iter;
	
	if(file_name == NULL)
		f = stdout;
	else
		f = fopen(file_name, "w");
	
	if(graph_p->num_vertices)
		iter = (linked_list_iterator_t *)malloc(sizeof(linked_list_iterator_t));
	
	fprintf(f,"digraph {\n");

	for (i = 0; i < graph_p->num_vertices; i++)
	{
		v = graph_get_vertex_i(i, graph_p);
		if(v == NULL)
			continue;
		
		if(!linked_list_size(v->src) && !linked_list_size(v->dst) && !linked_list_size(v->nd))
			fprintf(f,"\t%s;\n", v->name);
		
		iter = linked_list_iterator_init(v->dst, iter);
		e = (edge_t*) linked_list_iterator_curr(iter);
		while (e != NULL)
		{
			fprintf(f,"\t%s -> %s [label=\"%.2f\"];\n"
					, (graph_get_vertex_i(e->src_id, graph_p))->name
					, (graph_get_vertex_i(e->dst_id, graph_p))->name
					, e->weight);
			e = (edge_t*)linked_list_iterator_next(iter);
		}
		
		iter = linked_list_iterator_init(v->nd, iter);
		e = (edge_t*) linked_list_iterator_curr(iter);
		while (e != NULL)
		{
			if( e->src_id == v->id)
			{
				fprintf(f,"\t%s -> %s [dir=none,label=\"%.2f\"];\n"
					, (graph_get_vertex_i(e->src_id, graph_p))->name
					, (graph_get_vertex_i(e->dst_id, graph_p))->name
					, e->weight);
			}
			e = (edge_t*)linked_list_iterator_next(iter);
		}
	}
		fprintf(f,"}\n");
	if(file_name != NULL)
		fclose(f);
	if(graph_p->num_vertices)
		linked_list_iterator_free(iter);
}

int graph_get_order (graph_t* graph_p)
{
	return graph_p->num_vertices - linked_list_size(graph_p->removed_vertices);
}
int graph_get_size (graph_t* graph_p)
{
	return graph_p->num_edges;
}
