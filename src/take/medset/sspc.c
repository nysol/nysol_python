/* SSPC: Similar Set Pair Comparison */
/* 2007/11/30 Takeaki Uno,   e-mail:uno@nii.jp, 
    homepage:   http://research.nii.ac.jp/~uno/index.html  */
/* This program is available for only academic use, basically.
   Anyone can modify this program, but he/she has to write down 
    the change of the modification on the top of the source code.
   Neither contact nor appointment to Takeaki Uno is needed.
   If one wants to re-distribute this code, do not forget to 
    refer the newest code, and show the link to homepage of 
    Takeaki Uno, to notify the news about this code for the users.
   For the commercial use, please make a contact to Takeaki Uno. */

/*   internal_params.l1 = #solutions
  internal_params.l2 = #rows
  internal_params.l3 = #columns
*/


#ifndef _sspc_c_
#define _sspc_c_

// comment out if single-core mode
//#define MULTI_CORE

#define WEIGHT_DOUBLE

#include"trsact.c"
#include"problem.c"

typedef struct {
#ifdef MULTI_CORE
  pthread_t thr; // thread identifier
#endif
  PROBLEM *PP;
  QUEUE_INT **o;
  WEIGHT *w;
  FILE *fp;
  int core_id;
  int *lock_i;
} SSPC_MULTI_CORE;

#define SSPC_INCLUSION 1
#define SSPC_SIMILARITY 2
#define SSPC_INTERSECTION 4
#define SSPC_RESEMBLANCE 8
#define SSPC_INNERPRODUCT 16
#define SSPC_MININT 32
#define SSPC_MAXINT 64
#define SSPC_PMI 128
#define SSPC_FVALUE 256
#define SSPC_COUNT 2048
#define SSPC_MATRIX 4096
#define SSPC_NO_NEIB 16384
#define SSPC_POLISH 32768
#define SSPC_POLISH2 65536
#define SSPC_OUTPUT_INTERSECT 131072
#define SSPC_COMP_ITSELF 262144

void SSPC_error (){
  ERROR_MES = "command explanation";
  print_err ("SSPC: [ISCfQq] [options] input-filename ratio/threshold [output-filename]\n\
%%:show progress, _:no message, +:write solutions in append mode\n\
#:count the number of similar records for each record\n\
i(inclusion): find pairs [ratio] of items (weighted sum) of one is included in the other (1st is included in 2nd)\n\
I(both-inclusion): find pairs s.t. the size (weight sum) of intersection is [ratio] of both\n\
S:set similarity measure to |A\\cap B| / max{|A|,|B|}\n\
s:set similarity measure to |A\\cap B| / min{|A|,|B|}\n\
T(intersection): find pairs having common [threshld] items\n\
R(resemblance): find pairs s.t. |A\\capB|/|A\\cupB| >= [threshld]\n\
P(PMI): set similarity measure to log (|A\\capB|*|all| / (|A|*|B|)) where |all| is the number of all items\n\
F:set similarity measure to F-value (2*precision*recall)/(precision+recall)\n\
C(cosign distance): find pairs s.t. inner product of their normalized vectors >= [threshld]\n\
f,Q:output ratio/size of pairs following/preceding to the pairs\n\
N:normalize the ID of latter sets, in -c mode\n\
n:do not consider a and b in the set when comparing a and b\n\
Y(y):output elements of each set that contribute to no similarity (y:fast with much memory use)\n\
1:remove duplicated items in each transaction\n\
0:compare xth row with xth row\n\
t:transpose the database so that i-th transaction will be item i\n\
E:input column-row representation\n\
w:load weight of each item in each row (with E command)\n\
[options]\n\
-2 [num]:2nd input file name\n\
-9 [th] [filename]:write pairs satisfies 2nd threshold [th] to file [filename]\n\
-K [num]:output [num] pairs of most large similarities\n\
-k [num]:output [num] elements of most large similarities, for each element\n\
-w [filename]:read item weights from [filename]\n\
-W [filename]:read item weights in each row from [filename]\n\
-l,-u [num]:ignore transactions with size (weight sum) less/more than [num]\n\
  (-ll,-uu [ratio]:give the threshold by ratio of all items/transactions)\n\
-L,-U [num]: ignore items appearing less/more than [num]\n\
  (-LL,-UU [ratio]:give the threshold by ratio of all items/transactions)\n\
-c [num]:compare transactions of IDs less than num and the others (if 0 is given, automatically set to the boundary of the 1st and 2nd file)\n\
-b [num]:ignore pairs whose maximum common item is at most [num]\n\
-B [num]:ignore pairs whose minimum common item is at lease [num]\n\
-T [num]:ignore pairs whose intersection size is less than [num]\n\
    (-TT [num]: -T with outputting intersection size to the 1st column of each line\n\
-P [filename]:compare the pairs written in the file of [filename]\n\
-M [num]: for multi-core processors, use [num] processors (threads: need to be compiled with multi_core option\n\
-# [num]:stop after outputting [num] solutions\n\
-, [char]:give the separator of the numbers in the output\n\
-Q [filename]:replace the output numbers according to the permutation table given by [filename]\n\
# the 1st letter of input-filename cannot be '-'.\n\
# if the output file name is -, the solutions will be output to standard output.\n");
  EXIT;
//items have to begin from 1\n");
//E:read edge list file\n
}

