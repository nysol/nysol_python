/* forward star type graph structure -- for quite large graphs */
/* 6/Nov/2008   Takeaki Uno  */

#ifndef _fstar_c_
#define _fstar_c_

#include"fstar.h"
#include"stdlib2.c"

FSTAR INIT_FSTAR = {TYPE_FSTAR,NULL,NULL,0,NULL,NULL,NULL,NULL,0,0,0,0,0,0,0,0,NULL,NULL,NULL,0, 0,0,0,FSTAR_INTHUGE,FSTAR_INTHUGE,FSTAR_INTHUGE,-WEIGHTHUGE,WEIGHTHUGE};

QSORT_TYPE (FSTAR_INT , FSTAR_INT)

void FSTAR_print (FILE *fp, FSTAR *F){
  FSTAR_INT i, x=0, y, flag;

  FLOOP (i, 0, F->edge_num+1){
    while ( i == F->fstar[x+1]){
      flag = 0;
      if ( F->out_deg && (F->out_node_num ==0 || x < F->out_node_num) ) flag += F->out_deg[x];
      if ( F->in_deg && (F->in_node_num ==0 || x < F->in_node_num) ) flag += F->in_deg[x];
      if ( (!F->out_deg && !F->in_deg) && (F->flag&LOAD_EDGE) ) flag = F->fstar[x+1] - F->fstar[x];
      if ( !F->table || flag ) fprintf (fp, "\n");
      if ( ++x == F->out_node_num ) goto END;
    }
    y = F->edge[i];
    if ( !(F->flag&(LOAD_BIPARTITE+LOAD_EDGE)) && F->edge_dir==0 && x>y ) continue;
    if ( F->table && !(F->flag&LOAD_BIPARTITE) ){
      if ( (y=F->table[y]) == F->out_node_num ) continue;
    }
    if ( F->flag & LOAD_ID1 ) y++;
    fprintf (fp, FSTAR_INTF" ", y);
    if ( F->edge_w ) fprintf (fp, WEIGHTF " ", F->edge_w[i]);
  }
  fprintf (fp, "\n");
  END:;
}

/* initialization */
void FSTAR_init2 (FSTAR *F){
  if ( F->in_lb >0 || F->in_ub <FSTAR_INTHUGE ) F->flag |= (FSTAR_IN_CHK+FSTAR_CNT_IN);
  if ( F->out_lb >0 || F->out_ub <FSTAR_INTHUGE ) F->flag |= (FSTAR_OUT_CHK+FSTAR_CNT_OUT);
  if ( F->deg_lb >0 || F->deg_ub <FSTAR_INTHUGE ){
    F->flag |= FSTAR_DEG_CHK+FSTAR_CNT_IN;
    if ( (F->flag&(FSTAR_OUT_CHK+FSTAR_IN_CHK)) ==0 ) F->flag |= FSTAR_CNT_DEG_ONLY;
    else F->flag |= FSTAR_CNT_OUT;
  }
}

/* termination */
void FSTAR_end (FSTAR *F){
  mfree (F->edge, F->edge_w, F->in_deg, F->out_deg, F->fstar, F->table, F->rev_table);
  *F = INIT_FSTAR;
}

/* increment degrees */
void FSTAR_inc_deg (FSTAR *F, FSTAR_INT x, FSTAR_INT y){
  if ( F->out_deg ) F->out_deg[x]++;
  if ( F->out_deg && (F->flag&LOAD_EDGE) ) F->out_deg[y]++;
  if ( F->in_deg ) F->in_deg[y]++;
  if ( ((F->flag & FSTAR_CNT_DEG_ONLY) || F->edge_dir==0) && F->in_deg ) F->in_deg[x]++;
}

