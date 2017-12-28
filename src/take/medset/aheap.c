/*
    array-based simple heap (fixex size)
            12/Apr/2001   by Takeaki Uno  e-mail:uno@nii.jp, 
    homepage:   http://research.nii.ac.jp/~uno/index.html  */
/* This program is available for only academic use, basically.
   Anyone can modify this program, but he/she has to write down 
    the change of the modification on the top of the source code.
   Neither contact nor appointment to Takeaki Uno is needed.
   If one wants to re-distribute this code, please
    refer the newest code, and show the link to homepage of 
    Takeaki Uno, to notify the news about the codes for the users. */

#ifndef _aheap_c_
#define _aheap_c_

#include"aheap.h"
#include"stdlib2.c"

QSORT_TYPE (AHEAP_KEY, AHEAP_KEY)
QSORT_TYPE (AHEAP_ID, AHEAP_ID)
AHEAP INIT_AHEAP = {TYPE_AHEAP,NULL,0,0};

/* print heap keys according to the structure of the heap */
void AHEAP_print (AHEAP *H){
  AHEAP_ID i, j=1;
  while ( j<=H->end*2-1 ){
    FLOOP (i, j-1, MIN(j, H->end)*2-1) printf (AHEAP_KEYF ",", H->v[i] );
    printf ("\n");
    j = j*2;
  }
}

/* allocate memory */
void AHEAP_alloc (AHEAP *H, AHEAP_ID num){
  AHEAP_ID i;
#ifdef ERROR_CHECK
  if ( num<0 ) error_num ("size is out of range", num, EXIT);
#endif
  *H = INIT_AHEAP;
  if ( num>0 ) malloc2 (H->v, num*2, EXIT);
  H->end = num;
  ARY_FILL (H->v, 0, num*2, AHEAP_KEYHUGE);
  for (i=0 ; i<num-1 ; i=i*2+1);
  H->base = i - num + 1;
}

/* termination */
void AHEAP_end (AHEAP *H){
  free2 (H->v);
  *H = INIT_AHEAP;
}

/* return the index of the leaf having the minimum key among the descendants
  of the given node i. If several leaves with the smallest key are there, 
  return the minimum index among them if f=0, maximum index if f=1, and
  random choice if f=2  */
/* random choice version. choose one child to go down randomly for each node,
   thus it is not uniformly random */
/* node_ returns the ID of leaf */
AHEAP_ID AHEAP_findmin_node_ (AHEAP *H, AHEAP_ID i, int f){
  while ( i < H->end-1 ){
    if ( H->v[i*2+1] == H->v[i] )
      if ( H->v[i*2+2] == H->v[i] )
        if ( f == 2 ) i = i*2 + 1 + rand()%2;
        else i = i*2+1+f;
      else i = i*2+1;
    else i = i*2+2;
  }
  return (i);
}
AHEAP_ID AHEAP_findmin_node (AHEAP *H, AHEAP_ID i, int f){
  if ( H->end <= 0 ) return (-1);
  return (AHEAP_IDX(*H, AHEAP_findmin_node_ (H, i, f)));
}
AHEAP_ID AHEAP_findmin_head (AHEAP *H){ return (AHEAP_findmin_node (H, 0, 0) ); }
AHEAP_ID AHEAP_findmin_tail (AHEAP *H){ return (AHEAP_findmin_node (H, 0, 1) ); }
AHEAP_ID AHEAP_findmin_rnd (AHEAP *H){ return (AHEAP_findmin_node (H, 0, 2) ); }

/* return the index of the leaf having smaller value than a among the
  descendants of the given node i. If several leaves with the smallest key
  are there, return the minimum index among them if f=0, maximum index if f=1,
  and random choice if f=2  */
