/* frequent appearing item sequence enumeration algorithm based on LCM */
/* 2004/4/10 Takeaki Uno   e-mail:uno@nii.jp, 
    homepage:   http://research.nii.ac.jp/~uno/index.html  */
/* This program is available for only academic use, basically.
   Anyone can modify this program, but he/she has to write down 
    the change of the modification on the top of the source code.
   Neither contact nor appointment to Takeaki Uno is needed.
   If one wants to re-distribute this code, do not forget to 
    refer the newest code, and show the link to homepage of 
    Takeaki Uno, to notify the news about the codes for the users.
   For the commercial use, please make a contact to Takeaki Uno. */



#ifndef _lcm_seq_c_
#define _lcm_seq_c_

#define WEIGHT_DOUBLE

#include"trsact.c"
#include"problem_zero.c"

#define LCMSEQ_LEFTMOST 134217728
#define LCMSEQ_SET_RULE 268435456

typedef struct {
  QUEUE_INT t;   // transaction ID
  QUEUE_INT s;   // previous position
  QUEUE_INT org;  // original position
} LCMSEQ_ELM;

typedef struct {
  unsigned char type;  // type of the structure
  LCMSEQ_ELM *v;  // pointer to the array
  QUEUE_ID end;  // the length of the array
  QUEUE_ID t;  // end position+1
  QUEUE_ID s;  // start position
} LCMSEQ_QUE;


void LCMseq_error (){
  ERROR_MES = "command explanation";
  print_err ("LCMseq: [FCfQIq] [options] input-filename support [output-filename]\n\
%%:show progress, _:no message, +:write solutions in append mode\n\
F:position occurrence, C:document occurrence\n\
m:output extension maximal patterns only, c:output extension closed patterns only\n \
f,Q:output frequency following/preceding to each output sequence\n\
A:output coverages for positive/negative transactions\n\
I(J):output ID's of transactions including each pattern, if J is given, an occurrence is written in a complete stype; transaction ID, starting position and ending position\n\
i:do not output itemset to the output file (only rules)\n\
s:output confidence and item frequency by absolute values\n\
t:transpose the input database (item i will be i-th transaction, and i-th transaction will be item i)\n\
[options]\n\
-K [num]: output [num] most frequent sequences\n\
-l,-u [num]: output sequences with size at least/most [num]\n\
-U [num]: upper bound for support(maximum support)\n\
-g [num]: restrict gap length of each consequtive items by [num]\n\
-G [num]: restrict window size of the occurrence by [num]\n\
-w [filename]:read weights of transactions from the file\n\
-i [num]: find association rule for item [num]\n\
-a,-A [ratio]: find association rules of confidence at least/most [ratio]\n\
-r,-R [ratio]: find association rules of relational confidence at least/most [ratio]\n\
-f,-F [ratio]: output sequences with frequency no less/greater than [ratio] times the frequency given by the product of appearance probability of each item\n\
-p,-P [num]: output sequence only if (frequency)/(abusolute frequency) is no less/no greater than [num]\n\
-n,-N [num]: output sequence only if its negative frequency is no less/no greater than [num] (negative frequency is the sum of weights of transactions having negative weights)\n\
-o,-O [num]: output sequence only if its positive frequency is no less/no greater than [num] (positive frequency is the sum of weights of transactions having positive weights)\n\
-s [num]: output itemset rule (of the form (a,b,c) => (d,e)) with confidence at least [num] (only those whose frequency of the result is no less than the support)\n\
-# [num]: stop after outputting [num] solutions\n\
-, [char]:give the separator of the numbers in the output\n\
-Q [filename]:replace the output numbers according to the permutation table given by [filename]\n\
# the 1st letter of input-filename cannot be '-'.\n\
# if the output file name is -, the solutions will be output to standard output.\n");
  EXIT;
}

