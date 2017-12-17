/* forward star type graph structure -- for quite large graphs */
/* 6/Nov/2008   Takeaki Uno  */


#ifndef _fstar_h_
#define _fstar_h_

#include"stdlib2.h"

#define FSTAR_WRITE_EDGE_FILE 16777216   // write file as edge list
#define FSTAR_REDUCE 33554432    // output table
#define FSTAR_CNT_IN 67108864
#define FSTAR_CNT_OUT 134217728
#define FSTAR_CNT_DEG_ONLY 268435456   // in_deg means usual degree

#define FSTAR_DEG_CHK 536870912
#define FSTAR_IN_CHK 1073741824
#define FSTAR_OUT_CHK 8388608
#define FSTAR_INS_ROWID 536870912  // insert row ID to the head of each line
#define FSTAR_INS_ROWID_WEIGHT 1073741824  // insert row ID to the head of each line

// undirected graph => CNT_DEG_ONLY, edge_dir = 0
// bipartite graph => BIPARTITE, edge_dir != 0
#ifndef FSTAR_INT
 #ifdef FSTAR_INT_LONG
  #define FSTAR_INT LONG
  #define FSTAR_INTHUGE LONGHUGE
  #define FSTAR_INTF "%llu"
 #else
  #define FSTAR_INT unsigned int
  #define FSTAR_INTHUGE UINTHUGE
  #define FSTAR_INTF "%u"
 #endif
#endif
#define SWAP_FSTAR_INT(a,b)  (common_FSTAR_INT=a,a=b,b=common_FSTAR_INT)

typedef struct {
  unsigned char type;  // type definition
  char *fname, *wfname;      // input file name, edge weight file name
  int flag;         // flag
  FSTAR_INT *edge;   // edge array
  FSTAR_INT *fstar;  // starting position of edge list for each vertex
  FSTAR_INT *in_deg, *out_deg;    // in/out-degree of each vertex (can be NULL if not used)
  FSTAR_INT node_num, out_node_num, in_node_num;  // #vertex and #1st/2nd vertices
  FSTAR_INT edge_num, edge_num_org, reduced_node_num;  // #edges in file, in array
  FSTAR_INT xmax, ymax;  // maximum in 1st/2nd column
  FSTAR_INT *table, *rev_table;   // vertex permutation table and its reverse
  WEIGHT *edge_w;   // edge weights

  int edge_dir;

  FSTAR_INT deg_lb, in_lb, out_lb, deg_ub, in_ub, out_ub;  // bounds for degrees
  WEIGHT w_lb, w_ub; // bounds for edge weight
} FSTAR;

QSORT_TYPE_HEADER (FSTAR_INT,FSTAR_INT)
extern FSTAR INIT_FSTAR;

/**************************************************************/ 
void FSTAR_print (FILE *fp, FSTAR *F);
void FSTAR_init2 (FSTAR *F);
void FSTAR_end (FSTAR *F);
void FSTAR_load (FSTAR *F);

LONG FSTAR_alloc_deg (FSTAR *F);
void FSTAR_calc_fstar (FSTAR *F);
int FSTAR_eval_edge (FSTAR *F, FSTAR_INT x, FSTAR_INT y, WEIGHT w);
void FSTAR_extract_subgraph (FSTAR *F);
void FSTAR_sort_adjacent_node (FSTAR *F, int flag);

void FSTAR_scan_file (FSTAR *F, FILE2 *fp);
void FSTAR_read_file_edge (FSTAR *F, FILE2 *fp);
void FSTAR_read_file_node (FSTAR *F, FILE2 *fp, FILE2 *fp2);

void FSTAR_write_table_file (FSTAR *F, char *fname);
LONG FSTAR_write_graph (FSTAR *F, char *fname, char *fname2);
LONG FSTAR_write_graph_operation (FSTAR *F1, FSTAR *F2, char *fname, char *fname2, int op, double th);

#endif

