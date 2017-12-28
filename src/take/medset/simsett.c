/************************************************************************/
/* simstr.c: find frequently appearing string patterns from a string, by 
   collecting frequently appearing short substrings and extend it unless the 
   "voting" does not succeed (the mojority letter in a position of them is
   not determined) */
/* 17/Mar/2010 Takeaki Uno  */

#ifndef _simset_c_
#define _simset_c_

#ifdef _NO_MAIN_
#define _simset_c_no_main_
#else
#define _NO_MAIN_
#endif

#define WEIGHT_DOUBLE
#define USE_MATH

#include<string.h>
#include"fstar.c"
#include"trsact.c"
#include"sgraph.c"
#include"grhfil.c"
#include"sspc.c"
#include"mace.c"
#include"medset.c"



double SIMSET_th=0.0;    // threshold for data polishing
double SIMSET_th1=0.0;   // threshold for the transaction comparison phase
int SIMSET_thk=0;        // threshold for the k-best transaction comparison phase
double SIMSET_th2=0.0;   // threshold for clique unification
double SIMSET_th3=0.0;   // threshold for taking clique from clique connected component (connected by similarity measure)
double SIMSET_th4=0.0;   // threshold for unification of records
UNIONFIND_ID SIMSET_nodes = 0;  // number of nodes in the graph to be posihed

char SIMSET_sep=0;
int SIMSET_deg_ub=INTHUGE, SIMSET_deg_lb=0;   // ignore vertices of degree more/less than this, in data polishing
int SIMSET_item_ub=INTHUGE, SIMSET_item_lb=0;   // ignore items of frequency more/less than this, in transaction comparison phase
int SIMSET_trsact_ub=INTHUGE, SIMSET_trsact_lb=0;  // ignore transactions of size more/less than this, in transaction comparison phase
int SIMSET_lb;  // ignore clusters of size less than this

char *SIMSET_infname, *SIMSET_outfname, *SIMSET_outperm_fname = NULL;  // input/output/permutation/ file names
int SIMSET_no_remove=0;  // do not remove edges in "data polishing", if 1
int SIMSET_repeat=0;  // #repetition of "data polishing"
int SIMSET_intersection=0;      // take intersection of each cluster
int SIMSET_intgraph=0;      // take intersection of original graph and polished graph
int SIMSET_ignore=0, SIMSET_ignore2=0;      // ignore pairs with intersection size of less than this (first phase/second phase)
char SIMSET_tpose=' ';   // transpose the input? (only when -T option is given)
int SIMSET_mes=1;     // output messages, if 1
int SIMSET_append=0;  // append the output to the output file, if 1
int SIMSET_leave_tmp_files=0;  // do not delete the temporary files, if 1
char SIMSET_edge=' ', SIMSET_prog[10], *SIMSET_mes2 = "", *SIMSET_workdir="";    // tmp variables for commands
char SIMSET_com, SIMSET_com1=0, SIMSET_com2;  // similarity measure; for polishing, transaction comparison, and clique clustering
char SIMSET_rm_dup = 0; // do not remove duplicates when 1
int SIMSET_hist=0;       // output histogram? (frequency is output to each item)
double SIMSET_vote_th=0.5;  // threshold for taking the intersection of each cluster; items of frequency less than this will not be output
int SIMSET_ratio=0;   // output the original frequency of each item, in intersection mode
UNIONFIND_ID *SIMSET_unify_mark = 0, *SIMSET_unify_set = 0;  // mark for union finding (grouping)
char *SIMSET_itemweight = "";  // load itemweitht from ele-file if 1
char *SIMSET_itemweight_file = "";  // load itemweight from this file
char *SIMSET_f="";  // for spefifying f command
double SIMSET_multiply = 0, SIMSET_power = 0;



