
#include <stdio.h>
#include <string.h>
#include <check.h>

#include "../graph.h"
graph_t *g;
Suite *create_test_suite(void);

//**************************
//     Checked fixtures    *
//**************************


START_TEST(test_empty_graph) {
	
	g = graph_new(GRAPH_DIRECTED | GRAPH_CYCLIC, 20, COLLECTION_MODE_ASYNCHRONIZED);
	
	fail_if( graph_remove_vertex_i(0, NULL, NULL, g) != -1, "Removing inexistent vertex: return should be -1");
	fail_if( graph_remove_edge_i(0, 1, GRAPH_DIRECTED, NULL, g) != -1, "Removing inexistent vertex: return should be -1");
	fail_if( graph_remove_edge_i(0, 0, GRAPH_DIRECTED, NULL, g) != -1, "Removing inexistent vertex: return should be -1");
	
	graph_free(NULL, NULL, g);
}
END_TEST


START_TEST(test_small_graph) {
	
	g = graph_new(GRAPH_DIRECTED | GRAPH_ACYCLIC |GRAPH_STRICT, 20, COLLECTION_MODE_ASYNCHRONIZED);
	int ret = graph_add_vertex("A", NULL, g);
	
	fail_if( ret < 0, "add_vertex: return should be the new id but id=%d", ret);
	ret = graph_add_vertex("B", NULL, g);
	fail_if( ret < 0, "add_vertex: return should be the new id but id=%d", ret);
	ret = graph_add_vertex("C", NULL, g);
	fail_if( ret < 0, "add_vertex: return should be the new id but id=%d", ret);
	
	
	ret = graph_add_edge_sw("A", "B", NULL, GRAPH_DIRECTED, 2, g);
	fail_if( ret != 0, "add_vertex: return should be 0 but ret=%d", ret);
	
	ret = graph_add_edge_sw("D", "B", NULL, GRAPH_DIRECTED, 2, g);
	fail_if( ret != -1, "add_vertex: non existent vertex: return should be -1 but ret=%d", ret);
	
	ret = graph_add_edge_sw("A", "C", NULL, 9, 2, g);
	fail_if( ret != -2, "add_vertex: non supported edge type: return should be -2 but ret=%d", ret);
	
	ret = graph_add_edge_sw("C", "A", NULL, GRAPH_NON_DIRECTED, 2, g);
	fail_if( ret != -3, "add_vertex: non compatible direction type: return should be -3 but ret=%d", ret);
	
	ret = graph_add_edge_sw("B", "B", NULL, GRAPH_DIRECTED, 2, g);
	fail_if( ret != -4, "add_vertex: breaking acyclity: return should be -4 but ret=%d", ret);
	
	ret = graph_add_edge_sw("A", "B", NULL, GRAPH_DIRECTED, 2, g);
	fail_if( ret != -5, "add_vertex: breaking multiplicity: return should be -5 but ret=%d", ret);
	
	ret = graph_add_edge_sw("C", "B", NULL, GRAPH_DIRECTED, 2, g);
	fail_if( ret != 0, "add_vertex: return should be 0 but ret=%d", ret);
	
	//ret = graph_add_vertex(NULL, NULL, g);
	//fail_if( ret < 10, "add_vertex: return should be the new id but id=%d", ret);
	
	graph_print(g);
	
	graph_print_dot("check_graph.gv", g);
	
	graph_free(NULL, NULL, g);
}
END_TEST


START_TEST(test_big_graph) {
	
}
END_TEST


/* ******************************
 *      Main entry point        *
 * ******************************/

int main (int argc, char *argv) {
    Suite *fs = create_test_suite();
    SRunner *fs_runner = srunner_create(fs);
    srunner_run_all(fs_runner, CK_NORMAL);
    int number_failed = srunner_ntests_failed (fs_runner);
    srunner_free (fs_runner);
    return (number_failed == 0) ? EXIT_SUCCESS : EXIT_FAILURE;
}


Suite *create_test_suite(void) {
	TCase *tc_create_free = tcase_create("Create and free");
	
    //tcase_add_checked_fixture(tc_create_free, create_graph, free_graph);
    tcase_add_test(tc_create_free, test_empty_graph);
    tcase_add_test(tc_create_free, test_small_graph);
    tcase_add_test(tc_create_free, test_big_graph);
    //tcase_add_test(tc_create_free, test_create_free);
    
    //TCase *tc_iterators = tcase_create("Iterators");
    //tcase_add_checked_fixture(tc_iterators, create_graph, free_graph);
    //tcase_add_test(tc_iterators, test_iterators);
    
    // Add test cases to a test suite
    Suite *fs = suite_create("Graph");
    suite_add_tcase(fs, tc_create_free);
    //suite_add_tcase(fs, tc_iterators);
    
    return fs;
}