/* remove edges adjacent to small/large degree vertices */
void FSTAR_sort_adjacent_node (FSTAR *F, int flag){
  FSTAR_INT x, d=0, y, s = sizeof(FSTAR_INT)+sizeof(WEIGHT);
  char *p;
  if ( F->edge_w ){
       // sort with weight; make array of (ID,weight) and sort it
    FLOOP (x, 0, F->out_node_num) ENMAX (d, F->fstar[x+1]-F->fstar[x]);
    malloc2 (p, d * (sizeof(FSTAR_INT)+sizeof(WEIGHT)), EXIT);
    FLOOP (x, 0, F->out_node_num){
      FLOOP (y, 0, F->fstar[x+1]-F->fstar[x]){
        *((FSTAR_INT *)(&p[y*s])) = F->edge[y+F->fstar[x]];
        *((WEIGHT *)(&p[y*s+sizeof(FSTAR_INT)])) = F->edge_w[y+F->fstar[x]];
      }
      qsort_FSTAR_INT ((void *)p, F->fstar[x+1]-F->fstar[x], flag*s);
      FLOOP (y, 0, F->fstar[x+1]-F->fstar[x]){
        F->edge[y+F->fstar[x]] = *((FSTAR_INT *)(&p[y*s]));
        F->edge_w[y+F->fstar[x]] = *((WEIGHT *)(&p[y*s+sizeof(FSTAR_INT)]));
      }
    }
    free2 (p);
  } else {
    FLOOP (x, 0, F->out_node_num)
        qsort_FSTAR_INT (&F->edge[F->fstar[x]], F->fstar[x+1]-F->fstar[x], flag);
  }
}

/* compute node_num's and allocate arrays for degree */
LONG FSTAR_alloc_deg (FSTAR *F){
//  int tpose = (F->edge_dir==-1 && !(F->flag & LOAD_TPOSE)) || (F->edge_dir!=-1 && (F->flag & LOAD_TPOSE));
  LONG i = F->out_node_num, j;
  F->out_node_num = F->xmax;
  F->in_node_num = F->ymax;
//  if ( F->edge_dir == -1 ) SWAP_FSTAR_INT ( F->out_node_num, F->in_node_num);
  j = F->out_node_num; ENMAX (F->out_node_num, i);
  F->node_num = MAX (F->out_node_num, F->in_node_num);
  if ( F->edge_dir == 0 ) F->in_node_num = F->out_node_num = F->node_num;
  calloc2 (F->fstar, F->out_node_num+2, EXIT);
  if ( F->flag & LOAD_EDGE ) return (j);

  if ( F->flag & FSTAR_CNT_IN ){
    if ( F->flag & FSTAR_CNT_DEG_ONLY ){
        calloc2 (F->in_deg, F->node_num+2, EXIT);
    } else calloc2 (F->in_deg, F->in_node_num+2, EXIT);
  }
  if ( F->flag & FSTAR_CNT_OUT )
      calloc2 (F->out_deg, F->out_node_num+2, EXIT);
  return (j);
}

/* compute fstar from outdegree stored in fstar itself, and allocate edge array */
void FSTAR_calc_fstar (FSTAR *F){
  FSTAR_INT i, j=0, jj;
  FLOOP (i, 0, F->out_node_num){
    jj = j + F->fstar[i];
    F->fstar[i] = j;
    j = jj;
  }
  F->fstar[i] = F->edge_num = j;
//  malloc2 (F->edge, ((F->flag&LOAD_EDGE)?2:1) *F->edge_num +2, EXIT);
//  ARY_FILL (F->edge, 0, ((F->flag&LOAD_EDGE)?2:1) *F->edge_num +2, F->node_num+1);
  malloc2 (F->edge, F->edge_num +2, EXIT);
  ARY_FILL (F->edge, 0, F->edge_num +2, F->node_num+1);
  if ( (F->flag&LOAD_EDGEW) || F->wfname ) malloc2 (F->edge_w, F->edge_num +2, EXIT); // weight array
}