AHEAP_ID AHEAP_findlow_node (AHEAP *H, AHEAP_KEY a, AHEAP_ID i, int f){
  if ( H->end == 0 ) return (-1); 
  if ( H->v[0] > a ) return (-1);
  while ( i < H->end-1 ){
    if ( f == 2 ) {
      if ( H->v[i*2+1] <= a )
          if ( H->v[i*2+2] <= a ) i = i*2 + 1 + rand()%2;
          else i = i*2+1;
      else i = i*2+2;
    } else if ( H->v[i*2+1] <= a ) i = i*2+1+f; else i = i*2+2-f;
  }
  return (AHEAP_IDX(*H, i) );
}
AHEAP_ID AHEAP_findlow_head (AHEAP *H, AHEAP_KEY a){ return (AHEAP_findlow_node (H, a, 0, 0) ); }
AHEAP_ID AHEAP_findlow_tail (AHEAP *H, AHEAP_KEY a){ return (AHEAP_findlow_node (H, a, 0, 1) ); }
AHEAP_ID AHEAP_findlow_rnd (AHEAP *H, AHEAP_KEY a){ return (AHEAP_findlow_node (H, a, 0, 2) ); }

/* return the index of the leaf having smaller value than a next/previous to
  leaf i. return -1 if such a leaf does not exist  */
AHEAP_ID AHEAP_findlow_nxt (AHEAP *H, AHEAP_ID i, AHEAP_KEY a){
  if ( H->end == 0 ) return (-1);
  if ( i<0 || i>= H->end ) return ( AHEAP_findlow_head (H, a));
  for (i=AHEAP_LEAF(*H,i); i>0 ; i=(i-1)/2){
     /* i is the child of smaller index, and the key of the sibling of i is less than a */
    if ( i%2 == 1 && H->v[i+1] <= a ) return (AHEAP_findlow_node (H, a, i+1, 0) );
  }
  return (-1);
}
AHEAP_ID AHEAP_findlow_prv (AHEAP *H, AHEAP_ID i, AHEAP_KEY a){
  if ( H->end == 0 ) return (-1); 
  if ( i<0 || i>= H->end ) return ( AHEAP_findlow_head (H, a));
  for (i=AHEAP_LEAF(*H,i); i>0 ; i=(i-1)/2){
     /* i is the child of larger index, and the key of the sibling of i is less than a */
    if ( i%2 == 0 && H->v[i-1] <= a ) return (AHEAP_findlow_node (H, a, i-1, 1) );
  }
  return (-1);
}

/* change the key of node i to a /Add a to the key of node i, and update heap H */
void AHEAP_chg (AHEAP *H, AHEAP_ID i, AHEAP_KEY a){
  i = AHEAP_LEAF (*H, i);
  H->v[i] = a;
  AHEAP_update (H, i);
}
void AHEAP_add (AHEAP *H, AHEAP_ID i, AHEAP_KEY a){
  i = AHEAP_LEAF (*H, i);
  H->v[i] += a;
  AHEAP_update (H, i);
}

/* update the ancestor of node i */
void AHEAP_update (AHEAP *H, AHEAP_ID i){
  AHEAP_ID j;
  AHEAP_KEY a = H->v[i];
  while ( i>0 ){
    j = i - 1 + (i%2)*2;   /* j = the sibling of i */
    i = (i-1) / 2;
    if ( H->v[j] < a ) a = H->v[j];
    if ( a == H->v[i] ) break;
    H->v[i] = a;
  }
}

/* find the leaf with the minimum key value among the leaves having index 
 smaller/larger than i, or between i and j */
AHEAP_ID AHEAP_upper_min (AHEAP *H, AHEAP_ID i){
  AHEAP_ID fi=0, j = AHEAP_LEAF (*H, H->end - 1);
  AHEAP_KEY fm = AHEAP_KEYHUGE;
  if ( i == 0 ) return (AHEAP_findmin_head (H) );
  i = AHEAP_LEAF (*H, i-1);
  while ( i != j ){
    if ( i%2 ){ /* if i is the child with smaller index */
      if ( fm > H->v[i+1] ){
        fm = H->v[i+1];
        fi = i+1;
      }
    }
    i = (i-1)/2;
    if ( j == i ) break;  /* stop if the right pointer and the left pointer are the same */
    j = (j-1)/2;
  }
  while ( fi < H->end-1 ) fi = fi*2 + (H->v[fi*2+1]<=fm?1:2);
  return ( AHEAP_IDX(*H, fi) );
}
AHEAP_ID AHEAP_lower_min (AHEAP *H, AHEAP_ID i){
  AHEAP_ID fi=0, j = AHEAP_LEAF (*H, 0);
  AHEAP_KEY fm = AHEAP_KEYHUGE;
  if ( i == H->end-1 ) return (AHEAP_findmin_head (H) );
  i = AHEAP_LEAF (*H, i+1);
  while ( i != j ){
    if ( i%2 == 0 ){ /* if i is the child of larger index */
      if ( fm > H->v[i-1] ){
        fm = H->v[i-1];
        fi = i-1;
      }
    }
    j = (j-1)/2;
    if ( j == i ) break;  /* stop if the right pointer and the left pointer are the same */
    i = (i-1)/2;
  }
  while ( fi < H->end-1 ) fi = fi*2 + (H->v[fi*2+1]<=fm?1:2);
  return (AHEAP_IDX(*H, fi) );
}