/*



#include <stdio.h>
#include <string.h>

#include "../graph/graph.h"
graph_t *g;
void foo()
{


}

void free_cond(void* c)
{
	if(c)
		free(c);
}

void* new_mem(const char* c)
{
	return NULL;
	char* v = malloc(20);
	int i;
	for(i = 0; i < 20 && c[i] != 0; i++)
		v[i]=c[i];
	v[i] = 0;
	return v;
}

int main()
{
	printf("Holamundo\n");
	
	g = graph_new(GRAPH_MIXED_DIRECTED | GRAPH_ACYCLIC, 20, COLLECTION_MODE_ASYNCHRONIZED);
	
	
	graph_add_vertex("A", NULL, g);
	graph_add_vertex("B", NULL, g);
	graph_add_vertex("C", NULL, g);
	graph_add_vertex("D", malloc(29), g);
	
	graph_add_edge_i(0,1, new_mem("Hola"), GRAPH_DIRECTED,2, g);
	graph_add_edge_i(1,2, new_mem("Hola"), GRAPH_DIRECTED,2, g);
	void* v = malloc(23);
	if(graph_add_edge_i(2,0, v, GRAPH_DIRECTED,2, g)<0)
		free(v);
	
	
	graph_print(g);
	printf("Clear\n");
	graph_clear(free_cond,free_cond,g);
	
	graph_add_vertex("A", NULL, g);
	graph_add_vertex("S", NULL, g);
	graph_add_vertex("D", NULL, g);
	graph_add_vertex("F", NULL, g);
	graph_add_vertex("G", NULL, g);

	graph_add_edge_s("A","S",NULL,GRAPH_DIRECTED,2,g);
	graph_add_edge_s("S","D",NULL,GRAPH_DIRECTED,2,g);
	graph_add_edge_s("S","D",NULL,GRAPH_DIRECTED,2,g);
	graph_add_edge_s("G","D",NULL,GRAPH_DIRECTED,2,g);
	graph_add_edge_s("A","D",NULL,GRAPH_NON_DIRECTED,2,g);
	graph_add_edge_s("D","D",NULL,GRAPH_DIRECTED,2,g);
	graph_add_edge_s("A","F",NULL,GRAPH_NON_DIRECTED,2,g);
	
	
	graph_add_edge_s("F","G",new_mem("Pepe"),GRAPH_DIRECTED,2,g);
	graph_add_edge_s("F","G",new_mem("Pepon"),GRAPH_DIRECTED,2,g);
	graph_add_edge_s("F","G",new_mem("Null"),GRAPH_DIRECTED,2,g);
	               
	
	graph_add_edge_s("G","S",NULL,GRAPH_NON_DIRECTED,9,g);
	graph_add_edge_s("F","S",NULL,GRAPH_DIRECTED,2,g);

	edge_t * e = graph_get_edge_i(3,4,GRAPH_DIRECTED,g);
	if(e != NULL)
		printf("Edge Tomado %s\n",(char*)e->data);
	graph_remove_edge_e(e,GRAPH_DIRECTED,free_cond,g);
	graph_remove_edge_s("A","S",GRAPH_DIRECTED,free_cond,g);
	graph_remove_edge_s("A","Sasdfasd",GRAPH_DIRECTED,free_cond,g);
	graph_remove_edge_s("A","G",GRAPH_DIRECTED,free_cond,g);
	//printf("Edge Eliminado\n");
	
	
	//graph_print(g);
	//printf("Go go to Eliminar vertice\n");
	//graph_remove_vertex_s("A",NULL,free_cond,g);
	
	//graph_add_vertex("GAUSON", NULL, g);
/*	linked_list_iterator_t *iter = linked_list_iterator_new(l);
	char* c = (char*)linked_list_iterator_curr(iter);
	while(c != NULL)
	{
		printf("CHAR DEL LINKED : %s\n",c);
		c = (char*)linked_list_iterator_next(iter);
		
	}
	linked_list_iterator_free(iter);
	linked_list_free(l, NULL);
	*/
	/*
	graph_add_edge_s("GAUSON", "F", NULL, GRAPH_NON_DIRECTED,3.1415, g);
	graph_add_edge_s("GAUSON", "F", NULL, GRAPH_DIRECTED,2, g);
	graph_add_edge_s("F", "GAUSON", NULL, GRAPH_DIRECTED,1.618, g);
	
	
	//graph_remove_vertex_s("dsfg",NULL,free_cond,g);
	//graph_remove_vertex_s("S",NULL,free_cond,g);
	
	graph_print(g);
	
	graph_print_dot("grafo.gv",g);
	
	//printf("Existe camino entre 0 y 3? %d\n", graph_path_exists(0,3,g));
	
	graph_free(NULL,free_cond,g);
	
	return 0;
}

*/
