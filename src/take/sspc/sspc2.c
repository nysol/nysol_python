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

#ifndef _sspc_c_
#define _sspc_c_

#define WEIGHT_DOUBLE

#include"trsact.c"
#include"problem.c"

#define SSPC_INCLUSION 1
#define SSPC_SIMILARITY 2
#define SSPC_INTERSECTION 4
#define SSPC_RESEMBLANCE 8
#define SSPC_INNERPRODUCT 16
#define SSPC_MININT 32
#define SSPC_MAXINT 64
#define SSPC_PMI 128
#define SSPC_COUNT 2048
#define SSPC_MATRIX 4096
#define SSPC_UNIFY 8192
#define SSPC_NO_NEIB 16384
#define SSPC_POLISH 32768


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
P(PMI): find pairs s.t. log (|A\\capB|*|all| / (|A|*|B|)) >= [threshld]\n\
C(cosign distance): find pairs s.t. inner product of their normalized vectors >= [threshld]\n\
f,Q:output ratio/size of pairs following/preceding to the pairs\n\
D:the first entry is ID, and unify the records with the same ID\n\
N:normalize the ID of latter sets, in -c mode\n\
n:do not consider a and b in the set when comparing a and b\n\
Y:output elements of each set that contribute to no similarity\n\
t:transpose the database so that i-th transaction will be item i\n\
[options]\n\
-2 [num]:2nd input file name\n\
-K [num]:output [num] pairs of most large intersections\n\
-w [filename]:read item weights from [filename]\n\
-l,-u [num]:ignore transactions with size (weight sum) less/more than [num]\n\
-L,-U [num]: ignore items appearing less/more than [num]\n\
-c [num]:compare transactions of IDs less than num and the others (if 0 is given, automatically set to the boundary of the 1st and 2nd file)\n\
-b [num]:ignore pairs having no common item of at least [num]th frequency\n\
-B [num]:ignore pairs having no common item of frequency at most [num]\n\
-T [num]:ignore pairs whose intersection size is less than [num]\n\
-# [num]:stop after outputting [num] solutions\n\
-, [char]:give the separator of the numbers in the output\n\
-Q [filename]:replace the output numbers according to the permutation table given by [filename]\n\
# the 1st letter of input-filename cannot be '-'.\n\
# if the output file name is -, the solutions will be output to standard output.\n");
  EXIT;
//items have to begin from 1\n");
}

// c:multi stream transaction mode, separated by an empty transaction