/* return 1 if edge (x,y) is valid */
/* head != 0 means x is the origin, otherwise x is destination */
int FSTAR_eval_edge (FSTAR *F, FSTAR_INT x, FSTAR_INT y, WEIGHT w){

  if ( x>F->node_num || y>F->node_num || x<0 || y<0 ) return (0);
  if ( ((F->flag&LOAD_EDGEW) || F->wfname) && !RANGE (F->w_lb, w, F->w_ub) ) return (0);
  if ( F->flag & LOAD_EDGE ){
    if ( F->fstar[F->out_node_num] ){
      if ( !RANGE (F->deg_lb, F->fstar[x+1]-F->fstar[x], F->deg_ub) ) return (0);
      if ( !RANGE (F->deg_lb, F->fstar[y+1]-F->fstar[y], F->deg_ub) ) return (0);
    } else {
      if ( !RANGE (F->deg_lb, F->fstar[x], F->deg_ub) ) return (0);
      if ( !RANGE (F->deg_lb, F->fstar[y], F->deg_ub) ) return (0);
    }
    return (1);
  }

  if ( F->out_deg && (F->flag&FSTAR_OUT_CHK)){
    if ( !RANGE (F->out_lb, F->out_deg[x], F->out_ub) ) return (0);
    if ( (F->flag&LOAD_BIPARTITE)==0
          && !RANGE(F->out_lb, F->out_deg[y], F->out_ub) ) return (0);
  }
  if ( F->in_deg && F->out_deg && (F->flag&LOAD_BIPARTITE)==0 && (F->flag&FSTAR_DEG_CHK)){
    if ( !RANGE (F->deg_lb, F->in_deg[x]+F->out_deg[x], F->deg_ub) ) return (0);
    if ( !RANGE (F->deg_lb, F->in_deg[y]+F->out_deg[y], F->deg_ub) ) return (0);
  }
  
  if ( ((F->flag & FSTAR_CNT_DEG_ONLY) || F->edge_dir==0) && F->in_deg && (F->flag&FSTAR_DEG_CHK)){
    if ( !RANGE (F->deg_lb, F->in_deg[x], F->deg_ub) ) return (0);
    if ( !RANGE (F->deg_lb, F->in_deg[y], F->deg_ub) ) return (0);
  } else if ( F->in_deg && (F->flag&FSTAR_IN_CHK) && !(F->flag&FSTAR_CNT_DEG_ONLY) ){
    if ( (F->flag&LOAD_BIPARTITE)==0
         && !RANGE (F->in_lb, F->in_deg[x], F->in_ub) ) return (0);
    if ( !RANGE (F->in_lb, F->in_deg[y], F->in_ub) ) return (0);
  }
  return (1);
}

/* scan the file and count the degree, for edge listed file */
/* if F->out_node_num is set ot a number larger than #nodes, set the node number to it, so that isolated vertices will be attached to the last */
void FSTAR_scan_file (FSTAR *F, FILE2 *fp){
  LONG i, j;

//  LONG x, y, i;
//  double w;
  FILE_COUNT C = INIT_FILE_COUNT;
//  int tpose = F->flag & LOAD_TPOSE, f = F->flag - (F->flag & LOAD_EDGEW);

    // count #pairs
  C = FILE2_count (fp, (F->flag&(LOAD_ELE+LOAD_TPOSE+LOAD_NUM+LOAD_GRAPHNUM+LOAD_EDGE)) | FILE_COUNT_ROWT | (F->in_deg? FILE_COUNT_CLMT: 0), 0, 0, 0, (F->flag&LOAD_EDGEW)?1:0, 0);
  F->xmax = C.rows; F->ymax = C.clms; F->edge_num_org = C.eles;
  j = FSTAR_alloc_deg (F);
  FLOOP (i, 0, j){
    F->fstar[i] = C.rowt[i];
    if ( F->out_deg ) F->out_deg[i] = C.rowt[i];
    if ( F->in_deg ) F->in_deg[i] = C.clmt[i];
  }
  mfree (C.rowt, C.clmt);
}

/* load data from file to memory with allocation, (after the scan for counting) */
void FSTAR_read_file (FSTAR *F, FILE2 *fp, FILE2 *wfp){
  LONG i, x, y;
  int fc=0, FILE_err_=0;
  int flag = (F->flag & (FSTAR_DEG_CHK+FSTAR_IN_CHK+FSTAR_OUT_CHK)), phase;
  double w;

  if ( wfp ) FILE2_reset (wfp);

  for (phase=flag?1:2 ; phase < 3 ; phase++){
    i=0;
    FILE2_reset (fp);
    if ( F->flag&(LOAD_NUM+LOAD_GRAPHNUM) ) FILE2_read_until_newline (fp);
    if ( phase == 2 ) FSTAR_calc_fstar (F);
    do {
//////////////
      if ( F->flag&LOAD_ELE ){
        if ( FILE2_read_pair (fp, &x, &y, &w, F->flag) ) continue;
      } else {
        x = i;
        FILE_err_ = (FSTAR_INT)FILE2_read_item (fp, wfp, &x, &y, &w, fc, F->flag);
        if ( FILE_err&4 ) goto LOOP_END;
      }
/////////////

      if ( !flag || FSTAR_eval_edge (F, x, y, w) ){
        if ( phase == 2 ){
          if ( F->fstar[x+1]>F->fstar[x] && (!(F->flag & LOAD_EDGE) || F->fstar[y+1]>F->fstar[y])){
            if ( F->edge_w ) F->edge_w[F->fstar[x]] = w;
            F->edge[F->fstar[x]++] = y;
            if ( F->flag & LOAD_EDGE ){
              if ( F->edge_w ) F->edge_w[F->fstar[y]] = w;
              F->edge[F->fstar[y]++] = x;
            }
          }
        } else {
          F->fstar[x]--;
          if ( F->flag & LOAD_EDGE ) F->fstar[y]--;
        }
      }

//////////////
      if ( !(F->flag&LOAD_ELE) ){
        fc = 0;
        if ( FILE_err&3 ){
          LOOP_END:;
          i++;
          fc = FILE_err_? 0: 1; FILE_err_=0; // even if next weight is not written, it is the rest of the previous line
        }
      }
/////////////////////
    } while ( !(FILE_err&2) );
    phase++;
  }
  F->fstar[F->out_node_num+1] = 0;
  BLOOP (i, F->out_node_num, 0) F->fstar[i+1] = F->fstar[i];
  F->fstar[0] = 0;
}