/* find the index having the minimum among given two indices */
AHEAP_ID AHEAP_interval_min (AHEAP *H, AHEAP_ID i, AHEAP_ID j){
  AHEAP_ID fi=0;
  AHEAP_KEY fm = AHEAP_KEYHUGE;
  if ( i == 0 ) return (AHEAP_lower_min (H, j) );
  if ( j == H->end-1 ) return (AHEAP_upper_min (H, i) );
  i = AHEAP_LEAF (*H, i-1);
  j = AHEAP_LEAF (*H, j+1);
  while ( i != j && i != j-1 ){
    if ( i%2 ){ /* if i is the child of smaller index */
      if ( fm > H->v[i+1] ){
        fm = H->v[i+1];
        fi = i+1;
      }
    }
    i = (i-1)/2;
    if ( j == i || j == i+1 ) break; // stop if the right pointer and the left pointer are the same
    if ( j%2 == 0 ){ // if j is the child of larger index
      if ( fm > H->v[j-1] ){
        fm = H->v[j-1];
        fi = j-1;
      }
    }
    j = (j-1)/2;
  }
  while ( fi < H->end-1 )
      fi = fi*2 + (H->v[fi*2+1] <= fm?1:2);
  return (AHEAP_IDX(*H, fi) );
}


/******************************************************************************/
/* VHEAP: variable size heap */
/******************************************************************************/

QSORT_TYPE (VHEAP_KEY, VHEAP_KEY)
QSORT_TYPE (VHEAP_ID, VHEAP_ID)
VHEAP INIT_VHEAP = {TYPE_VHEAP,NULL,0,0,0};

/* print heap keys according to the structure of the heap */
void VHEAP_print (FILE *fp, VHEAP *H){
  VHEAP_ID i=0, j=1, ii;
  fprintf (fp, "siz:" VHEAP_IDF ", end: " VHEAP_IDF ", unit:%d\n", H->siz, H->end, H->unit);
  while (i < H->siz){
    ii = MIN (i+j, H->siz);
    while (i < ii){
      fprintf (fp, VHEAP_KEYF ",", VHEAP_V(*H,i) );
      i++;
    }
    fprintf (fp, "\n");
    j = j*2;
  }
}
void VHEAP_print_ele (FILE *fp, VHEAP *H){
  VHEAP_ID i=0, j=1, ii;
  fprintf (fp, "siz:" VHEAP_IDF ", end: " VHEAP_IDF ", unit:%d\n", H->siz, H->end, H->unit);
  while (i < H->siz){
    ii = MIN (i+j, H->siz);
    while (i < ii){
      fprintf (fp, VHEAP_KEYF "(" VHEAP_IDF "),", VHEAP_V(*H,i), VHEAP_I(*H,i) );
      i++;
    }
    fprintf (fp, "\n");
    j = j*2;
  }
}

/* allocate memory */
void VHEAP_alloc (VHEAP *H, VHEAP_ID num, int unit){
#ifdef ERROR_CHECK
  if ( num<0 ) error_num ("index is out of range", num, EXIT);
#endif
  *H = INIT_VHEAP;
  ENMAX (unit, sizeof(VHEAP_KEY));
  if (num == 0) num = 16;
  malloc2 (H->v, num*unit, EXIT);
  H->end = num;
  H->unit = unit;
}

/* termination */
void VHEAP_end (VHEAP *H){
  free2 (H->v);
  *H = INIT_VHEAP;
}

/* update VHEAP for decrease/increase of the value of i-th node to w
   return the index to that w is written */