// c:multi stream transaction mode, separated by an empty transaction

/***********************************************************************/
/*  read parameters given by command line  */
/***********************************************************************/
void SSPC_read_param (int argc, char *argv[], PROBLEM *PP){
  int c=1;
  ITEMSET *II = &PP->II;
  TRSACT *TT = &PP->TT;
  PP->th = 0; PP->th2 = 0;
  PP->II.multi_core = 1;

  if ( argc < c+3 ){ SSPC_error (); return; }

  if ( !strchr (argv[c], '_') ){ II->flag |= SHOW_MESSAGE; TT->flag |= SHOW_MESSAGE; }
  if ( strchr (argv[c], '%') ) II->flag |= SHOW_PROGRESS;
  if ( strchr (argv[c], '+') ) II->flag |= ITEMSET_APPEND;
  if ( strchr (argv[c], 'f') ) II->flag |= ITEMSET_FREQ;
  if ( strchr (argv[c], 'Q') ) II->flag |= ITEMSET_PRE_FREQ;
  if ( strchr (argv[c], 'M') ) PP->problem |= SSPC_MATRIX;
  if ( strchr (argv[c], 'i') ) PP->problem = SSPC_INCLUSION;
  else if ( strchr (argv[c], 'I') ) PP->problem = SSPC_SIMILARITY;
  else if ( strchr (argv[c], 'T') ) PP->problem = SSPC_INTERSECTION;
  else if ( strchr (argv[c], 's') ) PP->problem = SSPC_MININT;
  else if ( strchr (argv[c], 'S') ) PP->problem = SSPC_MAXINT;
  else if ( strchr (argv[c], 'R') ) PP->problem = SSPC_RESEMBLANCE;
  else if ( strchr (argv[c], 'P') ) PP->problem = SSPC_PMI;
  else if ( strchr (argv[c], 'F') ) PP->problem = SSPC_FVALUE;
  else if ( strchr (argv[c], 'C') ) PP->problem = SSPC_INNERPRODUCT;
  else error ("i, I, s, S, R, T or C command has to be specified", EXIT);
  if ( strchr (argv[c], '#') ) PP->problem |= SSPC_COUNT;
  if ( strchr (argv[c], 'N') ) PP->problem |= PROBLEM_NORMALIZE;
  if ( strchr (argv[c], 'n') ) PP->problem |= SSPC_NO_NEIB;
  if ( strchr (argv[c], 'Y') ) PP->problem |= SSPC_POLISH;
  if ( strchr (argv[c], 'y') ) PP->problem |= SSPC_POLISH2;
  if ( !strchr (argv[c], 't') ) TT->flag |= LOAD_TPOSE;
  if ( strchr (argv[c], 'E') ) TT->flag |= LOAD_ELE;
  if ( strchr (argv[c], 'w') ) TT->flag |= LOAD_EDGEW;
  if ( strchr (argv[c], '1') ) TT->flag |= LOAD_RM_DUP;
  if ( strchr (argv[c], '0') ) PP->problem |= SSPC_COMP_ITSELF;
  c++;
  
  while ( argv[c][0] == '-' ){
    switch (argv[c][1]){
      case 'K': II->topk_k = atoi(argv[c+1]);
      break; case 'k': II->itemtopk_item = atoi(argv[c+1]); II->itemtopk_item2 = 1;
      break; case 'L': if ( argv[c][2] == 'L' ) TT->row_lb_ = atof(argv[c+1]);
            else TT->row_lb = atoi(argv[c+1]); 
      break; case 'U':  if ( argv[c][2] == 'U' ) TT->row_ub_ = atof(argv[c+1]);
            else TT->row_ub = atoi(argv[c+1]);
      break; case 'l':  if ( argv[c][2] == 'l' ) TT->clm_lb_ = atof(argv[c+1]);
            else TT->w_lb = atof(argv[c+1]);
      break; case 'u': if ( argv[c][2] == 'u' ) TT->clm_ub_ = atof(argv[c+1]);
            else TT->w_ub = atof(argv[c+1]);
      break; case 'w': PP->TT.wfname = argv[c+1];
      break; case 'W': PP->TT.item_wfname = argv[c+1];
      break; case 'c': PP->dir = 1; PP->root = TT->sep = atoi(argv[c+1]);
      break; case '2': PP->TT.fname2 = argv[c+1];
      break; case '9': PP->th2 = atof(argv[c+1]); c++; PP->output_fname2 = argv[c+1];
      break; case 'P': PP->table_fname = argv[c+1];
          if ( TT->flag & LOAD_TPOSE ) TT->flag -= LOAD_TPOSE; else TT->flag |= LOAD_TPOSE;
      break; case 'b': PP->II.len_lb = atoi(argv[c+1]);
      break; case 'B': PP->II.len_ub = atoi(argv[c+1]);
      break; case 'T': PP->th = atoi(argv[c+1]);
        if ( argv[c][2] == 'T' ){ PP->problem |= SSPC_OUTPUT_INTERSECT; }
      break; case 'M': if ((PP->II.multi_core=atoi(argv[c+1])) <= 0 || atoi(argv[c+1])>CORE_MAX)
          error_num("number of cores has to be in 1 to",atof(argv[c+1]), EXIT);
      break; case '#': II->max_solutions = atoi(argv[c+1]);
      break; case ',': II->separator = argv[c+1][0];
      break; case 'Q': PP->outperm_fname = argv[c+1];
      break; default: goto NEXT;
    }
    c += 2;
    if ( argc<c+2 ){ SSPC_error (); return; }
  }

  NEXT:;
  if ( PP->problem & SSPC_MATRIX ) PP->MM.fname = argv[c];
  else PP->TT.fname = argv[c];
  II->frq_lb = atof(argv[c+1]);
  if ( argc>c+2 ) PP->output_fname = argv[c+2];
}