/* remove edges adjacent to small/large degree vertices */
void FSTAR_extract_subgraph (FSTAR *F){
  FSTAR_INT x, y, ii, i, nodes;
  int flag = 0;
  WEIGHT w=0;

    // count #pairs
  do {
    nodes = 0;
    if ( F->in_deg || F->out_deg ){
        // re-count in/out degrees
      if ( F->in_deg ) ARY_FILL (F->in_deg, 0, F->in_node_num, 0);
      if ( F->out_deg ) ARY_FILL (F->out_deg, 0, F->out_node_num, 0);
      x=0; FLOOP (i, 0, F->edge_num){
        while ( i == F->fstar[x+1] ) x++;
        FSTAR_inc_deg (F, x, F->edge[i]);
      }
    }

      // re-remove out-bounded degree vertices
    ii=x=0; FLOOP (i, 0, F->edge_num){
      while ( i == F->fstar[x+1] ){ F->fstar[x+1] = ii; x++; flag = 0; }
      y = F->edge[i];
      if ( F->edge_w ) w = F->edge_w[i];
      if ( FSTAR_eval_edge (F, x, y, w) ){
        if ( F->edge_w ) F->edge_w[ii] = F->edge_w[i];
        F->edge[ii++] = y;
        if ( flag==0 ){ nodes++; flag = 1; }
      }
    }
    F->edge_num = ii;
    print_mes (F, "iterative scan: #nodes="FSTAR_INTF", #edges = %d\n", nodes, i);
  } while ( ii < i );
}

/* load graph from file */
void FSTAR_load (FSTAR *F){
  FILE2 fp = INIT_FILE2, wfp = INIT_FILE2;
  FSTAR_init2 (F);

  FILE2_open (fp, F->fname, "r", EXIT);
  if ( F->wfname ) FILE2_open (wfp, F->wfname, "r", EXIT);

  FSTAR_scan_file (F, &fp);
  print_mes (F, "first & second scan end: %s\n", F->fname);

  FSTAR_read_file (F, &fp, F->wfname? &wfp: NULL);
  FILE2_close (&fp);   if (ERROR_MES) EXIT;
  if ( F->wfname ) FILE2_close (&wfp);  if (ERROR_MES) EXIT;
  print_mes (F, "file read end: %s\n", F->fname);

  FSTAR_extract_subgraph (F);
//  if ( ((F->flag&LOAD_INCSORT) && !(F->flag&LOAD_ELE))
  if ( (F->flag&LOAD_INCSORT)
     || (F->flag&LOAD_DECSORT) ) FSTAR_sort_adjacent_node (F, (F->flag&LOAD_DECSORT)?-1:1);
  print_mes (F, "forwardstar graph: %s ,#nodes %d(%d,%d) ,#edges %d\n", F->fname, F->node_num, F->in_node_num, F->out_node_num, F->edge_num);
}

/* make vertex permutation table and write to table-file */
void FSTAR_write_table_file (FSTAR *F, char *fname){
  FSTAR_INT i, flag;
  FILE *fp;
  if ( !fname ) return;
  fopen2 (fp, fname, "w", EXIT);
  calloc2 (F->table, F->node_num, {fclose(fp);EXIT;});
  F->reduced_node_num = 0;
  FLOOP (i, 0, F->out_node_num){
    flag = 0;
    if ( F->out_deg ) flag += F->out_deg[i];
    if ( F->in_deg && (F->in_node_num==0 || i < F->in_node_num) ) flag += F->in_deg[i];
    if ( flag ){
      fprintf (fp, FSTAR_INTF"\n", (F->flag&LOAD_ID1)?i+1:i);
      F->table[i] = F->reduced_node_num;
      F->reduced_node_num++;
    } else F->table[i] = F->out_node_num;
  }
  fclose2 (fp);
}

