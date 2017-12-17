/*
    array-based simple heap (fixed size)
            12/Apr/2001   by Takeaki Uno  e-mail:uno@nii.jp, 
    homepage:   http://research.nii.ac.jp/~uno/index.html  */
/* This program is available for only academic use, basically.
   Anyone can modify this program, but he/she has to write down 
    the change of the modification on the top of the source code.
   Neither contact nor appointment to Takeaki Uno is needed.
   If one wants to re-distribute this code, please
    refer the newest code, and show the link to homepage of 
    Takeaki Uno, to notify the news about the codes for the users. */

/* bench mark
  PentiumIII 500MHz, Memory 256MB Linux
  values 0-1,000,000 : set & del & get  1,000,000 times
  2.55sec

  # rotation  == 1/5 per 1 set/del

   *** simple array *** 
   value 0-1,000,000 set & set & set   1,000,000 times,
   0.88sec 
  */

#ifndef _aheap_h_
#define _aheap_h_

#include"stdlib2.h"

#ifndef AHEAP_KEY
 #ifdef AHEAP_KEY_DOUBLE
  #define AHEAP_KEY  double
  #define AHEAP_KEYHUGE DOUBLEHUGE
  #define AHEAP_KEYF "%f"
 #elif defined(AHEAP_KEY_WEIGHT)
  #define AHEAP_KEY  WEIGHT
  #define AHEAP_KEYHUGE WEIGHTHUGE
  #define AHEAP_KEYF WEIGHTF
 #else
#define AHEAP_KEY  int
  #define AHEAP_KEYHUGE INTHUGE
  #define AHEAP_KEYF "%d"
 #endif
#endif

#ifndef AHEAP_ID
 #define AHEAP_ID int
 #define AHEAP_ID_HUGE INTHUGE
 #define AHEAP_IDF "%d"
#endif

#define AHEAP_IDX(H,i) (((i)+1-(H).base)%(H).end)
#define AHEAP_LEAF(H,i)   (((i)+(H).base)%(H).end+(H).end-1)
#define AHEAP_H(H,i)   (H).v[(((i)+(H).base)%(H).end+(H).end-1)]

typedef struct {
  unsigned char type;
  AHEAP_KEY *v;       /* array for heap key */
  AHEAP_ID end;       /* the number of maximum elements */
  AHEAP_ID base;      /* the constant for set 0 to the leftmost leaf */
} AHEAP;

QSORT_TYPE_HEADER (AHEAP_KEY, AHEAP_KEY)
QSORT_TYPE_HEADER (AHEAP_ID, AHEAP_ID)
extern AHEAP INIT_AHEAP;

/* initialization. allocate memory for H and fill it by +infinity */
void AHEAP_alloc (AHEAP *H, int num);
void AHEAP_end (AHEAP *H);

/* return the index of the leaf having the minimum key among the descendants
  of the given node i. If several leaves with the smallest key are there, 
  return the minimum index among them if f=0, maximum index if f=1, and
  random choice if f=2  */
AHEAP_ID AHEAP_findmin_node_ (AHEAP *H, AHEAP_ID i, int f);
AHEAP_ID AHEAP_findmin_node (AHEAP *H, AHEAP_ID i, int f);
AHEAP_ID AHEAP_findmin_head (AHEAP *H);
AHEAP_ID AHEAP_findmin_tail (AHEAP *H);
AHEAP_ID AHEAP_findmin_rnd (AHEAP *H);

/* return the index of the leaf having smaller value than a among the
  descendants of the given node i. If several leaves with the smallest key
  are there, return the minimum index among them if f=0, maximum index if f=1,
  and random choice if f=2  */
AHEAP_ID AHEAP_findlow_node (AHEAP *H, AHEAP_KEY a, AHEAP_ID i, int f);
AHEAP_ID AHEAP_findlow_head (AHEAP *H, AHEAP_KEY a);
AHEAP_ID AHEAP_findlow_tail (AHEAP *H, AHEAP_KEY a);
AHEAP_ID AHEAP_findlow_rnd (AHEAP *H, AHEAP_KEY a);