/***********************************************************************/
/*  read parameters given by command line  */
/***********************************************************************/
void LCMseq_read_param (int argc, char *argv[], PROBLEM *PP){
  ITEMSET *II = &PP->II;
  int c=1, f=0;
  if ( argc < c+3 ){ LCMseq_error (); return; }
  
  if ( strchr (argv[c], 'C') ){ PP->problem |= PROBLEM_CLOSED+LCMSEQ_LEFTMOST; II->flag |= ITEMSET_RM_DUP_TRSACT;
  } else if (strchr( argv[c], 'F') ){ PP->problem |= PROBLEM_FREQSET;
  } else error ("F or C command has to be specified", EXIT);

  if ( !strchr (argv[c], '_') ){ II->flag |= SHOW_MESSAGE; PP->TT.flag |= SHOW_MESSAGE; }
  if ( strchr (argv[c], '%') ) II->flag |= SHOW_PROGRESS;
  if ( strchr (argv[c], '+') ) II->flag |= ITEMSET_APPEND;
  if ( strchr (argv[c], 'f') ) II->flag |= ITEMSET_FREQ;
  if ( strchr (argv[c], 'A') ) II->flag |= ITEMSET_OUTPUT_POSINEGA;
  if ( strchr (argv[c], 'R') ){ PP->problem |= ITEMSET_POSI_RATIO; II->flag |= ITEMSET_IGNORE_BOUND; }
  if ( strchr (argv[c], 'Q') ) II->flag |= ITEMSET_PRE_FREQ;
  if ( strchr (argv[c], 'I') || strchr (argv[c], 'J') ){
    II->flag |= ITEMSET_TRSACT_ID; // single occurrence
    if ( PP->problem & PROBLEM_FREQSET ) II->flag |= ITEMSET_MULTI_OCC_PRINT; // output pair
    if ( strchr (argv[c], 'J') ){
      II->flag -= ITEMSET_TRSACT_ID; // for outputting tuple
      II->flag |= ITEMSET_MULTI_OCC_PRINT;
    }
  }
  if ( strchr (argv[c], 'i') ) II->flag |= ITEMSET_NOT_ITEMSET;
  if ( strchr (argv[c], 's') ) II->flag |= ITEMSET_RULE_SUPP;
  if ( strchr (argv[c], 't') ) PP->TT.flag |= LOAD_TPOSE;
  if ( strchr (argv[c], 'm') ) PP->problem |= PROBLEM_EX_MAXIMAL;
  if ( strchr (argv[c], 'c') ) PP->problem |= PROBLEM_EX_CLOSED;
  c++;
  
  while ( argv[c][0] == '-' ){
    switch (argv[c][1]){
      case 'K': II->topk.end = atoi (argv[c+1]);
      break; case 'l': II->lb = atoi (argv[c+1]);
      break; case 'u': II->ub = atoi(argv[c+1]);
      break; case 'U': II->frq_ub = (WEIGHT)atof(argv[c+1]);
      break; case 'g': II->gap_ub = atoi(argv[c+1]);
      break; case 'G': II->len_ub = atoi(argv[c+1]);
      break; case 'w': PP->TT.wfname = argv[c+1];
      break; case 'f': II->prob_lb = atof(argv[c+1]); II->flag |= ITEMSET_RFRQ; f++;
      break; case 'F': II->prob_ub = atof(argv[c+1]); II->flag |= ITEMSET_RINFRQ; f++;
      break; case 'i': II->target = atoi(argv[c+1]);
      break; case 'a': II->ratio_lb = atof(argv[c+1]); II->flag |= ITEMSET_RULE_FRQ; f|=1;
      break; case 'A': II->ratio_ub = atof(argv[c+1]); II->flag |= ITEMSET_RULE_INFRQ; f|=1;
      break; case 'r': II->ratio_lb = atof(argv[c+1]); II->flag |= ITEMSET_RULE_RFRQ; f|=2;
      break; case 'R': II->ratio_ub = atof(argv[c+1]); II->flag |= ITEMSET_RULE_RINFRQ; f|=2;
      break; case 'P': II->flag |= ITEMSET_POSI_RATIO; II->flag |= ITEMSET_IGNORE_BOUND; II->rposi_ub = atof(argv[c+1]); f|=4;
      break; case 'p': II->flag |= ITEMSET_POSI_RATIO; II->flag |= ITEMSET_IGNORE_BOUND; II->rposi_lb = atof(argv[c+1]); f|=4;
      break; case 'n': II->nega_lb = atof(argv[c+1]);
      break; case 'N': II->nega_ub = atof(argv[c+1]);
      break; case 'o': II->posi_lb = atof(argv[c+1]);
      break; case 'O': II->posi_ub = atof(argv[c+1]);
      break; case 's': II->setrule_lb = atof(argv[c+1]); II->flag |= ITEMSET_SET_RULE;
      break; case '#': II->max_solutions = atoi(argv[c+1]);
      break; case ',': II->separator = argv[c+1][0];
      break; case 'Q': PP->outperm_fname = argv[c+1];
      break; default: goto NEXT;
    }
    c += 2;
    if ( argc < c+2 ){ LCMseq_error (); return; }
  }

  NEXT:;
  if ( (f&3)==3 || (f&5)==5 || (f&6)==6 )
      error ("-f, -F, -a, -A, -p, -P, -r and -R can not specified simultaneously", EXIT);
  if ( f ) BITRM (II->flag, ITEMSET_PRE_FREQ);

  if ( II->len_ub<INTHUGE || II->gap_ub<INTHUGE ) BITRM (PP->problem, LCMSEQ_LEFTMOST);
  PP->TT.fname = argv[c];
  if ( II->topk.end==0 ) II->frq_lb = (WEIGHT)atof(argv[c+1]);
  if ( argc>c+2 ) PP->output_fname = argv[c+2];
}