FILE *FSTAR_open_write_file (FSTAR *F, char *fname){
  FILE *fp;
  fopen2 (fp, fname, "w", EXIT);
  if ( F->flag&(LOAD_NUM+LOAD_GRAPHNUM) ){
    fprintf (fp, FSTAR_INTF" ", F->node_num);
    if ( F->flag&LOAD_NUM ) fprintf (fp, FSTAR_INTF" ", F->node_num);
    fprintf (fp, FSTAR_INTF"\n", F->edge_num/((F->flag&LOAD_EDGE)?2:1));
  }
  return (fp);
}

/* write row ID on the top of the line */
void FSTAR_write_graph_ID (FSTAR *F, FILE *fp, FILE *fp2, FSTAR_INT ID){

      // for the case "first element is row ID
  if ( F->flag & FSTAR_INS_ROWID){
    if ( F->flag & LOAD_ELE ){
      fprintf (fp, FSTAR_INTF" "FSTAR_INTF, ID, ID);
      if ( (F->flag & LOAD_EDGE) && (F->flag & FSTAR_INS_ROWID_WEIGHT) ){ fprintf (fp, " 1"); }
      fprintf (fp, "\n");
    } else {
      fprintf (fp, FSTAR_INTF" ", ID);
      if ( fp2 && (F->flag & FSTAR_INS_ROWID_WEIGHT) ) fprintf (fp2, "1 ");
      if ( (F->flag & LOAD_EDGEW) && (F->flag & FSTAR_INS_ROWID_WEIGHT)){ fprintf (fp, "1 "); }
    }
  }
}

/* write an edge */
int FSTAR_write_graph_item (FSTAR *F, FSTAR_INT x, FSTAR_INT y, WEIGHT w, FILE *fp, FILE *fp2, int *row, FSTAR_INT *prv){

  if ( !(F->flag&(LOAD_BIPARTITE+LOAD_EDGE)) && F->edge_dir==0 && x>y ) return (1);

     // ID permutation 
  if ( F->table ){
    x = F->table[x];
    if ( !(F->flag & LOAD_BIPARTITE) ){
      if ( (y=F->table[y]) == F->out_node_num ) return (1);
    }
  }
  if ( F->flag & LOAD_ID1 ) y++;

       // write an element
  if ( !(F->flag&LOAD_RM_DUP) || *row == 0 || *prv != y ){
    if ( F->flag & LOAD_ELE ){
      fprintf (fp, FSTAR_INTF" "FSTAR_INTF, x, y);
      if (F->flag & LOAD_EDGEW){ fputc (' ', fp); fprint_real (fp, w); }
      fprintf (fp, "\n");
    } else {
      fprintf (fp, FSTAR_INTF" ", y);
      if ( fp2 ){ fprint_real (fp2, w); fputc (' ', fp2);  }
      if ( F->flag&LOAD_EDGEW ){ fprint_real (fp, w); fputc (' ', fp); }
    }
    *row = 1;
  }
  *prv = y;  // remember the previous element
  return (0);
}


/* write induced graph and the number convert table; needs accumerated node_deg, edge */
LONG FSTAR_write_graph (FSTAR *F, char *fname, char *fname2){
  FILE *fp2=NULL, *fp = FSTAR_open_write_file (F, fname);
  FSTAR_INT i=0, x=0, y, flag = 0, c=0, prv=0;
  int row=0, IDf=0;
  LONG cnt=0;
  if ( fname2 ) fopen2 (fp2, fname2, "w", EXIT);

  while (i <= F->edge_num+1){
    while ( i == F->fstar[x+1]){
      if ( !F->table || flag ){
        if ( IDf == 0 ) FSTAR_write_graph_ID (F, fp, fp2, c);
        flag = 0; c++; row=0; IDf = 0;
        if ( !(F->flag & LOAD_ELE) ){
          fprintf (fp, "\n");
          if ( fp2 ) fprintf (fp2, "\n");
        }
      }
      if ( ++x >= F->out_node_num ) goto END;
    }
    y = F->edge[i];

    if ( IDf == 0 ){
      if ( F->out_deg && (F->out_node_num ==0 || x < F->out_node_num) ) flag += F->out_deg[x];
      if ( F->in_deg && (F->in_node_num ==0 || x < F->in_node_num) ) flag += F->in_deg[x];
      if ( (!F->out_deg && !F->in_deg) && (F->flag&LOAD_EDGE) ) flag = F->fstar[x+1] - F->fstar[x];
      if ( !F->table || flag ) FSTAR_write_graph_ID (F, fp, fp2, c);
      IDf = 1;
    }

    FSTAR_write_graph_item (F, x, y, F->edge_w? F->edge_w[i]: 0, fp, fp2, &row, &prv);
    cnt++; i++;
  }
  if ( !(F->flag & LOAD_ELE) ){
    fprintf (fp, "\n");
    if ( fp2 ) fprintf (fp2, "\n");
  }
  END:;
  fclose (fp);
  fclose2 (fp2);
  return (cnt);
}