WEIGHT SSPC_comp (PROBLEM *PP, WEIGHT c, WEIGHT wi, WEIGHT wx, WEIGHT sq){
  WEIGHT cc, ccc;
  if ( PP->problem & SSPC_INTERSECTION ) return (c);
  else if ( PP->problem & SSPC_INNERPRODUCT ){
    if ( PP->TT.T.w ) return (c);
    else return (c / sq / sqrt(wx));
  } else if ( (PP->problem & SSPC_RESEMBLANCE) && (cc= wi +wx -c) != 0 ) return (c/cc);
  else if ( (PP->problem & SSPC_MAXINT) && (cc=MAX(wi,wx)) != 0 ) return (c/cc);
  else if ( (PP->problem & SSPC_MININT) && (cc=MIN(wi,wx)) != 0 ) return (c/cc);
  else if ( (PP->problem & SSPC_PMI) && (cc=wi*wx) != 0 ) return ( log( ( c * PP->TT.T.t) / cc ) /  -log ( c / PP->TT.T.t));
  else if ( (PP->problem & SSPC_FVALUE) && MAX(wi,wx) != 0 ){ cc = c/wi; ccc = c/wx; return ( 2*cc*ccc / (cc+ccc)); }
  return (-WEIGHTHUGE);
}


void SSPC_output (PROBLEM *PP, QUEUE_INT *cnt, QUEUE_INT i, QUEUE_INT ii, QUEUE *itemset, WEIGHT frq, int core_id){
  size_t b;
  PERM *p = (PERM *)PP->position_fname;
  if ( PP->problem & (SSPC_POLISH+SSPC_POLISH2) ){
    PP->vecchr[i] = 1;
    if ( PP->problem & SSPC_POLISH2 ){  // store the solution
      SPIN_LOCK (PP->II.multi_core, PP->II.lock_counter);  // lock!!
      if ( (b=PP->itemary[PP->TT.T.clms]) ){
        PP->itemary[PP->TT.T.clms] = PP->buf[b]; // use deleted cell
      } else {  // allocate new cell
        b = PP->buf_end;
        realloci (PP->buf, b+30, EXIT);
        PP->buf_end += 2;
      }
      PP->buf[b] = PP->itemary[i];  // insert the cell to list i
      PP->buf[b+1] = ii;
      PP->itemary[i] = b;
      SPIN_UNLOCK (PP->II.multi_core, PP->II.lock_counter);  // unlock!!
    }
  } else if ( PP->problem & SSPC_COUNT ) (*cnt)++;
  else {
    if ( PP->problem & SSPC_OUTPUT_INTERSECT ){
      FILE2_print_int (&PP->II.multi_fp[core_id], PP->siz, 0);
      FILE2_putc (&PP->II.multi_fp[core_id], ' ');
    }
    if ( PP->table_fname ){ itemset->v[0] = i; itemset->v[1] = ii; }
      else { itemset->v[0] = p[i]; itemset->v[1] = p[ii]; }
    if ( (PP->problem&PROBLEM_NORMALIZE)&& PP->dir>0 ){
      if ( i >= PP->TT.sep ) itemset->v[0] -= PP->root;
      if ( ii >= PP->TT.sep ) itemset->v[1] -= PP->root;
    }

    if ( PP->II.itemtopk_end > 0 )
        ITEMSET_output_itemset_ (&PP->II, itemset, frq, frq, NULL, i, ii, core_id);
    ITEMSET_output_itemset_ (&PP->II, itemset, frq, frq, NULL, ii, i, core_id);
  }
}