/*******************************************************/
/* compute occurrences of all frequent items */
/* if flag!=NULL, construct each OQ[i] (derivery), o.w., compute frequency of i */
/*******************************************************/
void LCMseq_occ_delivery (PROBLEM *PP, LCMSEQ_QUE *occ, int flag){
  ITEMSET *II = &PP->II;
  TRSACT *TT = &PP->TT;
  QUEUE_ID j;
  QUEUE_INT e, m;
  WEIGHT w;
  LCMSEQ_ELM *u, *uu, *u_end = occ->v + (occ->t-1);
  int f = TT->flag&TRSACT_NEGATIVE;
  int fl = (!(PP->problem&PROBLEM_CLOSED)&&!flag) || (!(PP->problem&LCMSEQ_LEFTMOST)&&flag);

  MQUE_FLOOP (*occ, u){  // loop for occurrences
    m = MAX (MAX(0, u->s -II->gap_ub), u->org -(II->len_ub-1));
    if ( u < u_end && u->t == (u+1)->t ) ENMAX (m, (u+1)->s);
    w = TT->w[u->t];
    if ( II->itemset.t == 0 ) m = 0;
    if ( !fl ) BLOOP (j, u->s, m) TT->sc[TT->T.v[u->t].v[j]] = 0; // clear marks of all letters in scanning part of the current transaction
    BLOOP (j, u->s, m){
      e = TT->T.v[u->t].v[j]; // e:= letter
      if ( fl || TT->sc[e] == 0 ){   // not leftmost, or the first appearance of the letter
        TT->sc[e] = 1; // mark the letter
        if ( flag ){  // if occurrence computing
          uu = &((LCMSEQ_ELM *)(TT->OQ[e].v))[TT->OQ[e].t]; // insert new element to occurrence
          uu->t = u->t;
          uu->s = j;
          uu->org = II->itemset.t? u->org: j;
          TT->OQ[e].t++;
        } else {  // for just frequency counting
          if ( TT->OQ[e].end == 0 ){ // initialize weights if this is the first insertion
            QUE_INS (PP->itemcand, e);
            PP->occ_w[e] = PP->occ_pw[e] = 0;
          }
          TT->OQ[e].end++;
          PP->occ_w[e] += w;
          if ( f && w>0 ) PP->occ_pw[e] += w;
        }
//        TT->sc[e] = !fl;
      }
    }
  }
}


/* remove infrequent items from jump, and set active/in-active marks */
void LCMseq_rm_infreq (PROBLEM *PP){
  ITEMSET *II = &PP->II;
  TRSACT *TT = &PP->TT;
  QUEUE_ID ii=PP->itemcand.s;
  QUEUE_INT *e;
  
  MQUE_FLOOP (PP->itemcand, e){
    if ( PP->occ_pw[*e] >= II->frq_lb ){
      PP->itemcand.v[ii++] = *e;
      TT->sc[*e] = 0;
    } else {
      TT->OQ[*e].t = TT->OQ[*e].end = 0;
      if ( PP->root ) TT->sc[*e] = 3;  // remove infrequent items only when gap constraint is not given
    }
  }
  PP->itemcand.t = ii;
}

