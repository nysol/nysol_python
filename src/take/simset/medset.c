/* take the intersection of each cluster */
/* 8/Nov/2008   Takeaki Uno  */

#ifndef _medset_c_
#define _medset_c_

#include"fstar.c"
#include"problem.c"

#define MEDSET_CC 4
#define MEDSET_IND 8
#define MEDSET_NO_HIST 16
#define MEDSET_RATIO 32
#define MEDSET_ALLNUM 64
#define MEDSET_BELOW 128

void MEDSET_error (){
  ERROR_MES = "command explanation";
  print_err ("medset: compute the intersection of each set of sets\n\
usage: medset [-HRTIitl] cluster-filename set-filename threshold output-filename\n\
if threshold is negative, output the items whose frequencies are no more than the threshold\n\
-%%: show progress, -_: no message\n\
-H: do not use histgram, just output the items\n\
-R: do not output singleton clusters\n\
-V: output ratio of appearances of all items\n\
-T: clustering by connected component (read edge type file)\n\
-I: find an independent set, and clustering by using the vertices in it as seeds (read edge type files)\n\
-i: output for each item, ratio of records including the item\n\
-t: transpose the input database, (transaction will be item, and vice varsa)\n\
-l [num]: output clusters of size at least [num]\n\
# the 1st letter of input-filename cannot be '-'.\n");
  EXIT;
//-c [num]: specify the connectivity for the connected component clustering (-T)\n
}

/* read commands and options from command line */
void MEDSET_read_param (PROBLEM *PP, int argc, char *argv[]){
  int c=1;

  if ( argc < c+3 ){ MEDSET_error (); return; }
  PP->dir = 1; PP->FS.flag |= SHOW_MESSAGE;
  while ( argv[c][0] == '-' ){
    if ( argc<c+3 ){ MEDSET_error (); return; }
    switch ( argv[c][1] ){
      case 't': PP->FS.flag |= LOAD_TPOSE;
      break; case '_': PP->FS.flag -= SHOW_MESSAGE;   // connected component clustering
      break; case '%': PP->FS.flag |= SHOW_PROGRESS;   // connected component clustering
      break; case 'T': PP->problem |= MEDSET_CC;   // connected component clustering
      break; case 'I': PP->problem |= MEDSET_IND;   // independent set clustering
      break; case 'H': PP->problem |= MEDSET_NO_HIST;   // do not use histgram
      break; case 'V': PP->problem |= MEDSET_ALLNUM;   // output appearance ratio for all
      break; case 'l': PP->num = atoi(argv[c+1]); c++;   // minimum cluster size to be output
      break; case 'i': PP->problem |= MEDSET_RATIO;   // output included-ratio of items
//      break; case 'c': PP->deg = atoi(argv[c+1]); c++;   // least degree
   }
    c++;
  }
  
  PP->input_fname = argv[c];
  if ( !(PP->problem & (MEDSET_CC+MEDSET_IND))) PP->FS.fname = argv[c+1];
  PP->th = atof(argv[c+2]);
  if ( PP->th < 0 ){ PP->th = -PP->th; PP->problem |= MEDSET_BELOW; }  // output less frequency items
  PP->output_fname = argv[c+3];
}

/* read file, output the histogram of each line */
void MEDSET_read_file (PROBLEM *PP, FILE2 *fp){
  FSTAR *FS = &PP->FS;
  FSTAR_INT *cnt, *que, t, s, i, x;

  calloc2 (cnt, FS->in_node_num, EXIT);
  calloc2 (que, FS->in_node_num*2, goto END);
  
  do {
    s = t = 0;
    do {   // count #out-going edges for each vertex
      x = (FSTAR_INT)FILE2_read_int (fp);
      if ( FILE_err&4 ) break;
      if ( x<0 || x >= FS->out_node_num ){
        print_err ("set ID out of bound %d>%d\n", x, FS->out_node_num);
        exit(0);
      }
      FLOOP (i, FS->fstar[x], FS->fstar[x+1])
          if ( cnt[FS->edge[i]]++ == 0 ){ que[t*2+1] = FS->edge[i]; t++; }
      s++;
    } while ( (FILE_err&3)==0 );

    if ( PP->problem & MEDSET_ALLNUM ){
      FLOOP (i, 0, FS->in_node_num){
        fprintf (PP->II.fp, "%.2f ", ((double)cnt[i])/(double)s);
        cnt[i] = 0;
      }
      fprintf (PP->II.fp, "\n");
      continue;
    }

    if ( s>0 ){
      FLOOP (i, 0, t){ que[i*2] = cnt[que[i*2+1]]; cnt[que[i*2+1]] = 0; }
      qsort_FSTAR_INT (que, t, (PP->problem&MEDSET_BELOW?1:-1)*((int)sizeof(FSTAR_INT))*2);
/*
      FLOOP (i, 0, t){
        if ( ((double)cnt[que[i*2+1]])/(double)s >= PP->th )
            fprintf (PP->II.fp, "(%d:%.2f) ", que[i], ((double)cnt[que[i]])/(double)s);
        cnt[que[i]] = 0;
      }
*/
      FLOOP (i, 0, t){
        if ( PP->problem & MEDSET_BELOW ){
           if ( ((double)que[i*2])/(double)s > PP->th ) break;
        } else if ( ((double)que[i*2])/(double)s < PP->th ) break;
        if ( PP->problem & MEDSET_NO_HIST ) fprintf (PP->II.fp, "%d ", que[i*2+1]);
        else if ( PP->problem & MEDSET_RATIO ) fprintf (PP->II.fp, "(%d:%.2f) ", que[i*2+1], ((double)que[i*2])/(double)s);
        else fprintf (PP->II.fp, "%d ", que[i*2+1]);
      }
    }
    fprintf (PP->II.fp, "\n");
  } while ( (FILE_err&2)==0 );

  END:;
  mfree (cnt, que);
}


