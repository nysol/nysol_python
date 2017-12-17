/* SACHICA: Scalable Algorithm for Characteristic and Homologinous Interval CAlclation */
/* 2004 Takeaki Uno */
/* matdraw */

#ifndef _grhfil_c_
#define _grhfil_c_

// #define FSTAR_INT unsigned int
// internal_params.l1 :  #edges written to the output file

#define WEIGHT_DOUBLE

#include"fstar.c"
#include"problem.c"

#define GRHFIL_INS_ROWID 1024

/* error routine */
void GRHFIL_error (){
  ERROR_MES = "command explanation";
  print_err ("graph filtering: transform/convert/extract graph/subgraph\n\
grhfil dDUBeEq [options] input-file output-file\n\
%%:show progress, _:no message, +:write solutions in append mode\n\
d:directed graph (x->y), D:directed graph with reverse direction (x<-y)\n\
U,u:undirected graph (u:edge for both direction), B:bipartite graph, e,E:read/write file as edge list\n\
s,S:sort vertex adjacent list in increasing/decreasing order\n\
n,N:read/write the number of vertices and edges written in/at 1st line of the file\n\
v,V:node ID in read/write file starts from 1, q:non-transform mode (valid with -P option)\n\
0:insert vertex ID as the first entry, for each vertex (adjacency list mode)\n\
9:give weight 1 to each vertex ID (with 0)\n\
w,W:read/write edge weights in the graph file\n\
1:unify consecutive two same numbers into one\n\
[options]\n\
-t,T [num]: remove vertices with degree smaller/larger then [num]\n\
-i,I [num]: remove vertices with in-degree smaller/larger then [num]\n\
-o,O [num]: remove vertices with out-degree smaller/larger then [num]\n\
-r,R [num]: remove edges with weights smaller/larger then [num]\n\
-n [num]: specify #nodes\n\
-X [num1] [num2]: multiply each weight by [num1] and trancate by [num2]\n\
-w,W [filename]: weight file to be read/write\n\
-d [filename]: take difference with graph of [filename] (2nd -d: specify the threshold value)\n\
-m,M [filename]: take intersection/union with graph of [filename]\n\
-p [filename]: permute the vertex ID to coutinuous numbering and output the permutation table to the file\n\
-Q [filename]: permute the numbers in the file according to the table \n");
  EXIT;
}

int GRHFIL_read_param_iter (char *a, int *ff){
  int f=0;
  *ff = 2;
  if ( strchr(a, 'u') ){ f |= LOAD_EDGE; *ff = 0; }
  if ( strchr(a, 'U') ) *ff = 0;
  if ( strchr(a, 'B') ){ f |= LOAD_BIPARTITE; *ff = 1; }
  if ( strchr(a, 'D') ){ *ff = -1; f |= LOAD_TPOSE; }
  if ( strchr(a, 'd') ) *ff = 1;
  if ( strchr(a, 'e') ) f |= LOAD_ELE;
  if ( strchr(a, 's') ) f |= LOAD_INCSORT;
  if ( strchr(a, 'S') ) f |= LOAD_DECSORT;
  if ( strchr(a, 'n') ) f |= LOAD_GRAPHNUM;
  if ( strchr(a, 'w') ) f |= LOAD_EDGEW;
  if ( strchr(a, 'v') ) f |= LOAD_ID1;
  return (f);
}