/* error routine */
void SIMSET_error (){
  ERROR_MES = "command explanation";
  print_err ("simset ISCMOt [options] similarity-graph-filename similarity-threshold degree-threshold output-filename\n\
%%:show progress, _:no message, +:write solutions in append mode, =:do not remove temporal files\n\
@:do not execute data polishing, E:read edge list file\n\
i:set similarity measure to the ratio of one is included in the other\n\
I:set similarity measure to the ratio of both are included in the other\n\
S:set similarity measure to |A\\cap B|/max(|A|,|B|)\n\
s:set similarity measure to |A\\cap B|/min(|A|,|B|)\n\
C:set similarity measure to the cosign distance, the inner product of the normalized characteristic vectors\n\
T:set similarity measure to the intersection size, i.e., |A\\cap B|\n\
R:(recemblance) set similarity measure to |A\\cap B|/|A\\cup B|\n\
P(PMI): set similarity measure to log (|A\\capB|*|all| / (|A|*|B|)) where |all| is the number of all items\n\
M:output intersection of each clique, instead of IDs of its members\n\
v (with M): output ratio of records, including each item\n\
m:do not remove edges in the data polishing phase\n\
O:repeatedly similarity clustering until convergence\n\
Y:take intersection of original graph and polished graph, instead of clique mining\n\
1:do not remove the same items in a record (with -G)\n\
W:load weight of each element\n\
t:transpose the input database, so that each line will be considered as a record\n\
\n[options]\n\
-G [similarity] [threshold]:use [similarity] of [threshold] in the first phase (file is regarded as a transaction database)\n\
-k [threshold]:find only k-best for each record in -G option\n\
-M [num]:merge similar cliques of similarity in [num] of recemblance (changes to 'x' by giving '-Mx')\n\
-m [num]:take independently cliques from similar cliques of similarity in [num] of recemblance, and merge the neighbors of each independent clique ('recemblance' changes to 'x' by giving '-Mx')\n\
-v [num]:specify majority threshold (default=0.5)\n\
-u [num]:ignore vertices of degree more than [num]\n\
-l [num]:ignore vertices of degree less than [num]\n\
-U [num]:ignore transactions of size more than [num] (with -G)\n\
-L [num]:ignore transactions of size less than [num] (with -G)\n\
-I [num]:ignore items of frequency more than [num] (with -G)\n\
-i [num]:ignore items of frequency less than [num] (with -G)\n\
-T,t [num]:ignore pairs whose intersection size is less than [num] (T for first phase with -G option, and t for polishing)\n\
-O [num]:specify the number of repetitions\n\
-9 [num]:shrink records of similarity more than [num]\n\
-X [num]:multiply the weight by [num] (and trancate by 1, for C command)\n\
-x [num]:power the weight by [num] (and normalize, for C command)\n\
-w [filename]:load weight of elements from the file\n\
-W [dir]:specify the working directory (folder). The last letter of the directory has to be '/' ('\\')\n\
-, [char]:give the separator of the numbers in the output\n\
-Q [filename]:replace the output numbers according to the permutation table given by [filename]\n\
# the 1st letter of input-filename cannot be '-'.\n\
if similarity-threshold is 0, skip the similarity graph construction phase\n");
  EXIT;
}