/* remove merged occurrences from occ, and re-set temporary end-marks marked in each occurrence */
void LCMseq_reduce_occ (TRSACT *TT, LCMSEQ_QUE *occ, QUEUE_INT item){
  LCMSEQ_ELM *u, *uu=occ->v;
  MQUE_FLOOP (*occ, u){
    TT->T.v[u->t].v[u->s] = item;
    if ( TT->mark[u->t] == 0 ) continue;
    *uu = *u;
       // update positions in occ, for shrinked transactions
    if ( TT->mark[u->t] > 1 ){
      uu->t = TT->mark[u->t] -2;
      uu->org = TT->T.v[uu->t].t + u->org - u->s;  // actually, org is not used when database is shrinked
      uu->s = TT->T.v[uu->t].t;
    }
    uu++;
  }
  occ->t = (VEC_ID)(uu - occ->v);
}

/***************************************************************/
/* iteration of LCMseq */
/* INPUT: occurrences of current sequence */
/*************************************************************************/
void LCMseq (PROBLEM *PP, QUEUE_INT item, LCMSEQ_QUE *occ){
  ITEMSET *II = &PP->II;
  TRSACT *TT = &PP->TT;
  QUEUE_ID js=PP->itemcand.s, i, j;
  VEC_ID new_t = TT->new_t;
  int bnum = TT->buf.num, bblock = TT->buf.block_num;
  int output_flag = 1;
  QUEUE_INT *x, cnt=0, tt=TT->rows_org;
  WEIGHT *w=NULL, *pw=NULL;
  double prob = II->prob;
  LCMSEQ_ELM *u, L;
  QUEUE *Q = NULL;

//QUEUE_print__ (&II->itemset);
//  MQUE_FLOOP (*occ, u){ printf ("(%d, %d, %d) ", u->t, u->s, u->org); } printf ("\n");

// re-computing frequency, for (document occurrence & non-leftmost)
  if ( (PP->problem & PROBLEM_CLOSED) && !(PP->problem & LCMSEQ_LEFTMOST)){
    II->frq = II->pfrq = 0;
    MQUE_FLOOP (*occ, u){
      if ( u->t != tt ){
        II->frq += TT->w[u->t];
        if ( TT->w[u->t] > 0 ) II->pfrq += TT->w[u->t];
      }
      tt = u->t;
    }
  }

  PP->itemcand.s = PP->itemcand.t; // initilization for the re-use of queue
  II->iters++;
  if ( PP->problem & PROBLEM_EX_CLOSED ) PP->th = II->frq;  // threshold value for for ex_maximal/ex_closed check; in the case of maximal, it is always II->frq_lb
  if ( II->flag&ITEMSET_POSI_RATIO && II->pfrq!=0 ) II->frq /= (II->pfrq+II->pfrq-II->frq);

    // if the itemset is empty, set frq to the original #trsactions, and compute item_frq's
  LCMseq_occ_delivery (PP, occ, 0);
  if ( II->itemset.t == 0 ){
    if ( (II->frq = TT->total_w_org) != 0 )
        FLOOP (i, 0, TT->T.clms) II->item_frq[i] = PP->occ_w[i]/TT->total_w_org;
  }

  II->prob = 1.0;
  MQUE_FLOOP (II->itemset, x) II->prob *= II->item_frq[*x];

    // extending maximality/closedness check
  if ( PP->problem & (PROBLEM_EX_MAXIMAL+PROBLEM_EX_CLOSED) )
      MQUE_FLOOP (PP->itemcand, x) if ( PP->occ_w[*x] >= PP->th ) output_flag = 0;

  if ( output_flag )
      ITEMSET_check_all_rule (II, PP->occ_w, (QUEUE *)occ, &PP->itemcand, TT->total_pw_org, 0);
  MQUE_FLOOP (PP->itemcand, x) if ( PP->occ_pw[*x] >= II->frq_lb ) cnt++;

  if ( cnt == 0 || II->itemset.t >= II->ub ) goto END;
///////     database reduction     ///////////
  if ( 0&& PP->dir && cnt>10 && occ->t>2 && II->itemset.t>0 ){
      // put end-mark to each occurrence transaction 
//printf ("AAA: "); MQUE_FLOOP (*occ, u){ printf ("(%d, %d, %d) ", u->t, u->s, u->org); } printf ("\n");
    Q = &TT->OQ[TT->T.clms];
    Q->s = Q->t = 0;
    MQUE_FLOOP (*occ, u){
      TT->T.v[u->t].v[u->s] = TT->T.clms;
      QUE_INS (*Q, u->t);
    }
//printf ("========\n");
//TRSACT_print (TT, Q, NULL);
//printf ("BBB: "); MQUE_FLOOP (*occ, u){ printf ("(%d, %d, %d) ", u->t, u->s, u->org); } printf ("\n");
    TRSACT_find_same (TT, Q, TT->T.clms);
    TRSACT_merge_trsact (TT, Q, TT->T.clms);
      // erase end-mark of each occurrence transaction, and remove unified occurrences
//MQUE_FLOOP (*occ, u){ printf ("(%d, %d, %d) ", u->t, u->s, u->org); } printf ("\n");
    LCMseq_reduce_occ (TT, occ, item);
TRSACT_print (TT, Q, NULL);
printf ("-------\n");
//MQUE_FLOOP (*occ, u){ printf ("(%d, %d, %d) ", u->t, u->s, u->org); } printf ("\n");
  }

/////////////     deliverly    /////////////
  LCMseq_occ_delivery (PP, occ, 1);
  LCMseq_rm_infreq (PP);
  cnt = QUEUE_LENGTH_ (PP->itemcand);
  QUEUE_occ_dup (&PP->itemcand, &Q, TT->OQ, &w, PP->occ_w, &pw, PP->occ_pw, sizeof(LCMSEQ_ELM));
  if ( Q == NULL ) goto END;
  MQUE_FLOOP (PP->itemcand, x) TT->OQ[*x].end = TT->OQ[*x].t = 0;
  PP->itemcand.t = PP->itemcand.s;

/************ recursive calls ***************/
  FLOOP (i, 0, cnt){
    II->frq = w[i];
    II->pfrq = pw[i];
    if ( II->flag & ITEMSET_SET_RULE ){
      II->set_weight[II->itemset.t] = II->frq;
      II->set_occ[II->itemset.t] = &Q[i];
    }
    QUE_INS (II->itemset, Q[i].end);
//    prob2 = II->prob;
//    II->prob *= frqs[e+TT->item_max*2];
    if ( II->itemset.t == 0 ){    // reverse occurrence order for the first iteration (for the process of overlapping occurrences)
      u = (LCMSEQ_ELM *)TT->OQ[i].v;
      FLOOP (j, 0, TT->OQ[i].t/2){
        L = u[j]; u[j] = u[TT->OQ[i].t-j-1]; u[TT->OQ[i].t-j-1] = L;
      }
    }
    LCMseq (PP, Q[i].end, (LCMSEQ_QUE *)&Q[i]);
//    II->prob = prob2;
    II->itemset.t--;
  }
  free2 (Q);
  TT->new_t = new_t;
  TT->buf.num = bnum, TT->buf.block_num = bblock;

  END:;
  MQUE_FLOOP (PP->itemcand, x) TT->OQ[*x].end = TT->OQ[*x].t = 0;
  PP->itemcand.t = PP->itemcand.s;
  PP->itemcand.s = js;
  II->prob = prob;
}