/* output a pair if it is in the similarity */
void SSPC_comp2 (PROBLEM *PP, QUEUE_ID i, QUEUE_ID x, WEIGHT c, WEIGHT wi, WEIGHT wx, double sq, QUEUE_INT *cnt, FILE *fp, QUEUE *itemset, int core_id){
  WEIGHT f1, f2, f1_=0, f2_=0, frq;
  ITEMSET *II = &PP->II;
  int f=0, f_=0;
  QUEUE_ID i_ = i, x_ = x;
  PERM *p = (PERM *)PP->position_fname;

  if ( !PP->table_fname ){ x_ = p[x_]; i_ = p[i_]; }

  if ( (PP->problem&PROBLEM_NORMALIZE)&& PP->dir>0 ){
    if ( i_ >= PP->TT.sep ) i_ -= PP->root;
    if ( x_ >= PP->TT.sep ) x_ -= PP->root;
  }

  if ( c < PP->th ) return;  // threshold for the intersection size 
  PP->siz = c; // outputting intersection size
  if ( PP->problem & (SSPC_INTERSECTION +SSPC_RESEMBLANCE +SSPC_INNERPRODUCT +SSPC_MAXINT +SSPC_MININT + SSPC_PMI + SSPC_FVALUE) ){
    frq = SSPC_comp (PP, c, wi, wx, sq);
    if ( frq == -WEIGHTHUGE ) return;
    if ( frq >= II->frq_lb ) SSPC_output (PP, cnt, x, i, itemset, frq, core_id);
    if ( PP->output_fname2 && frq >= PP->th2 ){
      SPIN_LOCK(II->multi_core, II->lock_output);
      fprintf (fp, "%d %d\n", x_, i_);
      SPIN_UNLOCK(II->multi_core, II->lock_output);
    }
  } else {
    f1 = wi*II->frq_lb; f2 = wx*II->frq_lb;  // size of i and x
    if ( PP->output_fname2 ){ f1_ = wi*PP->th2; f2_ = wx*PP->th2; }
    if ( PP->problem & SSPC_SIMILARITY ){
      if ( PP->output_fname2 ) f_ = ( (c >= f1_) && (c >= f2_) );
      else f = ( (c >= f1) && (c >= f2) );
      frq = MIN(c / wi, c / wx);
    } else if ( PP->problem & SSPC_INCLUSION ){
      if ( c >= f2 ){
        frq = c / wx;
        SSPC_output (PP, cnt, x, i, itemset, frq, core_id);
      }
      f = ( c >= f1 );
      frq = c / wi;
      if ( PP->output_fname2 ){
        if ( c >= f2_ ){
          SPIN_LOCK(II->multi_core, II->lock_output);
          fprintf (fp, "%d %d\n", x_, i_);
          SPIN_UNLOCK(II->multi_core, II->lock_output);
        }
        f_ = (c >= f1_);
      }
    } else return;
    if ( f ) SSPC_output (PP, cnt, i, x, itemset, frq, core_id);
    if ( PP->output_fname2 && f_ ){
      SPIN_LOCK(II->multi_core, II->lock_output);
      fprintf (fp, "%d %d\n", i_, x_);
      SPIN_UNLOCK(II->multi_core, II->lock_output);
    }
  }
}

/* initialization, for delivery */
WEIGHT *SSPC_init1 (PROBLEM *PP, FILE **fp){
  WEIGHT *w, *y;
  TRSACT *TT = &PP->TT;
  QUEUE_ID i;
  QUEUE_INT *x;

    // initialization
  calloc2 (w, TT->T.clms*2, EXIT);
  if ( (PP->problem&SSPC_INNERPRODUCT) && !TT->T.w ) FLOOP (i, 0, TT->T.clms) TT->w[i] *= TT->w[i];
  TRSACT_delivery (TT, &TT->jump, w, w+TT->T.clms, NULL, TT->T.clms);
//  FLOOP (i, 0, PP->dir?TT->sep:1) TT->OQ[i].end = 0;
  if ( (PP->problem & SSPC_INNERPRODUCT) && TT->T.w ){  // normalize the vectors for inner product
    ARY_FILL (w, 0, TT->T.clms, 0);
    FLOOP (i, 0, TT->T.t){
      y = TT->T.w[i];
      MQUE_FLOOP (TT->T.v[i], x){ w[*x] += (*y)*(*y); y++; }
    }
    FLOOP (i, 0, TT->T.clms) w[i] = sqrt(w[i]);
    FLOOP (i, 0, TT->T.t){
      y = TT->T.w[i];
      MQUE_FLOOP (TT->T.v[i], x){ *y /= w[*x]; y++; }
    }
  }
  FLOOP (i, 0, TT->T.clms) TT->OQ[i].end = 0;
  return (w);
}

/* initialization */
QUEUE_INT **SSPC_init2 (PROBLEM *PP){
  TRSACT *TT = &PP->TT;
  QUEUE_ID i, j;
  QUEUE_INT **o=NULL;

    // skipping items of large frequencies
  if ( TT->flag & LOAD_SIZSORT ){
    malloc2 (o, TT->T.clms, EXIT);
    FLOOP (i, 0, TT->T.clms){
      o[i] = TT->OQ[i].v;
      TT->OQ[i].v[TT->OQ[i].t] = INTHUGE;  // put end-mark at the last; also used in main loop
      for (j=0 ; TT->OQ[i].v[j] < PP->II.len_lb ; j++);
      TT->OQ[i].v = &TT->OQ[i].v[j]; TT->OQ[i].t -= j;
    }
  }
  return (o);
}