/* read parameters given by command line */
void SIMSET_read_param (int argc, char *argv[]){
  int c=1;
  SIMSET_prog[0] = 0;

  if ( argc<c+5 ){ SIMSET_error (); return; }
  if ( strchr (argv[c], '_') ){ strcat (SIMSET_prog, "_"); SIMSET_mes = 0; SIMSET_mes2 = " -_";}
  if ( strchr (argv[c], '%') ) strcat (SIMSET_prog, "%");
  if ( strchr (argv[c], '+') ) SIMSET_append = 1;
  if ( strchr (argv[c], '=') ) SIMSET_leave_tmp_files = 1;
  if ( strchr (argv[c], '@') ) SIMSET_repeat = -1;

  if ( strchr (argv[1], 'I') ) SIMSET_com = 'I';
  else if ( strchr (argv[1], 'i') ) SIMSET_com = 'i';
  else if ( strchr (argv[1], 'C') ){ SIMSET_com = 'C'; SIMSET_f = "f"; }
  else if ( strchr (argv[1], 'T') ) SIMSET_com = 'T';
  else if ( strchr (argv[1], 'S') ) SIMSET_com = 'S';
  else if ( strchr (argv[1], 'R') ) SIMSET_com = 'R';
  else if ( strchr (argv[1], 's') ) SIMSET_com = 's';
  if ( strchr (argv[1], 'M') ) SIMSET_intersection = 1;
  if ( strchr (argv[1], 'm') ) SIMSET_no_remove = 1;
  if ( strchr (argv[1], 'O') ) SIMSET_repeat = 10000000;
  if ( strchr (argv[c], 't') ) SIMSET_tpose = 't';
  if ( strchr (argv[c], 'H') ) SIMSET_hist = 1;
  if ( strchr (argv[c], 'v') ) SIMSET_ratio = 1;
  if ( strchr (argv[c], 'E') ) SIMSET_edge = 'E';
  if ( strchr (argv[c], 'Y') ) SIMSET_intgraph = 1;
  if ( strchr (argv[c], 'W') ) SIMSET_itemweight = "w";
  if ( strchr (argv[c], '1') ) SIMSET_rm_dup = 1;
//  SIMSET_com1 = SIMSET_com;
  c++;
  

  while ( argv[c][0] == '-' ){
    if ( argc<c+5 ){ SIMSET_error (); return; }
    switch ( argv[c][1] ){
      case 'G': if ( !strchr ("IiCTSsR", argv[c+1][0]) )
          error_num("unknown similarity measure", 0, EXIT);
        if ( (SIMSET_th1 = atof(argv[c+2])) <= 0 )
          error_num("the majority threshold has to be positive", atof(argv[c+1]), EXIT);
        SIMSET_com1 = argv[c+1][0]; c++; 
      break; case 'v': if ( (SIMSET_vote_th = atof(argv[c+1])) <= 0 )
          error_num("the majority threshold has to be positive", atof(argv[c+1]), EXIT);
      break; case 'm': if ( (SIMSET_th3 = atof(argv[c+1])) <= 0 )
          error_num("the independent set threshold has to be positive", atof(argv[c+1]), EXIT);
          if ( argv[c][2] ) SIMSET_com2 = argv[c][2]; else SIMSET_com2 = 'R';
      break; case 'M': if ( (SIMSET_th2 = atof(argv[c+1])) <= 0 )
          error_num("the merge threshold has to be positive", atof(argv[c+1]), EXIT);
          if ( argv[c][2] ) SIMSET_com2 = argv[c][2]; else SIMSET_com2 = 'R';
      break; case '9': if ( (SIMSET_th4 = atof(argv[c+1])) <= 0 )
          error_num("the unification threshold has to be positive", atof(argv[c+1]), EXIT);
      break; case 'k': if ( (SIMSET_thk = atoi(argv[c+1])) <= 0 )
          error_num("the k-best threshold has to be positive", atof(argv[c+1]), EXIT);
      break; case 'u': SIMSET_deg_ub = atoi(argv[c+1]);
      break; case 'l': SIMSET_deg_lb = atoi(argv[c+1]);
      break; case 'U': SIMSET_trsact_ub = atoi(argv[c+1]);
      break; case 'L': SIMSET_trsact_lb = atoi(argv[c+1]);
      break; case 'I': SIMSET_item_ub = atoi(argv[c+1]);
      break; case 'i': SIMSET_item_lb = atoi(argv[c+1]);
      break; case 'T': SIMSET_ignore = atoi(argv[c+1]);
      break; case 't': SIMSET_ignore2 = atoi(argv[c+1]);
      break; case 'O': SIMSET_repeat = atoi(argv[c+1]);
      break; case 'X': if ( (SIMSET_multiply = atof(argv[c+1])) <= 0 )
          error_num("the factor has to be positive", atof(argv[c+1]), EXIT);
      break; case 'x': if ( (SIMSET_power = atof(argv[c+1])) == 0 )
          error_num("the factor has to be non zero", atof(argv[c+1]), EXIT);
      break; case 'w': SIMSET_itemweight_file = argv[c+1];
      break; case 'W': SIMSET_workdir = argv[c+1];
      break; case ',': SIMSET_sep = argv[c+1][0];
      break; case 'Q': SIMSET_outperm_fname = argv[c+1];
      break; default: goto NEXT;
   }
    c += 2;
  }

  NEXT:;
  SIMSET_infname = argv[c];    // input file name
  SIMSET_th = atof(argv[c+1]);  // similarity threshold
  SIMSET_lb = atoi(argv[c+2]);  // threshold for cluster size
  SIMSET_outfname = argv[c+3];  // output file name

  if ( (c = strlen(SIMSET_infname) + strlen(SIMSET_workdir)) > 800 )
      error_num ("too long filename/workdir", c, EXIT);
}