/*************************************************************************/
/* initialization of LCMseq ver. 2 */
/*************************************************************************/
void LCMseq_init (PROBLEM *PP, LCMSEQ_QUE *occ){
  ITEMSET *II = &PP->II;
  TRSACT *TT = &PP->TT;
  VEC_ID i;
  QUEUE_ID j;
  QUEUE_INT *x;
  LCMSEQ_ELM L;

  II->X = TT;
  II->frq = TT->total_w_org; II->pfrq = TT->total_pw_org;
  II->flag |= ITEMSET_USE_ORG +ITEMSET_ITEMFRQ;
  PROBLEM_alloc (PP, TT->T.clms, TT->T.t, TT->row_max, TT->perm, PROBLEM_ITEMCAND +((TT->flag&TRSACT_NEGATIVE)?PROBLEM_OCC_PW: PROBLEM_OCC_W));
  malloc2 (occ->v, TT->T.t, EXIT);
  occ->end = TT->clm_max; occ->s = occ->t = 0;
  TT->perm = NULL;
  if ( II->perm && RANGE(0, II->target, II->item_max) ) II->target = II->perm[II->target];
  
  if ( !(TT->sc) ) calloc2 (TT->sc, TT->T.clms+2, return);
  free2 (II->itemflag); II->itemflag = TT->sc;   // II->itemflag and TT->sc shares the same memory
//  TT->occ_unit = sizeof(QUEUE_INT)*3;  // one occ is composed of 3 intgers

    // make occurrence & reverse each transaction
  FLOOP (i, 0, TT->T.t){
    FLOOP (j, 0, TT->T.v[i].t/2)
        SWAP_QUEUE_INT (TT->T.v[i].v[j], TT->T.v[i].v[TT->T.v[i].t-1-j]);
  }
  if ( II->len_ub >= INTHUGE ) II->len_ub = TT->row_max;
  if ( II->gap_ub >= INTHUGE ) II->gap_ub = TT->row_max;
  II->total_weight = TT->total_w_org;
  i=0;MQUE_FLOOP (TT->OQ[TT->T.clms], x){
    L.t = *x;
    L.s = L.org = TT->T.v[*x].t;   // !! org is originally -1
    QUE_INS (*occ, L);
  }
  PP->dir = (PP->problem&LCMSEQ_LEFTMOST) && II->len_ub>=TT->row_max && !(II->flag&(ITEMSET_TRSACT_ID+ITEMSET_MULTI_OCC_PRINT));   // flag for shrink or not
  PP->root = II->gap_ub>=TT->row_max && II->len_ub>=TT->row_max;   // flag for removing infrequent item or not
  PP->th = II->frq_lb;
}