/***********************************************************************/
/*  read parameters given by command line  */
/***********************************************************************/
void SSPC_read_param (int argc, char *argv[], PROBLEM *PP){
  int c=1;
  ITEMSET *II = &PP->II;
  TRSACT *TT = &PP->TT;
  PP->th = 0;
  
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
  else if ( strchr (argv[c], 'C') ) PP->problem = SSPC_INNERPRODUCT;
  else error ("i, I, s, S, R, T or C command has to be specified", EXIT);
  if ( strchr (argv[c], '#') ) PP->problem |= SSPC_COUNT;
  if ( strchr (argv[c], 'N') ) PP->problem |= PROBLEM_NORMALIZE;
  if ( strchr (argv[c], 'D') ) PP->problem |= SSPC_UNIFY;
  if ( strchr (argv[c], 'n') ) PP->problem |= SSPC_NO_NEIB;
  if ( strchr (argv[c], 'Y') ) PP->problem |= SSPC_POLISH;
  if ( !strchr (argv[c], 't') ) TT->flag |= LOAD_TPOSE;
  c++;
  
  while ( argv[c][0] == '-' ){
    switch (argv[c][1]){
      case 'K': II->topk.end = atoi(argv[c+1]);
      break; case 'L': TT->row_lb = atoi(argv[c+1]); 
      break; case 'U': TT->row_ub = atoi(argv[c+1]);
      break; case 'l': TT->w_lb = atof(argv[c+1]);
      break; case 'u': TT->w_ub = atof(argv[c+1]);
      break; case 'w': PP->TT.wfname = argv[c+1];
      break; case 'c': PP->dir = 1; TT->sep = atoi(argv[c+1]);
      break; case '2': PP->TT.fname2 = argv[c+1];
      break; case 'b': PP->II.len_lb = atoi(argv[c+1]);
      break; case 'B': PP->II.len_ub = atoi(argv[c+1]);
      break; case 'T': PP->th = atoi(argv[c+1]);
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
  if ( II->topk.end==0 ) II->frq_lb = atof(argv[c+1]);
  if ( argc>c+2 ) PP->output_fname = argv[c+2];
}

void SSPC_output (PROBLEM *PP, QUEUE_INT *cnt, QUEUE_INT i){
  QUEUE_INT *x;
  if ( PP->problem & SSPC_POLISH ){
    MQUE_FLOOP (PP->OQ[i], x) PP->vecchr[*x] = 1;
  } else if ( PP->problem & SSPC_COUNT ) (*cnt)++;
  else ITEMSET_output_itemset (&PP->II, NULL, 0);
}

/*************************************************************************/
/* SSPC main routine */
/*************************************************************************/
void SSPC (PROBLEM *PP){
  ITEMSET *II = &PP->II;
  TRSACT *TT = &PP->TT;
  QUEUE J = INIT_QUEUE;
  QUEUE_ID i, j, begin = (PP->problem&SSPC_POLISH)?0:(PP->dir>0?TT->sep:1), f, ii=0, m, t;
  QUEUE_INT *x, **o = NULL, *oi, *oj, cnt, id;
  WEIGHT *w, f1, f2, c, cc, *y;
  char *u = NULL, *mark = NULL;
  int pf = TT->flag&TRSACT_NEGATIVE;
  double sq =0;
  int count=0, fs=SSPC_INTERSECTION +SSPC_RESEMBLANCE +SSPC_INNERPRODUCT +SSPC_MAXINT +SSPC_MININT + SSPC_PMI;

    // initialization
  calloc2 (w, TT->T.clms*2, EXIT);
  if ( PP->problem & SSPC_NO_NEIB ) calloc2 (mark, TT->T.clms, EXIT);
  if ( PP->problem & SSPC_INNERPRODUCT ) FLOOP (i, 0, TT->T.clms) TT->w[i] *= TT->w[i];
  TRSACT_delivery (TT, &TT->jump, w, w+TT->T.clms, NULL, TT->T.clms);
//  FLOOP (i, 0, PP->dir?TT->sep:1) TT->OQ[i].end = 0;
  FLOOP (i, 0, TT->T.clms) TT->OQ[i].end = 0;
  II->itemset.t = 2;

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

  if (PP->problem & SSPC_UNIFY){
    FLOOP (i, 0, TT->T.t){ // first entry is ID
      id = TT->T.v[i].v[0];
      FLOOP (ii, 0, TT->T.v[i].t-1) TT->T.v[i].v[ii] = TT->T.v[i].v[ii+1];
      TT->T.v[i].t--;
      TT->T.v[i].v[TT->T.v[i].t] = id;
    }
    QUEUE_alloc (&J, TT->T.clms);
    calloc2 (u, TT->T.clms, EXIT);
    ARY_FILL (w, 0, TT->T.clms*2, 0);   // weight is to be re-computed
    id = TT->T.t; J.t = J.s = 0;
    FLOOP (i, 0, TT->T.t){
      if ( id != TT->T.v[i].v[TT->T.v[i].t] ){
        id = TT->T.v[i].v[TT->T.v[i].t];
        MQUE_FLOOP (J, x) u[*x] = 0;
        J.t = J.s = 0;
      }

      MQUE_FLOOP (TT->T.v[i], x){
        if ( u[*x] == 0 ){ QUE_INS (J, *x); u[*x] = 1; }
        else continue;
        w[*x] += TT->w[t]; w[*x+TT->T.clms] += TT->pw[t];
      }
    }
    MQUE_FLOOP (J, x){ u[*x] = 0; }
    J.t = J.s = 0;
  }

    // main loop
  FLOOP (i, begin, TT->T.clms){
//printf ("## %d\n", i);
    if ( II->flag & SHOW_PROGRESS ){
      if ( count < i*100/TT->T.clms ){ count++; fprintf (stderr, "%d%%\n", count); }
    }
    cnt = 0;
    II->itemset.v[0] = ((PP->problem&PROBLEM_NORMALIZE)&& PP->dir>0)? i-TT->sep: i;
    if ( PP->problem & SSPC_INNERPRODUCT ) sq = sqrt (w[i]);

      // delivery    
    if ( PP->problem & SSPC_POLISH ) m = PP->TT.T.clms;
    else m = (PP->dir>0)?TT->sep:i;
    TT->jump.t = TT->jump.s;
    if (PP->problem & SSPC_UNIFY) id = TT->T.v[t].v[TT->T.v[TT->OQ[i].s].t];
    FLOOP (ii, TT->OQ[i].s, TT->OQ[i].t){
      t = TT->OQ[i].v[ii];
      if (PP->problem & SSPC_NO_NEIB){ mark[t] = 1; } // for no_neib
      if ( TT->T.w ) y = TT->T.w[t]; else y = 0;
      if (PP->problem & SSPC_UNIFY){ // for unify
        if ( id != TT->T.v[t].v[TT->T.v[t].t] ){
          id = TT->T.v[t].v[TT->T.v[t].t];
          if ( t != TT->OQ[i].s ){
            MQUE_FLOOP (J, x) u[*x] = 0;
            J.t = J.s = 0;
          }
        }
      }  // unify end

      MQUE_MLOOP (TT->T.v[t], x, m){
        if ( TT->OQ[*x].end == 0 ){
          QUE_INS (TT->jump, *x);
          PP->occ_w[*x] = 0;
          if ( pf ) PP->occ_pw[*x] = 0;
        }
        if (PP->problem & SSPC_UNIFY){
          if ( u[*x] == 0 ){ QUE_INS (J, *x); u[*x] = 1; }
          else continue;
        }
        TT->OQ[*x].end++;
        if ( y ){
          PP->occ_w[*x] += *y; if ( *y>0 && pf) PP->occ_pw[*x] += *y;
          y++;
        } else {
          PP->occ_w[*x] += TT->w[t]; if ( pf ) PP->occ_pw[*x] += TT->pw[t];
        }
        if ( PP->problem & SSPC_POLISH ) QUE_INS (PP->OQ[*x], t);  // make occ for data polish
      }
    }

    if (PP->problem & SSPC_UNIFY){ // for unify
      MQUE_FLOOP (J, x) u[*x] = 0;
      J.t = J.s = 0;
    } // unify end

    MQUE_FLOOP (TT->jump, x){
      if ( *x == i ) goto SKIP;
      II->itemset.v[1] = *x;
      c = PP->occ_w[*x];

      if ( TT->flag & LOAD_SIZSORT ){
        for (oi=o[i],oj=o[*x] ; *oi<PP->II.len_lb ; oi++ ){
          while ( *oj < *oi ) oj++;
          if ( *oi == *oj ) c += TT->w[*oi];
        }
      }
      if (PP->problem & SSPC_NO_NEIB){  // for no_neib
        if ( mark[*x] ){ w[*x] -= TT->w[i]; w[i] -= TT->w[*x]; }
      }
      if ( c < PP->th ) goto SKIP;  // threshold for the intersection size 
      if ( PP->problem & fs ){
        if ( PP->problem & SSPC_INTERSECTION ) II->frq = c;
        else if ( PP->problem & SSPC_INNERPRODUCT ) II->frq = c / sq / sqrt(w[*x]);
        else if ( (PP->problem & SSPC_RESEMBLANCE) && (cc= w[i] +w[*x] -c) != 0 ) II->frq = c/cc;
        else if ( (PP->problem & SSPC_MAXINT) && (cc=MAX(w[i],w[*x])) != 0 ) II->frq = c/cc;
        else if ( (PP->problem & SSPC_MININT) && (cc=MIN(w[i],w[*x])) != 0 ) II->frq = c/cc;
        else if ( (PP->problem & SSPC_PMI) && (cc=w[i]*w[*x]) != 0 ) II->frq = log( ( c * TT->T.t) / cc ) /  -log ( c / TT->T.t);
        else continue;
        if ( II->frq >= II->frq_lb ) SSPC_output (PP, &cnt, *x);
      } else {
        f1 = w[i]*II->frq_lb; f2 = w[*x]*II->frq_lb;  // size of i and *x
        if ( PP->problem & SSPC_SIMILARITY ){
          f = ( (c >= f1) && (c >= f2) );
          II->frq = MIN(c/w[i], c/w[*x]);
        } else if ( PP->problem & SSPC_INCLUSION ){
          if ( c >= f2 ){
            II->frq = c/w[*x];
            II->itemset.v[0] = *x; II->itemset.v[1] = i-PP->root;
            SSPC_output (PP, &cnt, *x);
            II->itemset.v[0] = i-PP->root; II->itemset.v[1] = *x;
          }
          f = ( c >= f1 );
          II->frq = c/w[i];
        } else continue;
        if ( f ) SSPC_output (PP, &cnt, *x);
      }
      SKIP:;
      if (PP->problem & SSPC_NO_NEIB){  // for no_neib
        if ( mark[*x] ){ w[*x] += TT->w[i]; w[i] += TT->w[*x]; }
      }
      TT->OQ[*x].end = 0;
    }
    
    if ( PP->problem & SSPC_POLISH ){  // data polish;  clear OQ, and marks
      f = 0;
      MQUE_FLOOP (PP->TT.OQ[i], x){
        II->frq = DOUBLEHUGE;
        if ( PP->vecchr[*x] == 1 ){
          FILE2_print_int (&PP->II.multi_fp[0], *x, f);
          f = PP->II.separator;

//        if ( PP->vecchr[*x] == 0 ){
//          II->itemset.v[0] = i; II->itemset.v[1] = *x;
//          ITEMSET_output_itemset (&PP->II, NULL, 0);
//          QUEUE_print__ (&PP->TT.OQ[i]);
//          QUEUE_print__ (&PP->TT.T.v[*x]);
        }
        PP->vecchr[*x] = 0;
      }
      FILE2_putc (&PP->II.multi_fp[0], '\n');
      FILE2_flush (&PP->II.multi_fp[0]);
      MQUE_FLOOP (TT->jump, x) PP->OQ[*x].t = 0;
    }
    
    if (PP->problem & SSPC_NO_NEIB)  // for no_neib
        FLOOP (ii, TT->OQ[i].s, TT->OQ[i].t){ mark[TT->OQ[i].v[ii]] = 0; }
    TT->OQ[i].end = 0;
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
  if ( TT->flag & LOAD_SIZSORT ){
    FLOOP (i, 0, TT->T.clms){
      TT->OQ[i].t += TT->OQ[i].v - o[i];
      TT->OQ[i].v = o[i];
    }
  }
  mfree (w, o, u, mark);
  QUEUE_end (&J);
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
  int fs = SSPC_INTERSECTION +SSPC_RESEMBLANCE +SSPC_INNERPRODUCT+SSPC_MAXINT+SSPC_MININT;

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

  PP.TT.flag |= LOAD_INCSORT + TRSACT_ALLOC_OCC;
  if ( PP.II.len_ub<INTHUGE || PP.II.len_lb>0 ) PP.TT.flag |= LOAD_SIZSORT+LOAD_DECROWSORT;
  PROBLEM_load (&PP);
  if ( PP.II.len_ub < INTHUGE ){
    FLOOP (i, 0, PP.TT.T.t) if ( PP.TT.T.v[i].t <= PP.II.len_ub ){ PP.II.len_lb = i; break; }
  }
  PROBLEM_alloc (&PP, T->clms, T->t, 0, PP.TT.perm, PROBLEM_OCC_W +PROBLEM_OCC_T +PROBLEM_VECCHR);
  PP.TT.perm = NULL;
  realloc2 (PP.TT.w, MAX(T->t, T->clms)+1, EXIT);
  ARY_FILL (PP.TT.w, 0, MAX(T->t, T->clms)+1, 1);
  
//  if ( PP.problem & SSPC_PMI ){ PP.II.frq_lb = exp (PP.II.frq_lb) / T->t; } // shift the threshold

// delivery
//TRSACT_print (&PP.TT, NULL, PP.II.perm);
  print_mes (&PP.TT, "separated at %d\n", PP.TT.sep);
  QUEUE_delivery (PP.TT.OQ, NULL, NULL, T->v, &PP.TT.OQ[T->clms], T->t, T->clms);
  if ( PP.problem & SSPC_POLISH ){
    FLOOP (i, 0, PP.TT.T.clms) PP.occ_t[i] = PP.TT.OQ[i].t;
    MQUE_ALLOC (PP.OQ, PP.TT.T.clms, PP.occ_t, PP.TT.occ_unit, 1, EXIT);
  }

  if ( !ERROR_MES && PP.TT.T.clms>1 ){
    if ( PP.problem & SSPC_MATRIX ){ SSPCmat (&PP); }
    else SSPC (&PP);
    print_mes (&PP.TT, LONGF " pairs are found\n", PP.II.sc[2]);
  }

  ITEMSET_merge_counters (&PP.II);
  internal_params.l1 = PP.II.solutions;

//  PROBLEM_end (&PP);
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