/* unify & SIZSORT & itemweigts are not yer implemented */ 
/* not yet checked -L,-U option, -l.-u, -i,-I options */
void SSPC_list_comp (PROBLEM *PP){
  ITEMSET *II = &PP->II;
  TRSACT *TT = &PP->TT;
  LONG i, j;
  QUEUE_INT *x, *y, *yy;
  WEIGHT *xw=0, *yw=0, c, *w, wx, wi;
  FILE2 fp;
  FILE *fp2 = NULL;
  int cnt=0;
  PERM *p = II->perm;

  II->perm = NULL; // for outputting usual numbers in ITEMSET_output_itemset, that are not items (o.w., will be permuted by II->perm)
  if ( PP->output_fname2 ) fopen2 (fp2, PP->output_fname2, "w", EXIT);
  PP->II.itemset.t = 2;
  calloc2 (w, TT->T.t, EXIT);
  if ( (PP->problem & SSPC_INNERPRODUCT) && !TT->T.w ){
    FLOOP (i, 0, TT->T.clms) TT->w[i] *= TT->w[i];
  }
  FLOOP (i, 0, TT->T.t){
    w[i] = 0;
    if ( (PP->problem & SSPC_INNERPRODUCT) && TT->T.w){
      FLOOP (yw, TT->T.w[i], TT->T.w[i]+TT->T.v[i].t) w[i] += (*yw)*(*yw);
      w[i] = sqrt (w[i]);
      FLOOP (yw, TT->T.w[i], TT->T.w[i]+TT->T.v[i].t) (*yw) /= w[i];
    } else {
      MQUE_FLOOP (TT->T.v[i], x) w[i] += TT->w[*x];
    }
  }

  FILE2_open (fp, PP->table_fname, "r", EXIT);
  do {
    if ( FILE2_read_pair (&fp, &i, &j, &c, 0) ) continue;
    y = TT->T.v[j].v; yy = y+TT->T.v[j].t; 
    c = 0; wi = w[i], wx = w[j];
    if ( TT->T.w ){ xw = TT->T.w[i]; yw = TT->T.w[j]; }
//printf ("%lld %lld\n", i, j);
//MQUE_FLOOP (TT->T.v[i], x){ printf ("%d ", *x); } printf ("\n");
//MQUE_FLOOP (TT->T.v[j], x){ printf ("%d ", *x); } printf ("\n");

    if (PP->problem & SSPC_NO_NEIB){
      MQUE_FLOOP (TT->T.v[i], x){
        if ( *x == j ) wi -= (PP->problem & SSPC_INNERPRODUCT)?  (*xw) * (*xw): TT->w[*x];
        while (*y < *x){
          if ( *y == i ) wx -= (PP->problem & SSPC_INNERPRODUCT)?  (*yw) * (*yw): TT->w[*y];
          y++; yw++;
          if ( y == yy ) goto END2;
        }
        if ( *x == *y && *x != j && *y != i ){  // for usual
          if ( TT->T.w ){
            if (PP->problem & SSPC_INNERPRODUCT){
              c += (*xw) * (*yw);
            } else c += *yw;
          } else c += TT->w[*x];
        }
        xw++;
        END2:;
      }
      while (y != yy){
        if ( *y == i ) wx -= (PP->problem & SSPC_INNERPRODUCT)?  (*yw) * (*yw): TT->w[*y];
        y++; yw++;
      }
    } else {
      MQUE_FLOOP (TT->T.v[i], x){ // for usual
        while (*y < *x){
          y++; yw++;
          if ( y == yy ) goto END;
        }
        if ( *x == *y ){  
          if ( TT->T.w ){
            if (PP->problem & SSPC_INNERPRODUCT){
              c += (*xw) * (*yw);
            } else c += *yw;
          } else c += TT->w[*x];
        }
      }
      xw++;
      END:;
    }
    SSPC_comp2 (PP, i, j, c, wi, wx, sqrt(w[i]), &cnt, fp2, &II->itemset, 0);
  } while ( (FILE_err&2)==0 );
  FILE2_close (&fp);
  fclose2 (fp2);
  II->perm = p;
}