/* return the index of the leaf having smaller value than a next/previous to
  leaf i. return -1 if such a leaf does not exist  */
AHEAP_ID AHEAP_findlow_nxt (AHEAP *H, AHEAP_ID i, AHEAP_KEY a);
AHEAP_ID AHEAP_findlow_prv (AHEAP *H, AHEAP_ID i, AHEAP_KEY a);

/* change the key of node i to a /Add a to the key of node i, and update heap H */
void AHEAP_chg (AHEAP *H, AHEAP_ID i, AHEAP_KEY a);
void AHEAP_add (AHEAP *H, AHEAP_ID i, AHEAP_KEY a);

/* update the ancestor of node i */
void AHEAP_update (AHEAP *H, AHEAP_ID i);

/* find the leaf with the minimum key value among the leaves having index 
 smaller/larger than i, or between i and j */
AHEAP_ID AHEAP_upper_min (AHEAP *H, AHEAP_ID i);
AHEAP_ID AHEAP_lower_min (AHEAP *H, AHEAP_ID i);
AHEAP_ID AHEAP_interval_min (AHEAP *H, AHEAP_ID i, AHEAP_ID j);

/* print heap keys according to the structure of the heap */
void AHEAP_print (AHEAP *H);


/******************************************************************************/
/* VHEAP: variable size heap */
/******************************************************************************/


#ifndef VHEAP_KEY
 #ifdef VHEAP_KEY_DOUBLE
  #define VHEAP_KEY  double
  #define VHEAP_KEYHUGE DOUBLEHUGE
  #define VHEAP_KEYF "%f"
 #elif defined(VHEAP_KEY_WEIGHT)
  #define VHEAP_KEY  WEIGHT
  #define VHEAP_KEYHUGE WEIGHTHUGE
  #define VHEAP_KEYF WEIGHTF
 #else
#define VHEAP_KEY  int
  #define VHEAP_KEYHUGE INTHUGE
  #define VHEAP_KEYF "%d"
 #endif
#endif

#ifndef VHEAP_ID
 #define VHEAP_ID int
 #define VHEAP_ID_HUGE INTHUGE
 #define VHEAP_IDF "%d"
#endif

typedef struct {
  unsigned char type;
  char *v;       // array for heap key
  VHEAP_ID siz, end;       // the current size, and the maximum size
  int unit;   // size of a cell (element) the first has to be VHEAP_KEY
//  int *f();
} VHEAP;

// structure for elements of VHEAP
typedef struct {
  VHEAP_KEY *w;       // weight
  VHEAP_ID i; // ID
} VHEAP_ELE;

QSORT_TYPE_HEADER (VHEAP_KEY, VHEAP_KEY)
QSORT_TYPE_HEADER (VHEAP_ID, VHEAP_ID)
extern VHEAP INIT_VHEAP;

#define VHEAP_I(H,i)    (*((VHEAP_ID *)(((VHEAP_KEY *)(&(H).v[(H).unit*i]))+1)))
#define VHEAP_V(H,i)    (*((VHEAP_KEY *)(&(H).v[(H).unit*i])))


/* initialization. allocate memory for H and fill it by +infinity */
void VHEAP_print (FILE *fp, VHEAP *H);
void VHEAP_print_ele (FILE *fp, VHEAP *H);
void VHEAP_alloc (VHEAP *H, int num, int unit);
void VHEAP_end (VHEAP *H);

/* change the value of i-th node to w, return the position to which the cell moved */
VHEAP_ID VHEAP_dec_ (VHEAP *H, VHEAP_ID i, VHEAP_KEY w);
VHEAP_ID VHEAP_inc_ (VHEAP *H, VHEAP_ID i, VHEAP_KEY w);
VHEAP_ID VHEAP_chg_ (VHEAP *H, VHEAP_ID i, VHEAP_KEY w);
VHEAP_ID VHEAP_dec (VHEAP *H, VHEAP_ID i, VHEAP_KEY w, size_t ii, VHEAP_KEY **hh);
VHEAP_ID VHEAP_inc (VHEAP *H, VHEAP_ID i, VHEAP_KEY w, size_t ii, VHEAP_KEY **hh);
VHEAP_ID VHEAP_chg (VHEAP *H, VHEAP_ID i, VHEAP_KEY w);