/* write induced graph and the number convert table; needs accumerated node_deg, edge */
LONG FSTAR_write_graph_operation (FSTAR *F1, FSTAR *F2, char *fname, char *fname2, int op, double th){
  FILE *fp2=NULL, *fp = FSTAR_open_write_file (F1, fname);
  FSTAR_INT i=0, j=0, x=0, y, y1, y2, prv=0, ee = MAX(F1->out_node_num, F1->in_node_num);
  int row2=0, end1=0, end2=0;
  LONG cnt=0;
  WEIGHT w=0, w1, w2;
  if ( fname2 ) fopen2 (fp2, fname2, "w", EXIT);
  ENMAX (ee, F2->out_node_num);
  ENMAX (ee, F2->in_node_num);

  FSTAR_write_graph_ID (F1, fp, fp2, 0);
  while (i <= F1->edge_num+1 && j <= F2->edge_num+1){
    while ( (end1 || i == F1->fstar[x+1]) && (end2 || j == F2->fstar[x+1])){
      row2 = 0;
      if ( !(F1->flag & LOAD_ELE) ){
        fprintf (fp, "\n");
        if ( fp2 ) fprintf (fp2, "\n");
      }
      x++;
      if ( x >= F1->out_node_num ) end1 = 1;
      if ( x >= F2->out_node_num ) end2 = 1;
      if ( end1 && end2 ) goto END;
      FSTAR_write_graph_ID (F1, fp, fp2, x);
    }

    y1 = (!end1 && i<F1->edge_num+1 && i < F1->fstar[x+1])? F1->edge[i]: ee;
    y2 = (!end2 && j<F2->edge_num+1 && j < F2->fstar[x+1])? F2->edge[j]: ee;
//printf ("come %d %d    %d %d %d\n", y2, (!end2 && j<F2->edge_num+1 && j < F2->fstar[x+1])? F2->edge[j]: ee, !end2, j<F2->edge_num+1, j < F2->fstar[x+1]);
    if ( y1 < y2 ){
      if ( op == 1 ){ i++; continue; } // intersection
      y = y1;
      w = F1->edge_w? F1->edge_w[i]: 0;
      i++;
    } else if ( y1 > y2){
      if ( op == 1 ){ j++; continue; } // intersection
      y = y2;
      w = F2->edge_w? F2->edge_w[j]: 0;
      j++;
    } else {
      if ( op == 3 ){ i++; j++; continue; } // symmetric difference
      y = y1;
      w1 = F1->edge_w? F1->edge_w[i]: 0;
      w2 = F2->edge_w? F2->edge_w[j]: 0;
      i++; j++;
      if ( op == 1 ) w = MIN(w1, w2); // weighted intersection
      if ( op == 2 ) w = MAX(w1, w2); // weighted union
      if ( op == 4 ) w = w1 - w2; // difference
    }
    if ( op == 4 && (w=abs(w)) < th ) continue;  // difference
    
//printf ("#### %d %d (%d, %d)\n", x, y, F1->out_node_num, F2->out_node_num);
//printf ("#### %d %d (%d, %d) (%d %d)\n", i, j, x, y, F1->edge_num, F2->edge_num);
    FSTAR_write_graph_item (F1, x, y, w, fp, fp2, &row2, &prv);
    cnt++;
  }
  if ( !(F1->flag & LOAD_ELE) ){
    fprintf (fp, "\n");
    if ( fp2 ) fprintf (fp2, "\n");
  }
  END:;
  fclose (fp);
  fclose2 (fp2);
  return (cnt);
}

#endif