/* iteration for muticore mode */
void *SSPC_iter (void *p){
  SSPC_MULTI_CORE *SM = (SSPC_MULTI_CORE *)p;
  int core_id = SM->core_id;
  PROBLEM *PP = SM->PP;
  QUEUE_INT **o = SM->o;
  WEIGHT *w = SM->w;
  FILE *fp = SM->fp;
  QUEUE jump, itemset;
  char *mark = NULL;
  TRSACT *TT = &PP->TT;
  QUEUE_ID ii=0, t, m;
  QUEUE_INT i=0, i_=0, ff;
  QUEUE_INT *OQend, cnt=((PP->problem&SSPC_COMP_ITSELF) && PP->dir == 0)?1:0, *x, *oi, *oj;
  WEIGHT *occ_w, *occ_pw, c, *y, yy=0, wi=0, wx=0;
  double sq = 0;
  ITEMSET *II = &PP->II;
  size_t b, bb;
  int f, pf = TT->flag2&TRSACT_NEGATIVE;  // dealing with only positive weitht, not yet implemented

  if ( PP->problem & SSPC_NO_NEIB ) calloc2 (mark, TT->T.clms, EXIT);
  calloc2 (occ_w, TT->T.clms, EXIT);
  calloc2 (occ_pw, TT->T.clms, EXIT);
  calloc2 (OQend, TT->T.clms, EXIT);
  QUEUE_alloc (&jump, TT->T.clms);
  QUEUE_alloc (&itemset, 2);
  itemset.t = 2;
  
  while (1){
    if ( i == i_ ){
      i_ = 100000000 / (TT->T.eles / TT->T.clms);
      SPIN_LOCK (II->multi_core, II->lock_counter);  // lock!!
      if ( (i = *(SM->lock_i)) >= TT->T.clms ){
        SPIN_UNLOCK (II->multi_core, II->lock_counter);  // unlock!!
        break;
      }
//      i_ = MIN(TT->T.clms, i + (i_ / (i+10)) + 1);
      i_ = MIN(TT->T.clms, i + 100);
      (*(SM->lock_i)) = i_;
      if ( II->flag & SHOW_PROGRESS ){
        if ( (int)((i-1)*100/TT->T.clms) < (int)(i*100/TT->T.clms) )
            fprintf (stderr, "%d%%\n", (int)(i*100/TT->T.clms));
      }
      SPIN_UNLOCK (II->multi_core, II->lock_counter);  // unlock!!
    }
    if ( (PP->problem & SSPC_INNERPRODUCT) && !TT->T.w ) sq = sqrt (w[i]);
     // delivery    
    if ( PP->problem & (SSPC_POLISH+SSPC_POLISH2) ) m = PP->TT.T.clms;
    else m = (PP->dir>0)?TT->sep:i;
    jump.t = jump.s;

    if (PP->problem & SSPC_NO_NEIB){ // for no_neib
      MQUE_FLOOP (TT->T.v[i], x) mark[*x] |= 1;
      MQUE_FLOOP (TT->OQ[i], x) mark[*x] |= 2;
    }

    FLOOP (ii, TT->OQ[i].s, TT->OQ[i].t){
      t = TT->OQ[i].v[ii];
      if ( TT->T.w && (PP->problem & SSPC_INNERPRODUCT)){ // get item weight of current vector
        y = TT->T.w[t];
        MQUE_MLOOP (TT->T.v[t], x, i) y++;
        yy = *y;
      }
      if ( (PP->problem & SSPC_NO_NEIB) && t == i ) continue;
      if ( TT->T.w ) y = TT->T.w[t]; else y = 0;

      MQUE_MLOOP (TT->T.v[t], x, m){
        if ( (PP->problem & SSPC_POLISH2) && *x < i) continue;
        if ( (PP->problem & SSPC_NO_NEIB) && *x == t ) continue;
        if ( OQend[*x] == 0 ){
          QUE_INS (jump, *x);
          occ_w[*x] = 0;
          if ( pf ) occ_pw[*x] = 0;
        }
        OQend[*x]++;
        if ( TT->T.w ){
          if (PP->problem & SSPC_INNERPRODUCT){
            occ_w[*x] += (*y) * yy; if ( *y>0 && pf) occ_pw[*x] += (*y) * yy;
          } else { occ_w[*x] += *y; if ( *y>0 && pf) occ_pw[*x] += *y; }
          y++;
        } else {
          occ_w[*x] += TT->w[t]; if ( pf ) occ_pw[*x] += TT->pw[t];
        }
      }
    }

    MQUE_FLOOP (jump, x){
      if ( *x == i ) goto SKIP;
      c = occ_w[*x];

      if ( TT->flag & LOAD_SIZSORT ){
        for (oi=o[i],oj=o[*x] ; *oi<II->len_lb ; oi++ ){
          while ( *oj < *oi ) oj++;
          if ( *oi == *oj ) c += TT->w[*oi];
        }
      }
      wi = w[i];
      wx = w[*x];
      if (PP->problem & SSPC_NO_NEIB){  // for no_neib
        if ( mark[*x]&1 ) wi -= TT->w[*x];
        if ( mark[i]&2 ) wx -= TT->w[i];
      }

      SSPC_comp2 (PP, i, *x, c, wi, wx, sq, &cnt, fp, &itemset, core_id);
      SKIP:;
      OQend[*x] = 0;
    }
    if ((PP->problem & SSPC_COMP_ITSELF) && PP->dir == 0)  // selfcomparison
        SSPC_output (PP, &cnt, i, i, &itemset, ((PP->problem&PROBLEM_NORMALIZE)&& PP->dir>0)? i-TT->sep: i, core_id);

    if ( PP->problem & (SSPC_POLISH+SSPC_POLISH2) ){  // data polish;  clear OQ, and marks
      if ( PP->problem & SSPC_POLISH2 )  // data polish;  clear OQ, and marks
          for (b=PP->itemary[i] ; b ; b=PP->buf[b]) PP->vecchr[PP->buf[b+1]] = 1;
      f = 0;
      FLOOP (ii, TT->OQ[i].s, TT->OQ[i].t){
        t = TT->OQ[i].v[ii]; ff = 0;
        MQUE_MLOOP (TT->T.v[t], x, PP->TT.T.clms)
            if ( PP->vecchr[*x] ){ ff = 1; break; }
        if ( ff ){
          FILE2_print_int (&II->multi_fp[core_id], t, f);
          f = II->separator;
        }
      }
      FILE2_putc (&II->multi_fp[core_id], '\n');
      FILE2_flush (&II->multi_fp[core_id]);
      MQUE_FLOOP (jump, x) PP->vecchr[*x] = 0;  // clear mark
      if ( PP->problem & SSPC_POLISH2 ){  // data polish;  clear OQ, and marks
        for (b=PP->itemary[i] ; b ; b=bb){ // insert cells to deleted cell queue
          bb = PP->buf[b];
          PP->vecchr[PP->buf[b+1]] = 0;
          PP->buf[b] = PP->itemary[PP->TT.T.clms];
          PP->itemary[PP->TT.T.clms] = b;
        }
      }
    }
/*     else if ( PP->problem & SSPC_POLISH2 ){  // data polish;  clear OQ, and marks
      f = 0;
      for (b=PP->itemary[i] ; b ; b=PP->buf[b]) PP->vecchr[PP->buf[b+1]] = 1;
      FLOOP (ii, TT->OQ[i].s, TT->OQ[i].t){
        t = TT->OQ[i].v[ii]; ff = 0;
        MQUE_MLOOP (TT->T.v[t], x, m)
            if ( PP->vecchr[*x] ){ ff = 1; break; }
        if ( ff ){
          FILE2_print_int (&PP->II.multi_fp[0], t, f);
          f = PP->II.separator;
        }
      }
      FILE2_putc (&PP->II.multi_fp[0], '\n');
      FILE2_flush (&PP->II.multi_fp[0]);
      MQUE_FLOOP (TT->jump, x) PP->vecchr[*x] = 0;  // clear mark
    }
*/
    if (PP->problem & SSPC_NO_NEIB){ // for no_neib
      MQUE_FLOOP (TT->T.v[i], x) mark[*x] = 0;
      MQUE_FLOOP (TT->OQ[i], x) mark[*x] = 0;
    }

    TT->OQ[i].end = 0;
    if ( PP->problem & SSPC_COUNT ){
      while ( ii<II->perm[i] ){
        FILE2_putc (&II->multi_fp[core_id], '\n');
        FILE2_flush (&II->multi_fp[core_id]);
        ii++;
      }
      FILE2_print_int (&II->multi_fp[core_id], cnt, 0);
      FILE2_putc (&II->multi_fp[core_id], '\n');
      FILE2_flush (&II->multi_fp[core_id]);
      II->sc[2] += cnt;
      ii++;
    }
    i++;
  }
  mfree (mark, occ_w, occ_pw, OQend);
  QUEUE_end (&jump);
  QUEUE_end (&itemset);
  return (NULL);
}