/* insert/delete an element */
void VHEAP_ins_ (VHEAP *H, VHEAP_KEY w);
VHEAP_ID VHEAP_ins (VHEAP *H, VHEAP_KEY w);  // return the final position of inserted cell
void VHEAP_ins_ele (VHEAP *H, VHEAP_KEY w, VHEAP_ID v);
void VHEAP_rm_ (VHEAP *H, VHEAP_ID i);
void VHEAP_rm (VHEAP *H, VHEAP_ID i);

/* extract the minimum element from H */
VHEAP_KEY VHEAP_ext_min_ (VHEAP *H);
VHEAP_KEY VHEAP_ext_min (VHEAP *H);
VHEAP_KEY VHEAP_ext_min_ele (VHEAP *H, VHEAP_ID *i);


/******************************************************************************/
/* IHEAP: variable size, index heap */
/******************************************************************************/


#ifndef IHEAP_KEY
 #ifdef IHEAP_KEY_DOUBLE
  #define IHEAP_KEY  double
  #define IHEAP_KEYHUGE DOUBLEHUGE
  #define IHEAP_KEYF "%f"
 #elif defined(IHEAP_KEY_WEIGHT)
  #define IHEAP_KEY  WEIGHT
  #define IHEAP_KEYHUGE WEIGHTHUGE
  #define IHEAP_KEYF WEIGHTF
 #else
#define IHEAP_KEY  int
  #define IHEAP_KEYHUGE INTHUGE
  #define IHEAP_KEYF "%d"
 #endif
#endif

#ifndef IHEAP_ID
 #define IHEAP_ID int
 #define IHEAP_ID_HUGE INTHUGE
 #define IHEAP_IDF "%d"
#endif


typedef struct {
  unsigned char type;
  IHEAP_ID *v;       // array for heap ID's
  IHEAP_ID siz, end;       // the current size, and the maximum size
  IHEAP_KEY *x;   // array for the values (IHEAP does not prepare this)
  int mode;  // mode; minheap = 1, maxhead = 2;
  int *f;  // function for comparing the values; under construction
  int unit;  // size of a value-cell in H->x 
} IHEAP;

QSORT_TYPE_HEADER (IHEAP_KEY, IHEAP_KEY)
QSORT_TYPE_HEADER (IHEAP_ID, IHEAP_ID)
extern IHEAP INIT_IHEAP;

#define IHEAP_P(H,i)    (H->f? (IHEAP_KEY *)(&((char *)H->x)[H->v[i]*H->unit]): &H->x[H->v[i]])


/* initialization. allocate memory for H and fill it by +infinity */
void IHEAP_print (FILE *fp, IHEAP *H);
void IHEAP_print_ele (FILE *fp, IHEAP *H);
void IHEAP_alloc (IHEAP *H, int num, int mode, IHEAP_KEY *x);
void IHEAP_end (IHEAP *H);

/* change the value of i-th node to w, return the position to which the cell moved */
IHEAP_ID IHEAP_dec (IHEAP *H, IHEAP_ID i, IHEAP_ID j);
IHEAP_ID IHEAP_inc (IHEAP *H, IHEAP_ID i, IHEAP_ID j);
IHEAP_ID IHEAP_chg (IHEAP *H, IHEAP_ID i, IHEAP_ID j);

/* insert/delete an element */
IHEAP_ID IHEAP_ins (IHEAP *H, IHEAP_ID j);
void IHEAP_rm (IHEAP *H, IHEAP_ID i);

/* extract the minimum element from H */
IHEAP_ID IHEAP_ext_min (IHEAP *H);

#endif