/* no appended area version (each cell consists only of a VHEAP_KEY) */
VHEAP_ID VHEAP_dec_ (VHEAP *H, VHEAP_ID i, VHEAP_KEY w){
  VHEAP_KEY *h=(VHEAP_KEY *)H->v;
  VHEAP_ID ii;
  while (i > 0){
    ii = (i-1)/2;
    if (h[ii] <= w) break;
    h[i] = h[ii];
    i = ii;
  }
  h[i] = w;
  return (i);
}
VHEAP_ID VHEAP_inc_ (VHEAP *H, VHEAP_ID i, VHEAP_KEY w){
  VHEAP_ID jj, j1, j2, end=(H->siz-1)/2;
  VHEAP_KEY *h=(VHEAP_KEY *)H->v;
  while (i < end){
    j1 = i*2+1; j2 = j1+1;
    jj = h[j1]<=h[j2]? j1: j2;
    if ( w <= h[jj] ) goto END;
    h[i] = h[jj];
    i = jj;
  }
  if (i == end && (H->siz&1)==0){
    jj = i*2+1;
    if ( w > h[jj] ){ h[i] = h[jj]; i = jj; }
  }
  END:;
  h[i] = w;
  return (i);
}


/* update VHEAP for decrease/increase of the value of i-th node to w
   return the index to that w is written */
VHEAP_ID VHEAP_dec (VHEAP *H, VHEAP_ID i, VHEAP_KEY w, size_t ii, VHEAP_KEY **hh){
  VHEAP_KEY *h;
//  size_t unit = H->unit-sizeof(VHEAP_KEY);
  while (i > 0){
    ii = (i&1)? (ii-H->unit)/2: ii/2-H->unit;
    h = (VHEAP_KEY *)(H->v + ii);
    if (*h <= w) break;
    memcpy (*hh, h, H->unit);
    i = (i-1)/2; *hh = h;
  }
  **hh = w;
  return (i);
}
VHEAP_ID VHEAP_inc (VHEAP *H, VHEAP_ID i, VHEAP_KEY w, size_t ii, VHEAP_KEY **hh){
  size_t jj, j1, j2, end=(H->siz-1)/2;
  VHEAP_KEY *h1, *h2, *h;
  end *= H->unit;
  while (ii < end){
    j1 = ii*2+H->unit; j2 = j1+H->unit;
    h1 = (VHEAP_KEY *)(H->v + j1); h2 = (VHEAP_KEY *)(H->v + j2);
    if ( *h1 <= *h2 ){ jj = j1; h = h1; } else { jj = j2; h = h2; }
    if ( w <= *h ) goto END;
    memcpy (*hh, h, H->unit);
    *hh = h; ii = jj;
  }
  if (ii == end && (H->siz&1)==0){
    jj = ii*2+H->unit; h = (VHEAP_KEY *)(H->v + jj);
    if ( w > *h ){ memcpy (*hh, h, H->unit); ii = jj; *hh = h; }
  }
  END:;
  **hh = w;
  return (ii/H->unit);
}

//  memcpy (hh+1, he, H->unit-sizeof(VHEAP_KEY)); // copy preserved appended area to the target position

/* change the value of i-th node to w */
VHEAP_ID VHEAP_chg (VHEAP *H, VHEAP_ID i, VHEAP_KEY w){
  size_t ii = i*H->unit, unit = H->unit-sizeof(VHEAP_KEY);
  VHEAP_KEY *h = (VHEAP_KEY *)(H->v + ii), *he = (VHEAP_KEY *)(H->v + H->siz*H->unit);
#ifdef ERROR_CHECK
  if ( i<0 || i>H->siz ) error_num ("index is out of range", i, EXIT);
#endif
  if ( H->end <= 0 ) return (-1);
  memcpy (he, h+1, unit); // preserve appended area
  if ( w < *h ) i = VHEAP_dec (H, i, w, ii, &h);
  else i = VHEAP_inc (H, i, w, ii, &h);
  memcpy (h+1, he, unit); // preserve appended area
  return (i);
}
/* non-appended version (only with a VHEAP_KEY) */
VHEAP_ID VHEAP_chg_ (VHEAP *H, VHEAP_ID i, VHEAP_KEY w){
#ifdef ERROR_CHECK
  if ( i<0 || i>H->siz ) error_num ("index is out of range", i, EXIT);
#endif
  if ( H->end <= 0 ) return (-1);
  if ( w < VHEAP_V(*H,i) ) return (VHEAP_dec_ (H, i, w));
  else return (VHEAP_inc_ (H, i, w));
}