/*************************************************************************/
/* SSPC main routine */
/*************************************************************************/
void SSPC (PROBLEM *PP){
  SSPC_MULTI_CORE *SM = NULL;
  ITEMSET *II = &PP->II;
  TRSACT *TT = &PP->TT;
  QUEUE_ID i, begin = (PP->problem&(SSPC_POLISH+SSPC_POLISH2))?0:(PP->dir>0?TT->sep:0);
  QUEUE_INT **o;
  WEIGHT *w;
  FILE *fp = NULL;  // file pointer for the second output file
#ifdef MULTI_CORE
  void *tr;
#endif
  int cnt;
  QUEUE itemset;

  if ( PP->output_fname2 ) fopen2 (fp, PP->output_fname2, "w", EXIT);

    // initialization
  w = SSPC_init1 (PP, &fp);
  o = SSPC_init2 (PP);
  if ((PP->problem & SSPC_COMP_ITSELF) && PP->dir == 0){ // selfcomparison
    QUEUE_alloc (&itemset, 2);
    itemset.t = 2;
    SSPC_output (PP, &cnt, 0, 0, &itemset, 0, 0);
    QUEUE_end (&itemset);
  }
   // for multi-core
  malloc2 (SM, II->multi_core, EXIT);
  BLOOP (i, II->multi_core, 0){
    SM[i].PP = PP;
    SM[i].o = o;
    SM[i].w = w;
    SM[i].fp = fp;
    SM[i].core_id = i;
    SM[i].lock_i = &begin;
#ifdef MULTI_CORE
    if ( i > 0 ) pthread_create (&(SM[i].thr), NULL, SSPC_iter, (void*)(&SM[i]));
    else 
#endif
      SSPC_iter ((void*)(&SM[i]));
  }
      // wait until all-created-threads terminate
#ifdef MULTI_CORE
  FLOOP (i, 1, II->multi_core) pthread_join (SM[i].thr, &tr);
#endif

    // termination
  if ( TT->flag & LOAD_SIZSORT ){
    FLOOP (i, 0, TT->T.clms){
      TT->OQ[i].t += TT->OQ[i].v - o[i];
      TT->OQ[i].v = o[i];
    }
  }
  mfree (w, o, SM);
  fclose2 (fp);
}