/*************************************************************************/
/* main of LCMseq ver. 2 */
/*************************************************************************/
int LCMseq_main (int argc, char *argv[]){
  PROBLEM PP;
  ITEMSET *II = &PP.II;
  TRSACT *TT = &PP.TT;
  LCMSEQ_QUE occ;
  occ.v = NULL;
  ERROR_MES = NULL;
  PROBLEM_init (&PP);
  LCMseq_read_param (argc, argv, &PP);
if ( ERROR_MES ) return (1);

  TT->occ_unit = sizeof(LCMSEQ_ELM);
  TT->flag |= TRSACT_MAKE_NEW +TRSACT_ALLOC_OCC + ((II->flag&(ITEMSET_TRSACT_ID+ITEMSET_MULTI_OCC_PRINT))?0: (TRSACT_SHRINK+TRSACT_1ST_SHRINK)) ;
  TT->w_lb = (((II->flag&(ITEMSET_TRSACT_ID+ITEMSET_MULTI_OCC_PRINT)) && (PP.problem & PROBLEM_FREQSET)) || (II->flag&ITEMSET_RULE) || II->gap_ub<INTHUGE || II->len_ub<INTHUGE )? -WEIGHTHUGE: II->frq_lb;
  PROBLEM_load (&PP);
  
  if ( !ERROR_MES ){
    LCMseq_init (&PP, &occ);
    if ( !ERROR_MES ){
      LCMseq (&PP, TT->T.clms, &occ);
      ITEMSET_last_output (II);
    }
  }
  
  free2 (occ.v);
  TT->sc = NULL;
  PROBLEM_end (&PP);
  return (ERROR_MES?1:0);
}

/*******************************************************************************/
#ifndef _NO_MAIN_
#define _NO_MAIN_
int main (int argc, char *argv[]){
  return (LCMseq_main (argc, argv));
}
#endif
/*******************************************************************************/

#endif