/* insert/delete an element */
void VHEAP_ins_ (VHEAP *H, VHEAP_KEY w){
  H->siz++;
  if (H->siz >= H->end){ H->end = MAX(H->end*2, H->siz+1); realloc2 (H->v, H->end*sizeof(VHEAP_KEY),EXIT);}
  VHEAP_dec_ (H, H->siz-1, w);
}
VHEAP_ID VHEAP_ins (VHEAP *H, VHEAP_KEY w){
  size_t ii = H->siz*H->unit;
  VHEAP_KEY *h = (VHEAP_KEY *)(H->v + ii);
  H->siz++;
  if (H->siz >= H->end){ H->end = MAX(H->end*2, H->siz+1); realloc2 (H->v, H->end*H->unit,exit(1));}
//  if (H->siz >= H->end){ H->end = MAX(H->end*2, H->siz+1); realloc2 (H->v, H->end*H->unit,EXIT);}
  return (VHEAP_dec (H, H->siz-1, w, ii, &h));
}
void VHEAP_rm_ (VHEAP *H, VHEAP_ID i){
  H->siz--;
  VHEAP_chg_ (H, i, ((VHEAP_KEY *)(H->v))[H->siz]);
}
void VHEAP_rm (VHEAP *H, VHEAP_ID i){
  H->siz--;
  VHEAP_chg (H, i, VHEAP_V(*H,H->siz));
}
void VHEAP_ins_ele (VHEAP *H, VHEAP_KEY w, VHEAP_ID i){
  size_t ii = H->siz*H->unit;
  VHEAP_KEY *h;
  H->siz++;
  if (H->siz >= H->end){ H->end = MAX(H->end*2, H->siz+1); realloc2 (H->v, H->end*H->unit,EXIT);}
  h = (VHEAP_KEY *)(H->v + ii);
  VHEAP_dec (H, H->siz-1, w, ii, &h);
  *((VHEAP_ID *)(&h[1])) = i;
}

/* extract the minimum element from H */
VHEAP_KEY VHEAP_ext_min_ (VHEAP *H){
  VHEAP_KEY w= *((VHEAP_KEY *)H->v);
  H->siz--;
  VHEAP_inc_ (H, 0, ((VHEAP_KEY *)(H->v))[H->siz]);
  return (w);
}
VHEAP_KEY VHEAP_ext_min (VHEAP *H){
  VHEAP_KEY w= *((VHEAP_KEY *)H->v), *h=(VHEAP_KEY *)H->v, *hh;
  H->siz--;
  hh=(VHEAP_KEY *)(H->v+H->siz*H->unit);
  VHEAP_inc (H, 0, *hh, 0, &h);
  memcpy (h+1, hh+1, H->unit-sizeof(VHEAP_KEY)); // preserve appended area
  return (w);
}
VHEAP_KEY VHEAP_ext_min_ele (VHEAP *H, VHEAP_ID *i){
  VHEAP_KEY w= *((VHEAP_KEY *)H->v), *h=(VHEAP_KEY *)H->v, *hh;
  *i = *((VHEAP_ID *)(H->v+sizeof(VHEAP_KEY)));
  H->siz--;
  hh=(VHEAP_KEY *)(H->v+H->siz*H->unit);
  VHEAP_inc (H, 0, *hh, 0, &h);
  *((VHEAP_ID *)(&h[1])) = *((VHEAP_ID *)(&hh[1]));
  return (w);
}


/******************************************************************************/
/* VHEAP: variable size, index heap  */
/******************************************************************************/

QSORT_TYPE (IHEAP_KEY, IHEAP_KEY)
QSORT_TYPE (IHEAP_ID, IHEAP_ID)
IHEAP INIT_IHEAP = {TYPE_IHEAP,NULL,0,0,NULL,0,NULL,0};