/*************************************************************************/
/* SSPC matrix version */
/*************************************************************************/
void SSPCmat (PROBLEM *PP){
  ITEMSET *II = &PP->II;
  MAT *MM = &PP->MM;
  QUEUE_ID i, j, x, begin = PP->dir>0?0:1, f, ii=0;
  QUEUE_INT cnt;
  WEIGHT *w, f1, f2, c, cc;
  double sq =0;

  II->frq_lb = II->frq_lb * II->frq_lb;

    // initialization
//  calloc2 (w, MM->t, EXIT);
//  if ( PP->problem & SSPC_INNERPRODUCT ) FLOOP (i, 0, MM->clms) TT->w[i] *= TT->w[i];
//  TRSACT_delivery (TT, &TT->jump, w, w+TT->T.clms, NULL, TT->T.clms);
//  FLOOP (i, 0, PP.TT.T.clms) TT->OQ[i].end = 0;
  II->itemset.t = 2;

    // skipping items of large frequencies
//  if ( TT->flag & LOAD_SIZSORT ){
//    malloc2 (o, TT->T.clms, EXIT);
//    FLOOP (i, 0, TT->T.clms){
//      o[i] = TT->OQ[i].v;
//      TT->OQ[i].v[TT->OQ[i].t] = INTHUGE;  // put end-mark at the last; also used in main loop
//      for ( j=0 ; TT->OQ[i].v[j] < PP->II.len_lb ; j++ );
//      TT->OQ[i].v = &TT->OQ[i].v[j]; TT->OQ[i].t -= j;
//    }
//  }
  
    // main loop
  FLOOP (i, begin, MM->t){
    cnt = 0;
    II->itemset.v[0] = ((PP->problem&PROBLEM_NORMALIZE)&& PP->dir>0)? i-MM->clms: i; // i-TT->sep
    if ( PP->problem || 1 ){
      PP->occ_w[i] = 0;
      FLOOP (x, 0, MM->clms) PP->occ_w[i] += MM->v[i].v[x] * MM->v[i].v[x];
    }

    FLOOP (j, 0, PP->dir>0?begin:i){
      II->itemset.v[1] = j;
      f = 0; sq = 0;
      FLOOP (x, 0, MM->clms) sq += MM->v[i].v[x] * MM->v[j].v[x];
      if ( sq / PP->occ_w[i] / PP->occ_w[j] > II->frq_lb ) f = 1;

      if ( f ){
        if ( PP->problem & SSPC_COUNT ) cnt++;
        else ITEMSET_output_itemset (II, NULL, 0);
      }
    }
    if ( PP->problem & SSPC_COUNT ){
      while ( ii<II->perm[i] ){
        FILE2_putc (&II->multi_fp[0], '\n');
        FILE2_flush (&II->multi_fp[0]);
        ii++;
      }
      FILE2_print_int (&II->multi_fp[0], cnt, 0);
      FILE2_putc (&II->multi_fp[0], '\n');
      FILE2_flush (&II->multi_fp[0]);
      II->sc[2] += cnt;
      ii++;
    }
  }

    // termination
//  mfree (w, o);
}



/*************************************************************************/
/* main function of SSPC */
/*************************************************************************/
int SSPC_main (int argc, char *argv[]){
  PROBLEM PP;
  SETFAMILY *T = &PP.TT.T;
  QUEUE_ID i;

  PROBLEM_init (&PP);
  SSPC_read_param (argc, argv, &PP);
if ( ERROR_MES ) return (1);

  PP.TT.flag |= LOAD_INCSORT;
  PP.TT.flag2 |= TRSACT_ALLOC_OCC;
  if ( PP.II.len_ub<INTHUGE || PP.II.len_lb>0 ) PP.TT.flag |= LOAD_SIZSORT+LOAD_DECROWSORT;
  PROBLEM_load (&PP);
//TRSACT_print (&PP.TT, NULL, NULL);
//printf ("come\n");
  internal_params.l2 = T->t;
  internal_params.l3 = PP.TT.clms_org;

  if ( PP.II.len_ub < INTHUGE ){
    FLOOP (i, 0, PP.TT.T.t) if ( PP.TT.T.v[i].t <= PP.II.len_ub ){ PP.II.len_lb = i; break; }
  }
  if ( PP.II.itemtopk_item > 0 ) PP.II.itemtopk_end = T->clms; 
  PROBLEM_alloc (&PP, T->clms, T->t, 0, PP.TT.perm, PROBLEM_OCC_W +PROBLEM_VECCHR +((PP.problem&SSPC_POLISH2)?PROBLEM_ITEMARY:0));
  PP.TT.perm = NULL;
  realloc2 (PP.TT.w, MAX(T->t, T->clms)+1, EXIT);
  ARY_FILL (PP.TT.w, 0, MAX(T->t, T->clms)+1, 1);
  
  print_mes (&PP.TT, "separated at %d\n", PP.TT.sep);
  PP.buf_end = 2;
  PP.position_fname = (char *)PP.II.perm; PP.II.perm = NULL;
  if ( !ERROR_MES && PP.TT.T.clms>1 ){
    if ( PP.problem & SSPC_MATRIX ){ SSPCmat (&PP); }
    else if ( PP.table_fname ){ SSPC_list_comp (&PP); }
    else {
      QUEUE_delivery (PP.TT.OQ, NULL, NULL, T->v, &PP.TT.OQ[T->clms], T->t, T->clms);
      SSPC (&PP);
    }
  }
  PP.II.perm = (PERM *)PP.position_fname;
  ITEMSET_merge_counters (&PP.II);
  internal_params.l1 = PP.II.solutions;

  if ( PP.II.topk.end > 0 || PP.II.itemtopk_end > 0 ) ITEMSET_last_output (&PP.II);
  else print_mes (&PP.TT, LONGF " pairs are found\n", PP.II.sc[2]);

  PROBLEM_end (&PP);
  return (ERROR_MES?1:0);
}

/*******************************************************************************/
#ifndef _NO_MAIN_
#define _NO_MAIN_
int main (int argc, char *argv[]){
  return (SSPC_main (argc, argv) );
}
#endif
/*******************************************************************************/

#endif