/* output clusters to the output file */
void MEDSET_print_clusters (PROBLEM *PP, FSTAR_INT *mark, FSTAR_INT *set, FSTAR_INT xmax){
  FSTAR_INT i, x, c;
  
  FLOOP (i, 0, xmax){
    if ( mark[i] != i ) continue;
    c = 0; x = i;
    while (1){
      c++;
      if ( set[x] == x ) break;
      x = set[x];
    }
    if ( c < PP->num ) continue;
    x = i;
    while (1){
      fprintf (PP->II.fp, "%d ", x);
      if ( set[x] == x ) break;
      x = set[x];
    }
    fputs ("\n", PP->II.fp);
  }
}


/* read file, output the histogram of each line */
void MEDSET_cc_clustering (PROBLEM *PP, FILE2 *fp){
  FSTAR_INT *pnt=NULL, end1=0, end2=0, xmax=0, *mark=NULL, *set=NULL;
  LONG x, y;

    // merge the connponents to be connected by using spray tree
  do {
    if ( FILE2_read_pair (fp, &x, &y, NULL, 0) ) continue;
    ENMAX (xmax, MAX(x, y)+1);
    reallocx (mark, end1, xmax, common_size_t, EXIT);
    reallocx (set, end2, xmax, common_size_t, EXIT);
    UNIONFIND_unify_set (x, y, mark, set);
  } while ( (FILE_err&2)==0 );

  MEDSET_print_clusters (PP, mark, set, xmax);

  END:;
  mfree (mark, set);
}


/* clustering the nodes by finding independent set */
/* cnt: cluster siz, if v is representative, and #vertices covering v, if v isn't representative */
void MEDSET_ind_clustering (PROBLEM *PP, FILE2 *fp){
  FSTAR_INT *pnt=NULL, flag, end1=0, end2=0, end3=0, xmax=0, *mark=NULL, *set=NULL, *cnt=NULL;
  LONG x, y, yy;

    // merge the connponents to be connected by using spray tree
  do {
    flag = 0;
    do {
      if ( FILE2_read_pair (fp, &x, &y, NULL, 0) ) continue;
      ENMAX (xmax, MAX(x, y)+1);
      reallocx (mark, end1, xmax, common_size_t, EXIT);
      reallocx (set, end2, xmax, common_size_t, EXIT);
      reallocx (cnt, end3, xmax, 0, EXIT);
      if ( cnt[x] < cnt[y] ) SWAP_LONG (x, y);
      if ( mark[x] == x && mark[y] == y ){
        if ( set[x] == x && !(set[y]== y && cnt[y]>0) ){ UNIONFIND_unify_set (y, x, mark, set); cnt[y]++; cnt[x] = 1; flag = 1; }
        else {
          do {
            yy = set[y];
            set[y] = y;
            y = yy;
            mark[y] = y;
            cnt[y]--;
          } while (y != set[y]);
        }
        if ( set[y] == y ){ UNIONFIND_unify_set (x, y, mark, set); cnt[x]++; cnt[y] = 1; flag = 1;}
      }
      if ( mark[x] == x ){ cnt[y]++; }
      else if ( mark[y] == y ){ cnt[x]++; }
    } while ( (FILE_err&2)==0 );
  } while (flag);

  MEDSET_print_clusters (PP, mark, set, xmax);

  END:;
  mfree (mark, set, cnt);
}


/*******************************************************************/
int MEDSET_main (int argc, char *argv[]){
  PROBLEM PP;
  FILE2 fp;
  
  PROBLEM_init (&PP);
  MEDSET_read_param (&PP, argc, argv);
if ( ERROR_MES ) return (1);
  PP.FS.flag |= LOAD_BIPARTITE;
  PP.FS.edge_dir = 1;
  print_mes (&PP.FS, "medset: cluster-file= %s set-file= %s threshold= %f output-file= %s\n", PP.input_fname, PP.FS.fname, PP.th, PP.output_fname);
  PROBLEM_load (&PP);
  
  FILE2_open (fp, PP.input_fname, "r", goto END);
  fopen2 (PP.II.fp, PP.output_fname, "w", goto END);

  if ( !ERROR_MES ){
    if ( PP.problem & MEDSET_CC ) MEDSET_cc_clustering (&PP, &fp);
    else if ( PP.problem & MEDSET_IND ) MEDSET_ind_clustering (&PP, &fp);
    else MEDSET_read_file (&PP, &fp);
  }

  END:;
  FILE2_close (&fp);
  fclose2 (PP.II.fp);

  PROBLEM_end (&PP);
  return (ERROR_MES?1:0);
}


/*******************************************************************************/
#ifndef _NO_MAIN_
#define _NO_MAIN_
int main (int argc, char *argv[]){
  return (MEDSET_main (argc, argv));
}
#endif
/*******************************************************************************/

#endif