/* read filenames from command line parameters */
void GRHFIL_read_param (PROBLEM *PP, int argc, char *argv[]){
  int c=1;
  FSTAR *FS = &PP->FS;
  if ( argc<c+3 ){ GRHFIL_error (); return; }
  FS->edge_dir = 2;
  
  if ( !strchr (argv[c], '_') ){ FS->flag |= SHOW_MESSAGE; }
  if ( strchr (argv[c], '+') ) PP->II.flag |= ITEMSET_APPEND;

  FS->flag |= GRHFIL_read_param_iter (argv[c], &FS->edge_dir);

  if ( strchr(argv[c], 'u') ) PP->problem |= LOAD_EDGE;

  if ( strchr(argv[c], 'E') ) PP->problem |= LOAD_ELE;
  if ( strchr(argv[c], 'N') ) PP->problem |= LOAD_GRAPHNUM;
  if ( strchr(argv[c], 'W') ) PP->problem |= LOAD_EDGEW;
  if ( strchr(argv[c], 'V') ) PP->problem |= LOAD_ID1;
  if ( strchr(argv[c], 'q') ){ PP->dir = 1; FS->edge_dir = 0; }
  if ( strchr(argv[c], '0') ){ PP->problem |= FSTAR_INS_ROWID; }
  if ( strchr(argv[c], '9') ){ PP->problem |= FSTAR_INS_ROWID_WEIGHT; }
  if ( strchr(argv[c], '1') ){ PP->problem |= LOAD_RM_DUP; }
  if ( FS->edge_dir == 2 ) error ("one of B, D, d, u or U has to be given", EXIT);

  c++;
  while ( argv[c][0] == '-' ){
    switch (argv[c][1]){
      case 't': FS->deg_lb = atoi(argv[c+1]);
      break; case 'T': FS->deg_ub = atoi(argv[c+1]);
      break; case 'i': FS->in_lb = atoi(argv[c+1]);
      break; case 'I': FS->in_ub = atoi(argv[c+1]);
      break; case 'o': FS->out_lb = atoi(argv[c+1]);
      break; case 'O': FS->out_ub = atoi(argv[c+1]);
      break; case 'r': FS->w_lb = atof(argv[c+1]);
      break; case 'R': FS->w_ub = atof(argv[c+1]);
      break; case 'X': PP->ratio = atof(argv[c+1]); PP->th = atof(argv[c+2]); c++;
      break; case 'w': FS->wfname = argv[c+1];
      break; case 'W': PP->weight_fname = argv[c+1];
      break; case 'p': PP->table_fname = argv[c+1];
      break; case 'Q': PP->table_fname = argv[c+1]; PP->dir =1;
      break; case 'd': if ( PP->FS2.fname ){
          PP->th2 = atof(argv[c+1]);
          PP->root = 4;
      } else {
          PP->FS2.fname = argv[c+1];
          PP->root = 3; PP->th2 = 1;
          PP->FS2.flag |= GRHFIL_read_param_iter (&argv[c][2], &PP->FS2.edge_dir);
          printf ("%d \n", GRHFIL_read_param_iter (&argv[c][2], &PP->FS2.edge_dir));
          if ( PP->FS2.edge_dir == 2 ) PP->FS2.edge_dir = FS->edge_dir; }
      break; case 'm': PP->FS2.fname = argv[c+1]; PP->root = 1;
          PP->FS2.flag |= GRHFIL_read_param_iter (&argv[c][2], &PP->FS2.edge_dir);
      break; case 'M': PP->FS2.fname = argv[c+1]; PP->root = 2;
          PP->FS2.flag |= GRHFIL_read_param_iter (&argv[c][2], &PP->FS2.edge_dir);
      break; case 'n': PP->rows = atoi(argv[c+1]); 
      break; case ',': PP->II.separator = argv[c+1][0];
      break; default: goto NEXT;
    }
    c += 2;
    if ( argc<c+2 ){ GRHFIL_error (); return; }
  }
  
  NEXT:;
  PP->FS.fname = argv[c];
  PP->output_fname = argv[c+1];
  print_mes (FS, "input-file %s, output-file %s\n", PP->FS.fname, PP->output_fname);
  print_mes (FS, "degree threshold: ");
  if ( FS->deg_lb>0 ) print_mes (FS, FSTAR_INTF" <", FS->deg_lb);
  if ( FS->deg_lb>0 || FS->deg_ub<FSTAR_INTHUGE) print_mes (FS, " degree ");
  if ( FS->deg_ub<FSTAR_INTHUGE ) print_mes (FS, "< "FSTAR_INTF"  ", FS->deg_ub);

  if ( FS->in_lb>0 ) print_mes (FS, FSTAR_INTF" <", FS->in_lb);
  if ( FS->in_lb>0 || FS->in_ub<FSTAR_INTHUGE) print_mes (FS, " in-degree ");
  if ( FS->in_ub<FSTAR_INTHUGE ) print_mes (FS, "< "FSTAR_INTF"  ", FS->in_ub);
  
  if ( FS->out_lb>0 ) print_mes (FS, FSTAR_INTF" <", FS->out_lb);
  if ( FS->out_lb>0 || FS->out_ub<FSTAR_INTHUGE) print_mes (FS, " out-degree ");
  if ( FS->out_ub<FSTAR_INTHUGE ) print_mes (FS, "< "FSTAR_INTF"  ", FS->out_ub);
  print_mes (FS, "\n");
  
  if ( PP->table_fname ) print_mes (FS, "permutation-table-file %s\n", PP->table_fname);

}


/* main routine */
int GRHFIL_main (int argc, char *argv[]){
  PROBLEM PP;
  FSTAR *FS = &PP.FS;
  FILE2 fp, fp2;
  LONG l, x;
  char i;
  WEIGHT w;
	ERROR_MES = NULL;
  PROBLEM_init (&PP);
  GRHFIL_read_param (&PP, argc, argv);
FS->out_node_num = PP.rows;
  if ( PP.dir ) ARY_LOAD (PP.FS.table, int, l, PP.table_fname, 1, EXIT);

    // no transformation (just replace the numbers and separators)
  if ( PP.dir ){
    FILE2_open (fp, FS->fname, "r", EXIT);
    FILE2_open (fp2, PP.output_fname, "w", EXIT);
    do {
      i=0; x=0;
      do {
        l = FILE2_read_int (&fp);
        if ( (FILE_err&4)==0 ){
          FILE2_print_int (&fp2, FS->table? FS->table[l]: l, i);
          i = PP.II.separator;
          if ( (FS->flag&LOAD_EDGEW) && (((FS->flag&LOAD_ELE)&&x==1) || !(FS->flag&LOAD_ELE)) ){
            w = FILE2_read_double (&fp);
            FILE2_print_int (&fp2, w, i);
          }
          FILE2_flush (&fp2);
        }
        x++;
      } while ( (FILE_err&3)==0 );
      FILE2_puts (&fp2, "\n");
    } while ( (FILE_err&2)==0 );
    FILE2_close (&fp);
    FILE2_closew (&fp2);
    return (0);
  }

  PROBLEM_load (&PP);
  if ( PP.ratio != 0 || PP.th != 0 ){ // multiply & trancate
    FLOOP (l, 0, FS->edge_num+1){ FS->edge_w[l] *= PP.ratio; ENMIN (FS->edge_w[l], PP.th); }
  }
  FS->flag = PP.problem; // +(FS->flag&LOAD_EDGE);
  if ( !PP.dir ) FSTAR_write_table_file (FS, PP.table_fname);
  if ( PP.root ) internal_params.l1 = FSTAR_write_graph_operation (FS, &PP.FS2, PP.output_fname, PP.weight_fname, PP.root, PP.th2);
  else internal_params.l1 = FSTAR_write_graph (FS, PP.output_fname, PP.weight_fname);

  PROBLEM_end (&PP);
  return (0);
}

/*******************************************************************************/
#ifndef _NO_MAIN_
#define _NO_MAIN_
int main (int argc, char *argv[]){
  return (GRHFIL_main (argc, argv));
}
#endif
/*******************************************************************************/

#endif