/* print heap keys according to the structure of the heap */
void IHEAP_print (FILE *fp, IHEAP *H){
  IHEAP_ID i=0, j=1, ii;
  fprintf (fp, "siz:" IHEAP_IDF ", end: " IHEAP_IDF ", unit:%d\n", H->siz, H->end, H->unit);
  while (i < H->siz){
    ii = MIN (i+j, H->siz);
    while (i < ii){
      fprintf (fp, IHEAP_KEYF "(" IHEAP_IDF "),", H->v[i], H->x[i]);
      i++;
    }
    fprintf (fp, "\n");
    j = j*2;
  }
}

/* allocate memory */
void IHEAP_alloc (IHEAP *H, IHEAP_ID num, int mode, IHEAP_KEY *x){
#ifdef ERROR_CHECK
  if ( num<0 ) error_num ("index is out of range", num, EXIT);
#endif
  *H = INIT_IHEAP;
  if (num == 0) num = 16;
  malloc2 (H->v, num, EXIT);
  H->end = num;
  H->mode = mode;
  H->x = x;
  H->unit = 0;
}

/* termination */
void IHEAP_end (IHEAP *H){
  free2 (H->v);
  *H = INIT_IHEAP;
}

/* heap key comparison */
int IHEAP_compare (IHEAP *H, IHEAP_ID a, IHEAP_ID b){
  if ( H->f == NULL ){
    if ( H->mode == 1 ) return (H->x[a] <= H->x[b]);
    if ( H->mode == 2 ) return (H->x[a] >= H->x[b]);
  }// else return H->f( ((char *)H->x)[a*H->unit], ((char *)H->x)[b*H->unit]);
  return (0);
}


/* update IHEAP for decrease/increase of the value of i-th node to j
   return the index to that j is written */
IHEAP_ID IHEAP_dec (IHEAP *H, IHEAP_ID i, IHEAP_ID j){
  IHEAP_ID ii;
  while (i > 0){
    ii = (i-1)/2;
    if (IHEAP_compare (H, H->v[i], j)) break;
    H->v[i] = H->v[ii];
    i = ii;
  }
  H->v[i] = j;
  return (i);
}
IHEAP_ID IHEAP_inc (IHEAP *H, IHEAP_ID i, IHEAP_ID j){
  IHEAP_ID jj, j1, j2, end=(H->siz-1)/2;
  while (i < end){
    j1 = i*2+1; j2 = j1+1;
    jj = IHEAP_compare (H, H->v[j1], H->v[j2])? j1: j2;
    if ( IHEAP_compare (H, j, H->v[jj])) goto END;
    H->v[i] = H->v[jj];
    i = jj;
  }
  if (i == end && (H->siz&1)==0){
    jj = i*2+1;
    if ( !IHEAP_compare (H, H->v[jj], j)){ H->v[i] = H->v[jj]; i = jj; }
  }
  END:;
  H->v[i] = j;
  return (i);
}



/* change the value of i-th node to j */
IHEAP_ID IHEAP_chg (IHEAP *H, IHEAP_ID i, IHEAP_ID j){
#ifdef ERROR_CHECK
  if ( i<0 || i>H->siz ) error_num ("index is out of range", i, EXIT);
#endif
  if ( H->end <= 0 ) return (IHEAP_ID_HUGE);
  if ( IHEAP_compare (H, j, H->v[i]) ) return (IHEAP_dec (H, i, j));
  else return (IHEAP_inc (H, i, j));
}

/* insert/delete an element */
IHEAP_ID IHEAP_ins (IHEAP *H, IHEAP_ID j){
  H->siz++;
  if (H->siz >= H->end){ H->end = MAX(H->end*2, H->siz+1); realloc2 (H->v, H->end, exit(1));}
  return (IHEAP_dec (H, H->siz-1, j));
}
void IHEAP_rm (IHEAP *H, IHEAP_ID i){
  H->siz--;
  IHEAP_chg (H, i, H->v[H->siz]);
  H->v[H->siz] = i;  // preserve the deleted cell
}

/* extract the minimum element from H */
IHEAP_ID IHEAP_ext_min (IHEAP *H){
  IHEAP_ID j = H->v[0];
  H->siz--;
  IHEAP_inc (H, 0, H->v[H->siz]);
  H->v[H->siz] = j;  // preserve the deleted cell
  return (j);
}

#endif