/*****************************************************************************/
/* unify the similar records, by deleting them except for one representative */
/*****************************************************************************/
void SIMSET_unify (char *fname, char *fname2, int flag){  // flag= 0:cluster, 1:graph, 2:for mace
  FSTAR FF = INIT_FSTAR; // graph
  char c;
  FILE2 fp;
  FSTAR_INT i, j, e, z;
  char *buf;
  LONG x, y;
  
  if ( flag ){
    if ( !SIMSET_unify_mark )
         UNIONFIND_init (&SIMSET_unify_mark, &SIMSET_unify_set, SIMSET_nodes);
    FILE2_open (fp, fname2, "r", EXIT);
    do {
      if ( FILE2_read_pair (&fp, &x, &y, NULL, 0) ) continue;
      UNIONFIND_unify_set (x, y, SIMSET_unify_mark, SIMSET_unify_set);
    } while ( !(FILE_err&2) );
    FILE2_close (&fp);
  }
  FF.fname = fname;
  FSTAR_load (&FF);
  FILE2_open (fp, fname, "w", EXIT);
  FLOOP (i, 0, FF.node_num){
    c = 0; z = 0;  buf = fp.buf;
    if ( flag==0 || SIMSET_unify_mark[i] == i ){
      FLOOP (j, FF.fstar[i], FF.fstar[i+1]){
        e = FF.edge[j];
        if ( SIMSET_unify_mark[e] != e ) continue;
        while (1){
          FILE2_print_int (&fp, e, c);
          if ( z>=100 ) FILE2_flush (&fp);
          c = ' '; z++;
          if ( flag == 2 || SIMSET_unify_set[e] == e ) break;
          e = SIMSET_unify_set[e];
        }
      }
    }
    if ( flag || z >= SIMSET_lb || z >= 100 ){
      FILE2_puts (&fp, "\n");
      FILE2_flush (&fp);
    } else fp.buf = buf;
  }
  FILE2_flush_last (&fp);
  FILE2_close (&fp);
  FSTAR_end (&FF);
}


/******************************************************************************/
/*   SIMSET main routine                                                      */
/******************************************************************************/
int SIMSET_main (int argc, char *argv[]){
  int flag=1, count=0, unify_flag = 0, break_flag = 0;
  size_t siz=0;
  char *W = SIMSET_workdir, s1[1024], s2[1024], s3[1024], s4[1024], s5[1024], s6[1024], s7[1024], s8[1024], s9[1024], sk[20], *s10 = " -d 0.001", *f1="__tmp_out0__", *f2="__tmp_out1__", *fn = f1;

    // read params
  SIMSET_read_param (argc, argv);
  if ( ERROR_MES ) return (1);  // read params, and exit if error

  sprintf (s1, "%s__tmp__", W);  // write commands to string variables
  sprintf (s2, "%s__tmp_out2__", W);
  s5[0] = s6[0] = 0; if ( SIMSET_th4 > 0 ){
    sprintf (s5, " -9 %f %s__tmp_unify__", SIMSET_th4, W); 
    sprintf (s6, "%s__tmp_unify__", W); 
  }
  FILE2_copy (SIMSET_infname, s1);  // copy the input file to temporary file
  s7[0] = 0; s8[0] = 0;
  if ( *SIMSET_itemweight_file ){
    sprintf (s7, " -W %s", SIMSET_itemweight_file);
    if ( SIMSET_com == 'C' ){ sprintf (s8, " -W %s__tmp_weight__", W); } // inpro
  }
  s9[0]=0; if ( SIMSET_multiply > 0 ) sprintf (s9, " -X %f", SIMSET_multiply);
  else if ( SIMSET_power != 0.0 ) sprintf (s9, "Z -x %f", SIMSET_multiply);
  if ( SIMSET_com != 'C' ) s10 = ""; // not inpro
  
  sk[0]=0; if ( SIMSET_thk ) sprintf (sk, " -k %d", SIMSET_thk);

      // transaction comparison phase: similarity graph construction for transaction database
  if ( !SIMSET_com1 ){
    EXECSUB (GRHFIL_main, SIMSET_mes, goto END,"grhfil %su09%c%s%s%s \"%s\" \"%s__tmp__\"",
      SIMSET_prog, SIMSET_edge=='E'?'e':' ', SIMSET_itemweight, s7, s8, SIMSET_infname, W);
    goto GRAPH_POLISHING;
  }
  
       // for giving -T option for SSPC
  s3[0] = 0; if ( SIMSET_ignore > 0 ) sprintf (s3, " -T %d", SIMSET_ignore);
       // find similar pairs
  if ( SIMSET_edge=='E' ){
    EXECSUB (GRHFIL_main, SIMSET_mes, goto END,"grhfil ed%s%s%s \"%s\" \"%s__tmp_edge__\"",
      SIMSET_itemweight, s7, s8, SIMSET_infname, W);
    sprintf (s4, "%s__tmp_edge__", W);
    SIMSET_infname = s4;
  }

      // similarity graph construction phase
  EXECSUB (SSPC_main, SIMSET_mes, goto END, "sspc %s%c%s%s%s%s%s%s -L %d -U %d -l %d -u %d \"%s\" %f \"%s__tmp_out__\"",
      SIMSET_prog, SIMSET_com1, SIMSET_rm_dup?"":"1", SIMSET_tpose=='t'?"t":"", s3, s5, s8, sk,
      SIMSET_item_lb, SIMSET_item_ub, SIMSET_trsact_lb, SIMSET_trsact_ub, SIMSET_infname, SIMSET_th1, W);
  unify_flag = 1; SIMSET_nodes = internal_params.l3;

      // convert edge type to list type
  EXECSUB (GRHFIL_main, SIMSET_mes, goto END,"grhfil %sue09%s%s -n " UNIONFIND_IDF " \"%s__tmp_out__\" \"%s__tmp__\"",
      SIMSET_prog, SIMSET_itemweight, s8,
      SIMSET_nodes, W, W);

       // phase 2: data polishing
  GRAPH_POLISHING:;
  if ( SIMSET_intgraph ){ 
    EXECSUB (GRHFIL_main, SIMSET_mes, goto END,"grhfil %su0 \"%s__tmp__\" \"%s__tmp_org__\"",
      SIMSET_prog, SIMSET_infname, W);
  }

  if ( SIMSET_repeat < 0 ) goto FIND_CLIQUE;   // clique enumeration without polishing
  s3[0] = 0; if ( SIMSET_ignore2 > 0 ) sprintf (s3, " -T %d", SIMSET_ignore2);
  do {  // data polishing loop
    count ++;
    if ( SIMSET_mes ) print_err ("%dth-iter\n", count);
    if ( SIMSET_th4 > 0 && unify_flag ) SIMSET_unify (s1, s6, 1);

        // neighbor similarity comparison
    EXECSUB (SSPC_main, SIMSET_mes, goto END, "sspc %s%c%s%s%s%s -l %d -u %d \"%s__tmp__\" %f \"%s%s\"",
        SIMSET_prog, SIMSET_com, SIMSET_f, s3, s5, s8,
         SIMSET_deg_lb, SIMSET_deg_ub, W, SIMSET_th, W, fn);
    unify_flag = 1; SIMSET_nodes = internal_params.l3;
    siz = internal_params.l1;
    if ( siz == 0 ){ print_err (" no similar pair exists"); break; }

      // check the convergence
    if ( count > 1 && SIMSET_com != 'C' ){
      EXECSUB (GRHFIL_main, SIMSET_mes, goto END, "grhfil de%s -dde%s \"%s__tmp_out0__\"%s \"%s__tmp_out1__\" \"%s__tmp_out__\"", 
        SIMSET_com=='C'?"w":"", SIMSET_com=='C'?"w":"", W, s10, W, W);
      if ( internal_params.l1 == 0 ) break_flag = 1;  // no difference
      fprintf (stderr, "#diff edges %lld\n", internal_params.l1);
    }

    if ( SIMSET_com == 'C' ){ sprintf (s8, " -W %s__tmp_weight__", W); } // inpro
    EXECSUB (GRHFIL_main, SIMSET_mes, goto END,"grhfil %s%sue09%s%s -n " UNIONFIND_IDF " \"%s%s\" \"%s__tmp__\"",
        SIMSET_com=='C'?"w":"", SIMSET_prog, s9, s8, SIMSET_nodes, W, fn, W);

      // recover the deleted edges
    if ( SIMSET_no_remove == 1 ){
      EXECSUB (GRHFIL_main, SIMSET_mes, goto END, "grhfil %sue -M %s \"%s__tmp__\" \"%s__tmp_out2__\"", SIMSET_prog, SIMSET_infname, W, W);
      rename (s2, s1);
//SIMSET_unify_check (s1, s6, 0);
    }
    if ( break_flag || !SIMSET_repeat || SIMSET_repeat == count || count>=5000 ) break;      // repeat until maximi iterations
    fn = fn == f1? f2: f1; // alternate the result filename of sspc

  } while (1);    //  repeat at most 50 times

  if ( SIMSET_com == 'C' ){ // not inpro
    EXECSUB (GRHFIL_main, SIMSET_mes, goto END,"grhfil %su -1 0.5 -w \"%s__tmp_weight__\" \"%s__tmp__\" \"%s__tmp_out2__\"",
      SIMSET_prog, W, W, W);
    sprintf (s8, "%s__tmp_out2__", W);
    sprintf (s7, "%s__tmp__", W);
goto END;
    unlink (s7);
    rename (s8, s7);
  }

  if ( SIMSET_intgraph ){ 
    EXECSUB (GRHFIL_main, SIMSET_mes, goto END,"grhfil %su0 -m \"%s__tmp__\" \"%s\"",
      SIMSET_prog, W, SIMSET_outfname);
    goto END;
  }
      // phase3: clinue enumeration
  FIND_CLIQUE:;
  if ( SIMSET_th4 > 0 && unify_flag ) SIMSET_unify (s1, s6, 2);
  EXECSUB (MACE_main, SIMSET_mes, goto END, "mace %sM -l %d \"%s__tmp__\" \"%s__tmp_out2__\"",
     SIMSET_prog, SIMSET_th4>0?1:SIMSET_lb, W, W);
  if ( SIMSET_th4 > 0 && SIMSET_unify_mark ){ SIMSET_unify (s2, NULL, 0);}

      // phase4: merge similar cliques
  if ( SIMSET_th2 > 0.0 || SIMSET_th3 > 0.0 ){
    EXECSUB (SSPC_main, SIMSET_mes, goto END, "sspc %s%c \"%s__tmp_out2__\" %f \"%s__tmp2__\"",
       SIMSET_prog, SIMSET_com2, W, SIMSET_th2+SIMSET_th3, W);
    EXECSUB (MEDSET_main, SIMSET_mes, goto END, "medset%s%s -l %d -%c \"%s__tmp2__\" \"%s__tmp_out2__\" 1 \"%s__tmp2__\"",
          SIMSET_mes2, SIMSET_tpose=='t'?" -t":"", SIMSET_lb, SIMSET_th2>0.0? 'T': 'I', W, W, W);
    sprintf (common_comm, "%s__tmp2__", W);
    rename (common_comm, s2);
  }

      // phase5: take intersection
  if ( SIMSET_intersection ){
    EXECSUB (MEDSET_main, SIMSET_mes, goto END, "medset%s %s%s-l %d %s\"%s__tmp_out2__\" \"%s\" %f \"%s\"",
        SIMSET_mes2, SIMSET_tpose=='t'?"-t ":"", SIMSET_ratio?"-i ": "", SIMSET_lb,
         SIMSET_hist?"-H ":"", W, SIMSET_infname, SIMSET_vote_th, SIMSET_outfname);
  } else rename (s2, SIMSET_outfname);
  flag = 0;

///////////////////////////////////////////////////////////
  END:;
  if ( !SIMSET_leave_tmp_files ) MREMOV (W, "__tmp__", "__tmp2__", "__tmp_out0__", "__tmp_out1__", "__tmp_out2__", "__tmp_weight__", "__tmp_edge__", "__tmp_org__", "__tmp_unify__");
  mfree (SIMSET_unify_mark, SIMSET_unify_set);
  return (flag);
}

/*******************************************************************************/
#ifndef _simset_c_no_main_
int main (int argc, char *argv[]){
  return (SIMSET_main (argc, argv) );
}
#endif
/*******************************************************************************/

#endif




