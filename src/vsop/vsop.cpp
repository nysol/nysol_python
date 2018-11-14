#include "Python.h"
//#undef tolower
//#undef toupper
//#undef isalpha
//#undef islower
//#undef isupper
//#undef isspace
//#undef isalnum
#include <fstream>
#include <memory>
#include <stack>
#include"CtoI.h"
#include"ZBDDDG.h"
#include"vsop.h"

extern int CtoI_Lcm1_ub(char *, char *, int, int, int); // by ham

using namespace std;

#if PY_MAJOR_VERSION >= 3
extern "C" {
	PyMODINIT_FUNC PyInit__vsoplib(void);
}
#define PyStr_FromFormat PyUnicode_FromFormat

#else

extern "C" {
	void init_vsoplib(void);
}

#define PyStr_FromFormat PyString_FromFormat

#endif

/*
static char* strGET(PyObject* data){
#if PY_MAJOR_VERSION >= 3
	return PyUnicode_AsUTF8(data);
#else		
	return PyString_AsString(data);
#endif

}
*/
#if PY_MAJOR_VERSION >= 3
 #define strGET PyUnicode_AsUTF8
#else		
 #define strGET PyString_AsString
#endif

static bool strCHECK(PyObject* data){

#if PY_MAJOR_VERSION >= 3
	return PyUnicode_Check(data);
#else		
	return PyString_Check(data);
#endif

}

static bool numCHECK(PyObject* data){

#if PY_MAJOR_VERSION >= 3
	return PyLong_Check(data);
#else		
	return PyInt_Check(data) || PyLong_Check(data);
#endif

}

void yyerror(const char* s)
{
  cerr << s << ".\n";
  cerr.flush();
}



//数値チェック（アイテムが数値なら警告）
static void num_check(char *str)
{
	char *p=str;
	if(*p=='-' || *p=='+' ) p++;
		while(*p){
		if( ( *p>='0' && *p<='9' ) || *p == '.' ){
				fprintf(stderr,"chech %c\n",*p);
				fprintf(stderr,"use numbers for symbol Variable\n");
				break;
		}
		p++;
	}
}
struct OV
{
  char _ovf;

  OV(void) { _ovf = 0; }
  void Set(void) { _ovf = 1; }
  char Check(void)
  {
    char a = _ovf;
    _ovf = 0;
    return a;
  }
} OVF;

//初期化カウント
int init_cnt=0;
int env_nmax=0;
bool env_warning = false;
// global 
PyObject* py_partly ;

//この値はBDDライブラリとかぶらないよう注意すること
// キャッシュがおかしくなる
static const int power16 = 1 << 16;
static const char BC_VSOP_VALUE = 50;
static const char BC_VSOP_DENSITY = 51;
static const char BC_VSOP_MAXCOST = 52;
static const char BC_VSOP_MINCOST = 53;
static const char BC_VSOP_PRODUCT = 54;

static const char BC_CtoI_DELTA =  50;

CtoI CtoI_Delta(CtoI a, CtoI b)
{
  if(a == CtoI_Null())  return a;
  if(b == CtoI_Null()) return b;
  if(a == 0) return 0;
  if(b == 0) return 0;
  if(a == 1 && b==1) return 1;


  int atop = a.Top();
  int btop = b.Top();
  if(BDD_LevOfVar(atop) < BDD_LevOfVar(btop)) return CtoI_Delta(b, a);

  bddword ax = a.GetZBDD().GetID();
  bddword bx = b.GetZBDD().GetID();
  if(atop == btop && ax < bx) return CtoI_Delta(b, a);

  ZBDD z = BDD_CacheZBDD(BC_CtoI_DELTA, ax, bx);
  if(z != -1) return CtoI(z);

  CtoI a0 = a.Factor0(atop);
  CtoI a1 = a.Factor1(atop);
  CtoI c;
  if(atop != btop)
  {
		if(a.IsBool()) c =  CtoI_Union( CtoI_Delta(a0, b), CtoI_Delta(a1, b).AffixVar(atop) );
    else c = CtoI_Delta(a0, b) + CtoI_Delta(a1, b).TimesSysVar(atop);
  }
  else
  {
    CtoI b0 = b.Factor0(atop);
    CtoI b1 = b.Factor1(atop);
    if(a.IsBool())
      c = CtoI_Union( CtoI_Delta(a0, b0) + CtoI_Delta(a1, b1),
             (CtoI_Delta(a1, b0)+ CtoI_Delta(a0, b1)).AffixVar(atop) ) ;
		else if(atop > 1)
      c = CtoI_Delta(a0, b0)
        + (CtoI_Delta(a1, b0) + CtoI_Delta(a0, b1)).TimesSysVar(atop)
        + CtoI_Delta(a1, b1).TimesSysVar(atop-1);
    else BDDerr("CtoI_Delta(): SysVar overflow.");
  }
  BDD_CacheEnt(BC_CtoI_DELTA, ax, bx, c.GetZBDD().GetID());
  return c;
}


static CtoI Product(CtoI, CtoI);
CtoI Product(CtoI ac, CtoI bc)
{
  if(ac == 1) return bc;
  if(bc == 1) return ac;
  if(ac == CtoI_Null()) return ac;
  if(bc == CtoI_Null()) return bc;
  if(ac == 0) return 0;
  if(bc == 0) return 0;

  CtoI a = ac; CtoI b = bc;
  int atop = a.Top();
  int btop = b.Top();
  if(BDD_LevOfVar(atop) < BDD_LevOfVar(btop))
  {
    a = bc; b = ac;
    atop = a.Top(); btop = b.Top();
  }

  bddword ax = a.GetZBDD().GetID();
  bddword bx = b.GetZBDD().GetID();
  if(atop == btop && ax < bx)
  {
    a = bc; b = ac;
    ax = a.GetZBDD().GetID();
    bx = b.GetZBDD().GetID();
  }

  ZBDD z = BDD_CacheZBDD(BC_VSOP_PRODUCT, ax, bx);
  if(z != -1) return CtoI(z);
  BDD_RECUR_INC;

  CtoI a0 = a.Factor0(atop);
  CtoI a1 = a.Factor1(atop);
  CtoI c;
  if(atop != btop)
  {
    if(atop > BDDV_SysVarTop)
    {
      int azvar = VTable.GetGID(atop);
      CtoI bz = b;
      int bztop = bz.Top();
      while(BDD_LevOfVar(azvar) <= BDD_LevOfVar(bztop))
      {
        bz = bz.Factor0(bztop);
	bztop = bz.Top();
      }
      c = CtoI_Union(Product(a0, b), Product(a1, bz).AffixVar(atop));
    }
    else c = Product(a0, b) + Product(a1, b).TimesSysVar(atop);
  }
  else
  {
    CtoI b0 = b.Factor0(atop);
    CtoI b1 = b.Factor1(atop);
    if(atop > BDDV_SysVarTop)
    {
      int azvar = VTable.GetGID(atop);
      CtoI az = a;
      int aztop = az.Top();
      while(BDD_LevOfVar(azvar) <= BDD_LevOfVar(aztop))
      {
        az = az.Factor0(aztop);
	aztop = az.Top();
      }
      CtoI bz = b;
      int bztop = bz.Top();
      while(BDD_LevOfVar(azvar) <= BDD_LevOfVar(bztop))
      {
        bz = bz.Factor0(bztop);
	bztop = bz.Top();
      }
      c = CtoI_Union(Product(a0, b0),
          (Product(a1,bz)+Product(az,b1)+Product(a1,b1)).AffixVar(atop));
    }
    else if(atop > 1)
      c = Product(a0,b0) + (Product(a1,b0)+Product(a0,b1)).TimesSysVar(atop)
        + Product(a1,b1).TimesSysVar(atop - 1);
    else BDDerr("CtoI::operator*(): SysVar overflow.");
  }

  BDD_RECUR_DEC;
  BDD_CacheEnt(BC_VSOP_PRODUCT, ax, bx, c.GetZBDD().GetID());
  return c;
}

static int Value(CtoI);
int Value(CtoI a)
{
  if(a == CtoI()) return 0;
  if(a == 0) return 0;
  if(a == 1) return power16;

  int val = BDD_CacheInt(BC_VSOP_VALUE, a.GetZBDD().GetID(), 0);
  if(val <= BDD_MaxNode) return val;
  BDD_RECUR_INC;

  int var = a.Top();
  float f;
  if(var > BDDV_SysVarTop)
    f = (float)VTable.GetValue(var) / power16;
  else
  {
    f = -2;
    for(int i=0; i<(BDDV_SysVarTop-var); i++) f *= f;
  }
  int val1 = Value(a.Factor1(var));
  val = (int)(f * val1);
  if(val)
  {
    if((val1<0)^(f<0)^(val<0)) OVF.Set();
  }
  else if((f!=0) && val1) OVF.Set();
  int val0 = Value(a.Factor0(var));
  val1 = val;
  val += val0; 
  if(val1>0 && val0>0 && val<0) OVF.Set();
  if(val1<0 && val0<0 && val>0) OVF.Set();

  BDD_RECUR_DEC;
  BDD_CacheEnt(BC_VSOP_VALUE, a.GetZBDD().GetID(), 0, val);
  return val;
}

static const int power30 = 1 << 30;

static int Density(ZBDD, int);
int Density(ZBDD f, int tlev)
{
  if(f == -1) return 0;
  if(f == 0) return 0;

  int var = f.Top();
  int lev = BDD_LevOfVar(var);
  int c;
  if(f == 1) c = power30;
  else
  {
    c = BDD_CacheInt(BC_VSOP_DENSITY, f.GetID(), 0);
    if(c > BDD_MaxNode)
    {
      BDD_RECUR_INC;
      c = (Density(f.OffSet(var), lev-1) >> 1)
        + (Density(f.OnSet0(var), lev-1) >> 1);
      BDD_RECUR_DEC;
      BDD_CacheEnt(BC_VSOP_DENSITY, f.GetID(), 0, c);
    }
  }
  for(int i=1; i<=tlev-lev; i++) c >>= 1;
  return c;
}

static int MaxCost(ZBDD);
int MaxCost(ZBDD f)
{
  if(f == -1) return 0;
  if(f == 0) return -power30;
  if(f == 1) return 0;

  int c = BDD_CacheInt(BC_VSOP_MAXCOST, f.GetID(), 0);
  if(c <= BDD_MaxNode) return c;
  BDD_RECUR_INC;

  int var = f.Top();
  int c0 = MaxCost(f.OffSet(var));
  int c1 = MaxCost(f.OnSet0(var)) + VTable.GetValue(var);
  c = (c0 > c1)? c0: c1;

  BDD_RECUR_DEC;
  BDD_CacheEnt(BC_VSOP_MAXCOST, f.GetID(), 0, c);
  return c;
}

static int MinCost(ZBDD);
int MinCost(ZBDD f)
{
  if(f == -1) return 0;
  if(f == 0) return power30;
  if(f == 1) return 0;

  int c = BDD_CacheInt(BC_VSOP_MINCOST, f.GetID(), 0);
  if(c <= BDD_MaxNode) return c;
  BDD_RECUR_INC;

  int var = f.Top();
  int c0 = MinCost(f.OffSet(var));
  int c1 = MinCost(f.OnSet0(var)) + VTable.GetValue(var);
  c = (c0 > c1)? c1: c0;

  BDD_RECUR_DEC;
  BDD_CacheEnt(BC_VSOP_MINCOST, f.GetID(), 0, c);
  return c;
}



struct VsopEachNode{
	CtoI * val;
	int pos ; 
};

class VsopEach{

	int Depth_e;
	int * S_Var_e;
	std::stack<VsopEachNode*> _stk;
	bool err;
	
  public:
		VsopEach(CtoI* base){
			if(*base == CtoI_Null()) return ;
			if(*base == 0) return ;
			VsopEachNode *val = new VsopEachNode;
			val->val = new CtoI(*base);
			val->pos = 0;
			_stk.push(val);
			int lev = BDD_LevOfVar(base->TopItem());
			err =false;
			Depth_e = 0;
			S_Var_e = new int[lev];
		}


		int PF(VsopEachNode *a){
		  if(a->val->IsConst()){
		  	return 1;
		  }
		  if(a->pos==0){
			  int v = a->val->TopItem();
  			CtoI b = a->val->Factor1(v);
			  if(b == CtoI_Null()) { err=true;  return 1;}
				VsopEachNode *val = new VsopEachNode;
				val->val = new CtoI(b);
				val->pos = 0;
				_stk.push(val);
  			S_Var_e[Depth_e] = v;
				Depth_e++;
				int sts = PF(val);
				a->pos=1;
				if(sts == 1){ return 1; }
				_stk.pop();
			}
		  if(a->pos==1){
	 			Depth_e--;
			  CtoI b = a->val->Factor0(S_Var_e[Depth_e]);
  			if(b == 0) return 0;
  			if(b == CtoI_Null()){ err=true; return 1;}
				VsopEachNode *val = new VsopEachNode;
				val->val = new CtoI(b);
				val->pos = 0;
				_stk.push(val);
				int sts = PF(val);
				a->pos=2;
				if(sts == 0){ _stk.pop();}
				return sts;
  		}
  		return 0;
		}

		CtoI * next(){
			while(!_stk.empty()){
				//if(_stk.empty()){ return NULL;} 
				VsopEachNode *now = _stk.top();
				if(PF(now))break;
				_stk.pop();
			}
			if(_stk.empty()){ return NULL;} 
			VsopEachNode *out = _stk.top();
				

			CtoI a(*out->val);
			int sign=1;
			if(a.TopDigit() & 1) {sign= -1 ; a = -a; }
			CtoI rtn;
			int c1 = (a != 1);
			char *valrtn;
			if(c1 || Depth_e == 0){
				if(a.TopItem() > 0) a = a.MaxVal();
				int d = a.TopDigit() / 3 + 14;
				valrtn = new char[d];
				int  err = a.StrNum10(valrtn);
				if(err == 1){
					return NULL;
				}
				rtn = CtoI(CtoI_atoi(valrtn));
			}
			else if(!c1){
				rtn = CtoI(CtoI_atoi("1"));;
			}
			for(int i=0; i<Depth_e; i++)
			{
				char *str = VTable.GetName(S_Var_e[i]);
				int ckck = VTable.GetID(str);
				rtn =Product(rtn, CtoI(1).AffixVar( ckck));
			}		
			_stk.pop();
			return new CtoI(rtn);
		}
};


struct VsopRtnInfo{
	CtoI * val;
	int weight ; 
	bool top ; 
	bool bot ; 
};

class VsopItemEach{

	int Depth_e;
	int * S_Var_e;
	std::stack<VsopEachNode*> _stk;
	bool err;
	bool _top;
	bool _bot;
	bool _interrupt;
	int _weight;
	int _ipos;
	
  public:
		VsopItemEach(CtoI* base){
			if(*base == CtoI_Null()) return ;
			if(*base == 0) return ;
			VsopEachNode *val = new VsopEachNode;
			val->val = new CtoI(*base);
			val->pos = 0;
			_stk.push(val);
			int lev = BDD_LevOfVar(base->TopItem());
			err =false;
			Depth_e = 0;
			S_Var_e = new int[lev];
			_top =true;
			_bot =false;
			_interrupt = false;
			_ipos =0;
		}

		VsopRtnInfo * rtnval(){
			bool top = true;
			bool bot = false;
			if(_ipos<Depth_e){
				VsopRtnInfo * vrinfo = new VsopRtnInfo;
				char *str = VTable.GetName(S_Var_e[_ipos]);
				vrinfo->val = new CtoI (CtoI(1).AffixVar(VTable.GetID(str))) ;
				if( _ipos>0 ){ vrinfo->top=false;}
				else     { vrinfo->top=true; }
				_ipos++;
				_interrupt = true;
				if( _ipos==Depth_e ){ vrinfo->bot =true; } 
				else                   { vrinfo->bot =false; } 
				vrinfo->weight = _weight;
				return vrinfo;
			}
			_interrupt = false;
			_ipos=0;
			return NULL;
		}


		int PF(VsopEachNode *a){
		  if(a->val->IsConst()){
		  	return 1;
		  }
		  if(a->pos==0){
			  int v = a->val->TopItem();
  			CtoI b = a->val->Factor1(v);
			  if(b == CtoI_Null()) { err=true;  return 1;}
				VsopEachNode *val = new VsopEachNode;
				val->val = new CtoI(b);
				val->pos = 0;
				_stk.push(val);
  			S_Var_e[Depth_e] = v;
				Depth_e++;
				int sts = PF(val);
				a->pos=1;
				if(sts == 1){ return 1; }
				_stk.pop();
			}
		  if(a->pos==1){
	 			Depth_e--;
			  CtoI b = a->val->Factor0(S_Var_e[Depth_e]);
  			if(b == 0) return 0;
  			if(b == CtoI_Null()){ err=true; return 1;}
				VsopEachNode *val = new VsopEachNode;
				val->val = new CtoI(b);
				val->pos = 0;
				_stk.push(val);
				int sts = PF(val);
				a->pos=2;
				if(sts == 0){ _stk.pop();}
				return sts;
  		}
  		return 0;
		}

		VsopRtnInfo * next(){
			if(_interrupt){
				VsopRtnInfo * rval = rtnval();
				if(rval==NULL){ _stk.pop();}
				else{ return rval ;}
			}
			while(!_stk.empty()){
				VsopEachNode *now = _stk.top();
				if(PF(now))break;
				_stk.pop();
			}
			if(_stk.empty()){ return NULL;} 
			VsopEachNode *out = _stk.top();
				

			CtoI a(*out->val);
			int sign=1;
			if(a.TopDigit() & 1) {sign= -1 ; a = -a; }
			CtoI rtn;
			int c1 = (a != 1);
			char *valrtn;
			if(c1 || Depth_e == 0){
				if(a.TopItem() > 0) a = a.MaxVal();
				int d = a.TopDigit() / 3 + 14;
				valrtn = new char[d];
				int  err = a.StrNum10(valrtn);
				if(err == 1){
					return NULL;
				}
				_weight = atoi(valrtn);
			}
			else if(!c1){
				_weight = 1;
			}
			_ipos=0;
			return rtnval();
		}
};





typedef struct {
  PyObject_HEAD
  CtoI* ss;
} PyCtoIObject;




extern PyTypeObject PyCtoI_Type;

CtoI * int2ctoi(int val)
{
	//初回呼び出し時のみBDDの初期化
	if(init_cnt==0){ BDDV_Init(256, env_nmax);}
	init_cnt++;
	char wrkval[64];
	sprintf(wrkval,"%d",val);
	CtoI *rtnctoi = new CtoI(CtoI_atoi(wrkval));
	return rtnctoi;
}

CtoI* string2ctoi(const char *str)
{
	// 文字列複製する
	char *org = new char [strlen(str)+1];
	strcpy(org,str);
	if(init_cnt==0){ BDDV_Init(256, env_nmax);}
	init_cnt++;
	int sep_cnt=0;
	for(char *p=org;*p;p++){
		if(*p==' '){ 
			sep_cnt++;
		}
	}
	sep_cnt++;
	CtoI * ctoitmp = new CtoI[sep_cnt];

	int i=0;
	char *p,*q;
	p=org;
	q=org;
	while(1){
		if(*p==' '){//区切り文字
			*p='\0';
			int var = VTable.GetID(q);     
			if(var == 0){
				if(env_warning){ num_check(q); }
				VTable.SetB(q, power16/2);
				var  = VTable.GetID(q);
			}
			ctoitmp[i] = CtoI(1).AffixVar(var);
			q=p+1;
			i++;
		}
		else if(*p=='\0'){//終了時文字
			int var = VTable.GetID(q);     
			if(var == 0){
				if(env_warning){ num_check(q); }
			 	VTable.SetB(q, power16/2);
				var  = VTable.GetID(q);
			}
			ctoitmp[i] = CtoI(1).AffixVar(var);
			break;
		}
		p++;
	}
	CtoI ctmp = ctoitmp[0];
	for(int i=1;i<sep_cnt;i++){
		ctmp = Product(ctmp,ctoitmp[i]);
	}
	CtoI *rtnctoi = new CtoI(ctmp);
	if(ctoitmp) delete [] ctoitmp;
	if(org) delete [] org;
	return rtnctoi;
}

/*##vsop_symbol##*/
PyObject* vsop_symbol(PyObject*, PyObject* args)
{
	PyObject* rtn =NULL;
  char *    str1 =NULL ;
	PyObject* pyo2 =NULL;
  char *    str3 =NULL ;
  if (!PyArg_ParseTuple(args, "|sOs", &str1,&pyo2,&str3)) return NULL;

	//初回呼び出し時のみBDDの初期化	
	if(init_cnt==0){ BDDV_Init(256, env_nmax);}
	init_cnt++;

	if(str1!=NULL){
		int val=power16/2;
	  if (pyo2 != NULL && pyo2 != Py_None) {
			if(PyFloat_Check(pyo2)){
				val = (int)(PyFloat_AsDouble(pyo2)*power16);
			}
			else if(numCHECK(pyo2)){
				val = PyLong_AsLong(pyo2)*power16;
			}
			else{
				yyerror("argument type error (arguments must be FLOAT or INT or NIL)\n");
			}
		}
		if(env_warning){ num_check(str1); }
		if(*str1!='\0'){
			int var = VTable.GetID(str1);
			if(var == 0){
				if(str3!=NULL&&!strcmp(str3,"top")){
					 VTable.SetT(str1, val);
				}else{
					VTable.SetB(str1, val);
				}
			}
		}
		rtn =	PyLong_FromLong(0);
	}
	else{
		int n = VTable.Used();
		string vstr ;
		for(int i=n; i>0; i--)
		{
			int var = BDD_VarOfLev(i);
			vstr += VTable.GetName(var);
			vstr += " ";
		}
		rtn = Py_BuildValue("s",vstr.c_str());
	}
	return rtn;
}

/*##vsop_itemset##*/
PyObject* vsop_itemset(PyObject*, PyObject* args)
{
  char * str =NULL ;
  if (!PyArg_ParseTuple(args, "|s", &str)) return NULL;

  PyCtoIObject* ret = reinterpret_cast<PyCtoIObject*>(
      PyCtoI_Type.tp_alloc(&PyCtoI_Type, 0));

	if(str==NULL){
	 	ret->ss = int2ctoi(1);
	}else{
		ret->ss = string2ctoi(str);
	}
	return  reinterpret_cast<PyObject*>(ret);
}

/*##vsop_constant##*/
PyObject* vsop_constant(PyObject*, PyObject* args)
{

	int v =0;
  if (!PyArg_ParseTuple(args, "i", &v)) return NULL;

  PyCtoIObject* ret = reinterpret_cast<PyCtoIObject*>(
      PyCtoI_Type.tp_alloc(&PyCtoI_Type, 0));
	ret->ss = int2ctoi(v);
	return  reinterpret_cast<PyObject*>(ret);
}

/*##vsop_print_totalsize##*/
static PyObject* vsop_totalsize(PyCtoIObject* self){
	BDD_GC();
	return PyLong_FromLong(BDD_Used());
}

typedef struct {
  PyObject_HEAD
  VsopItemEach* it;
} PyCtoI_ItemIterObject;

typedef struct {
  PyObject_HEAD
  VsopEach* it;
} PyCtoIIterObject;


static PyObject* ctoiiter_new(PyTypeObject* type, PyObject* args, PyObject* kwds) {
  PyCtoIIterObject* self;
  self = reinterpret_cast<PyCtoIIterObject*>(type->tp_alloc(type, 0));
  if (self == NULL) return NULL;
  return reinterpret_cast<PyObject*>(self);
}

static PyObject* ctoiitemiter_new(PyTypeObject* type, PyObject* args, PyObject* kwds) {
  PyCtoI_ItemIterObject* self;
  self = reinterpret_cast<PyCtoI_ItemIterObject*>(type->tp_alloc(type, 0));
  if (self == NULL) return NULL;
  return reinterpret_cast<PyObject*>(self);
}


static void ctoiiter_dealloc(PyCtoIIterObject* self) {
  delete self->it;
  PyObject_Del(self);
}
static void ctoiitemiter_dealloc(PyCtoI_ItemIterObject* self) {
  delete self->it;
  PyObject_Del(self);
}


static PyObject* ctoiiter_next(PyCtoIIterObject* self) {
	CtoI *v = self->it->next(); 
	if ( v ){
	  PyCtoIObject* ret = reinterpret_cast<PyCtoIObject*>(
      PyCtoI_Type.tp_alloc(&PyCtoI_Type, 0));
		ret->ss = v;
		return  reinterpret_cast<PyObject*>(ret);
	}
	return NULL;
}

static PyObject* ctoiitemiter_next(PyCtoI_ItemIterObject* self) {
	VsopRtnInfo *v = self->it->next(); 
	if ( v ){
	  PyCtoIObject* ctoiobj = reinterpret_cast<PyCtoIObject*>(
      PyCtoI_Type.tp_alloc(&PyCtoI_Type, 0));
		ctoiobj->ss = v->val;
		return Py_BuildValue("[iOOO]",v->weight,ctoiobj,PyBool_FromLong(v->top),PyBool_FromLong(v->bot));
	}
	return NULL;
}


static PyMethodDef setsetiter_methods[] = {
  {NULL,           NULL}           /* sentinel */
};


static PyTypeObject PyCtoIIter_Type = {
  PyVarObject_HEAD_INIT(&PyType_Type, 0)
  "ctoi_iterator",                          /* tp_name */
  sizeof(PyCtoIIterObject),                 /* tp_basicsize */
  0,                                          /* tp_itemsize */
  /* methods */
  reinterpret_cast<destructor>(ctoiiter_dealloc), /* tp_dealloc */
  0,                                          /* tp_print */
  0,                                          /* tp_getattr */
  0,                                          /* tp_setattr */
  0,                                          /* tp_compare or *tp_reserved */
  0,                                          /* tp_repr */
  0,                                          /* tp_as_number */
  0,                                          /* tp_as_sequence */
  0,                                          /* tp_as_mapping */
  0,                                          /* tp_hash */
  0,                                          /* tp_call */
  0,                                          /* tp_str */
  PyObject_GenericGetAttr,                    /* tp_getattro */
  0,                                          /* tp_setattro */
  0,                                          /* tp_as_buffer */
#if PY_MAJOR_VERSION >= 3
  Py_TPFLAGS_DEFAULT | Py_TPFLAGS_BASETYPE , /* tp_flags */
#else
  Py_TPFLAGS_DEFAULT | Py_TPFLAGS_BASETYPE | Py_TPFLAGS_HAVE_ITER, /* tp_flags */
#endif
  0,                                          /* tp_doc */
  0,                                          /* tp_traverse */
  0,                                          /* tp_clear */
  0,                                          /* tp_richcompare */
  0,                                          /* tp_weaklistoffset */
  PyObject_SelfIter,                          /* tp_iter */
  reinterpret_cast<iternextfunc>(ctoiiter_next), /* tp_iternext */
  setsetiter_methods,                         /* tp_methods */
  0,                                          /* tp_members */
  0,                                          /* tp_getset */
  0,                                          /* tp_base */
  0,                                          /* tp_dict */
  0,                                          /* tp_descr_get */
  0,                                          /* tp_descr_set */
  0,                                          /* tp_dictoffset */
  0,                                          /* tp_init */
  PyType_GenericAlloc,                        /* tp_alloc */
  ctoiiter_new,                             /* tp_new */
#if PY_MAJOR_VERSION >= 3
  0, /* tp_free */
  0, /* tp_is_gc */
  0, /* *tp_bases */
  0, /* *tp_mro */
  0, /* *tp_cache */
  0, /* *tp_subclasses */
  0, /* *tp_weaklist */
  0, /* tp_version_tag */
  0, /* tp_finalize */
#endif
};

static PyTypeObject PyCtoI_ItemIter_Type = {
  PyVarObject_HEAD_INIT(&PyType_Type, 0)
  "ctoi_iterator",                          /* tp_name */
  sizeof(PyCtoIIterObject),                 /* tp_basicsize */
  0,                                          /* tp_itemsize */
  /* methods */
  reinterpret_cast<destructor>(ctoiitemiter_dealloc), /* tp_dealloc */
  0,                                          /* tp_print */
  0,                                          /* tp_getattr */
  0,                                          /* tp_setattr */
  0,                                          /* tp_compare or *tp_reserved */
  0,                                          /* tp_repr */
  0,                                          /* tp_as_number */
  0,                                          /* tp_as_sequence */
  0,                                          /* tp_as_mapping */
  0,                                          /* tp_hash */
  0,                                          /* tp_call */
  0,                                          /* tp_str */
  PyObject_GenericGetAttr,                    /* tp_getattro */
  0,                                          /* tp_setattro */
  0,                                          /* tp_as_buffer */
#if PY_MAJOR_VERSION >= 3
  Py_TPFLAGS_DEFAULT | Py_TPFLAGS_BASETYPE , /* tp_flags */
#else
  Py_TPFLAGS_DEFAULT | Py_TPFLAGS_BASETYPE | Py_TPFLAGS_HAVE_ITER, /* tp_flags */
#endif
  0,                                          /* tp_doc */
  0,                                          /* tp_traverse */
  0,                                          /* tp_clear */
  0,                                          /* tp_richcompare */
  0,                                          /* tp_weaklistoffset */
  PyObject_SelfIter,                          /* tp_iter */
  reinterpret_cast<iternextfunc>(ctoiitemiter_next), /* tp_iternext */
  setsetiter_methods,                         /* tp_methods */
  0,                                          /* tp_members */
  0,                                          /* tp_getset */
  0,                                          /* tp_base */
  0,                                          /* tp_dict */
  0,                                          /* tp_descr_get */
  0,                                          /* tp_descr_set */
  0,                                          /* tp_dictoffset */
  0,                                          /* tp_init */
  PyType_GenericAlloc,                        /* tp_alloc */
  ctoiiter_new,                             /* tp_new */
#if PY_MAJOR_VERSION >= 3
  0, /* tp_free */
  0, /* tp_is_gc */
  0, /* *tp_bases */
  0, /* *tp_mro */
  0, /* *tp_cache */
  0, /* *tp_subclasses */
  0, /* *tp_weaklist */
  0, /* tp_version_tag */
  0, /* tp_finalize */
#endif
};

static PyObject* vsop_iter(PyObject* self) {
  PyCtoIIterObject* ssi = PyObject_New(PyCtoIIterObject, &PyCtoIIter_Type);
  if (ssi == NULL) return NULL;
  ssi->it = new VsopEach(reinterpret_cast<PyCtoIObject*>(self)->ss);
  if (ssi->it == NULL) {
    PyErr_NoMemory();
    return NULL;
  }
  return reinterpret_cast<PyObject*>(ssi);
}

static PyObject* vsop_itemiter(PyObject* self) {
  PyCtoI_ItemIterObject* ssi = PyObject_New(PyCtoI_ItemIterObject, &PyCtoI_ItemIter_Type);
  if (ssi == NULL) return NULL;
  ssi->it = new VsopItemEach(reinterpret_cast<PyCtoIObject*>(self)->ss);
  if (ssi->it == NULL) {
    PyErr_NoMemory();
    return NULL;
  }
  return reinterpret_cast<PyObject*>(ssi);
}




static PyObject* vsop_lcm_nomal(char *tp,char *fname,int th,char *order=NULL){

	CtoI *ctoi_fin; 

	if(init_cnt==0){ BDDV_Init(256, env_nmax);}
	init_cnt++;
	if(strcmp(tp, "F") == 0) CtoI_Lcm1(fname, order, th, 0);
	else if(strcmp(tp, "C") == 0) CtoI_Lcm1(fname, order, th, 1);
	else if(strcmp(tp, "M") == 0) CtoI_Lcm1(fname, order, th, 2);
	else if(strcmp(tp, "FQ") == 0) CtoI_Lcm1(fname, order, th, 10);
	else if(strcmp(tp, "CQ") == 0) CtoI_Lcm1(fname, order, th, 11);
	else if(strcmp(tp, "MQ") == 0) CtoI_Lcm1(fname, order, th, 12);
	else { yyerror("type ERROR\n"); return NULL;}
	for(int i=VTable.Used(); i<CtoI_LcmItems(); i++){
		char s[32];
		int x = CtoI_LcmPerm(i);
		sprintf(s, "x%d", i+1);
		VTable.SetT(s, power16/2);
  }
  CtoI a = CtoI_Lcm2();
  ctoi_fin = new CtoI(a);

  PyCtoIObject* ret = reinterpret_cast<PyCtoIObject*>(
      PyCtoI_Type.tp_alloc(&PyCtoI_Type, 0));
	ret->ss = ctoi_fin;
	return  reinterpret_cast<PyObject*>(ret);

}



static PyObject* vsop_lcm_ub(char *tp,char *fname,int th,int ub,char *order=NULL)
{
	if(init_cnt==0){ BDDV_Init(256, env_nmax);}
	init_cnt++;

	CtoI *ctoi_fin; 
	if(strcmp(tp, "F" ) == 0) CtoI_Lcm1_ub(fname, order, th,  0, ub);
	else if(strcmp(tp, "C" ) == 0) CtoI_Lcm1_ub(fname, order, th,  1, ub);
	else if(strcmp(tp, "M" ) == 0) CtoI_Lcm1_ub(fname, order, th,  2, ub);
	else if(strcmp(tp, "FQ") == 0) CtoI_Lcm1_ub(fname, order, th, 10, ub);
	else if(strcmp(tp, "CQ") == 0) CtoI_Lcm1_ub(fname, order, th, 11, ub);
	else if(strcmp(tp, "MQ") == 0) CtoI_Lcm1_ub(fname, order, th, 12, ub);
	else { yyerror("type ERROR\n"); return NULL;}

  for(int i=VTable.Used(); i<CtoI_LcmItems(); i++){
		int t = 1;
		char s[32];
		int x = CtoI_LcmPerm(i);
		sprintf(s, "x%d", x);
		while(VTable.GetID(s) != 0)
		{
	  	t++;
	  	sprintf(s, "x%d_%d", x, t);
		}
		VTable.SetT(s, power16/2);
  }
  
  CtoI a = CtoI_Lcm2();
  ctoi_fin = new CtoI(a);

  PyCtoIObject* ret = reinterpret_cast<PyCtoIObject*>(
      PyCtoI_Type.tp_alloc(&PyCtoI_Type, 0));
	ret->ss = ctoi_fin;
	return  reinterpret_cast<PyObject*>(ret);
}



/*##vsop_lcm##*/
static PyObject* vsop_lcm(PyCtoIObject* self, PyObject* args)
{

	char *tp=NULL;
	char *tfname=NULL;
	int minsup=0;
	char *order=NULL;
	int ub=0;
	PyObject* rtnval ; 

  if (!PyArg_ParseTuple(args, "ssi|si", &tp, &tfname,&minsup, &order,&ub)) return NULL;

	int argc = PyTuple_Size(args);
	if(argc==5){
		if(*order=='\0'){ order = NULL;}
		rtnval = vsop_lcm_ub(tp,tfname,minsup,ub,order);
	}
	else{
		rtnval = vsop_lcm_nomal(tp,tfname,minsup,order);
	}
	return rtnval;
}

/*##vsop_import##*/
PyObject* vsop_import(PyCtoIObject* self, PyObject* args){

	char *fname;
  if (!PyArg_ParseTuple(args, "s", &fname)) return NULL;

	if(init_cnt==0){ BDDV_Init(256, env_nmax);}
	init_cnt++;

	CtoI *ctoi_fin; 
  FILE *fp;

  fp = fopen(fname, "r");
	if(fp == NULL){
		yyerror("Can't open the file");
		ctoi_fin = new CtoI(0);
	}
	else{
		ZBDDV v = ZBDDV_Import(fp);
		fclose(fp);
		if(v != ZBDDV(-1)){
		  int t = 1;
		  char s[32];
			while(BDD_LevOfVar(v.Top()) > VTable.Used()){
		    sprintf(s, "x%d", t);
		    while(VTable.GetID(s) != 0){
					t++;
					sprintf(s, "x%d", t);
				}
				VTable.SetT0(BDD_VarOfLev(t), s);
			}
			int len = v.Last()+1;
			CtoI a = CtoI(0);
			for(int i=0; i<len; i++){
				a += CtoI(v.GetZBDD(i)).ShiftDigit(i);
			}
      ctoi_fin = new CtoI(a);
		}
		else{
			yyerror("<WARNING> import error");
			ctoi_fin = new CtoI(0);
		}
	}

  PyCtoIObject* ret = reinterpret_cast<PyCtoIObject*>(
      PyCtoI_Type.tp_alloc(&PyCtoI_Type, 0));
	ret->ss = ctoi_fin;
	return reinterpret_cast<PyObject*>(ret);
}






static PyMethodDef ctoi_methods[] = {
	{"itemset", reinterpret_cast<PyCFunction>(vsop_itemset), METH_VARARGS },
	{"symbol", reinterpret_cast<PyCFunction>(vsop_symbol), METH_VARARGS },
	{"constant", reinterpret_cast<PyCFunction>(vsop_constant), METH_VARARGS },
	{"totalsize", reinterpret_cast<PyCFunction>(vsop_totalsize), METH_VARARGS },
	{"zddread",reinterpret_cast<PyCFunction>(vsop_import),METH_VARARGS,""},
  {"lcm",reinterpret_cast<PyCFunction>(vsop_lcm), METH_VARARGS, ""},
	{NULL}
};



static void ctoi_dealloc(PyCtoIObject* self) {
  delete self->ss;
  Py_TYPE(self)->tp_free(reinterpret_cast<PyObject*>(self));
}


static int ctoi_init(PyCtoIObject* self, PyObject* args, PyObject* kwds) {
  PyObject* obj = NULL;
  if (!PyArg_ParseTuple(args, "|O", &obj))
    return -1;
  if (obj == NULL || obj == Py_None) {
    self->ss = new CtoI();
  }
  else if(numCHECK(obj)){
    self->ss = new CtoI(PyLong_AsLong(obj));
  }
  
  if (PyErr_Occurred()){
    return -1;
  }
  return 0;
}


static PyObject* ctoi_new(PyTypeObject* type, PyObject* args, PyObject* kwds) {
  PyCtoIObject* self;
  self = reinterpret_cast<PyCtoIObject*>(type->tp_alloc(type, 0));
  if (self == NULL) return NULL;
  return reinterpret_cast<PyObject*>(self);
}
//static PyMemberDef ctoi_members[] = {
// {NULL}  /* Sentinel */
//};

// enable arg1
//	{"hex","bit","case","map","rmap","decomp","export",""};
static void vsop_print_arg(PyCtoIObject* self,char *arg1,char *arg2){

	CtoI *ctoi_modd =new CtoI(*(self->ss));
	int rtnFLG= 0;
	if(*ctoi_modd == CtoI_Null()){
  	*ctoi_modd = 0;
		yyerror("<WARNING> Memory overflow");
		return;
	}
	else if(!strcmp(arg1,"hex")){ rtnFLG = PrintCtoI_16(*ctoi_modd); }
	else if(!strcmp(arg1,"bit")){ rtnFLG = PrintDigital(*ctoi_modd);}
	else if(!strcmp(arg1,"case")){ rtnFLG = PrintCase(*ctoi_modd);}
	else if(!strcmp(arg1,"map")) { rtnFLG = MapAll(*ctoi_modd);}
	else if(!strcmp(arg1,"rmap")){ rtnFLG = MapSel(*ctoi_modd);}
	else if(!strcmp(arg1,"decomp")){ rtnFLG = PrintDecomp(*ctoi_modd);}
	else if(!strcmp(arg1,"export")){
		int d = ctoi_modd -> TopDigit();
		ZBDDV v = ZBDDV();
		for(int i=0; i<=d; i++){
	  	v += ZBDDV(ctoi_modd -> Digit(i).GetZBDD(), i);
		}
		FILE *fp=NULL;	
		if(arg2){
			fp = fopen(arg2, "w");
			if(fp == NULL) yyerror("Can't open the file");
		}
		if(fp){
 			v.Export(fp);
		  fclose(fp);
 		}else{
			v.Export();	
		}
	}
	else { yyerror("Illegal switch\n");}

	if(ctoi_modd){ delete ctoi_modd;}
	
	return;

}


static PyObject* vsop_print(PyCtoIObject* self){

	CtoI *ctoi_modb =new CtoI(*(self->ss));

	if(*ctoi_modb == CtoI_Null()){

		*ctoi_modb = 0;
		yyerror("<WARNING> Memory overflow\n");
		Py_RETURN_TRUE;
	}
  if(PrintCtoI(*ctoi_modb) == 1){

		yyerror("<WARNING> Memory overflow\n");
		Py_RETURN_TRUE;
	}

	delete ctoi_modb;
	Py_RETURN_TRUE;
}


CtoI *value2ctoi(PyObject* v)
{
	 CtoI *rtnctoi=NULL;

	if(Py_TYPE(v) == &PyCtoI_Type){
		rtnctoi = new CtoI(*(reinterpret_cast<PyCtoIObject*>(v)->ss));
	}
	else if(strCHECK(v)){
		rtnctoi=string2ctoi(strGET(v));
	}
	else if(numCHECK(v)){
		rtnctoi=int2ctoi(PyLong_AsLong(v));
	}
	else if(v==Py_None){
		yyerror("None err1\n");
	}
	else{
		yyerror("Unknow type err\n");
	}
	return rtnctoi; 
}

/*##vsop_minus_op##*/
static PyObject* vsop_minus_op(PyCtoIObject* self){

	CtoI *ctoi_modb =new CtoI(*(self->ss));
	CtoI *ctoi_fin; 
	*ctoi_modb = - *ctoi_modb;
  ctoi_fin = ctoi_modb;

  PyCtoIObject* ret = reinterpret_cast<PyCtoIObject*>(
      PyCtoI_Type.tp_alloc(&PyCtoI_Type, 0));
	ret->ss = ctoi_fin;
	return  reinterpret_cast<PyObject*>(ret);
}

/*##vsop_plus_op##*/
static PyObject* vsop_plus_op(PyCtoIObject* self)
{

	CtoI *ctoi_modb =new CtoI(*(self->ss));
	CtoI *ctoi_fin; 
	ctoi_fin = ctoi_modb;

  PyCtoIObject* ret = reinterpret_cast<PyCtoIObject*>(
      PyCtoI_Type.tp_alloc(&PyCtoI_Type, 0));
	ret->ss = ctoi_fin;
	return  reinterpret_cast<PyObject*>(ret);
}

/*##vsop_plus##*/
static PyObject* vsop_plus(PyObject* self, PyObject* other)
{
	PyCtoIObject* v1;
	PyObject* v2;
	if( Py_TYPE(self) == &PyCtoI_Type ){
		v1 = reinterpret_cast<PyCtoIObject*>(self);
		v2 = reinterpret_cast<PyObject*>(other);
	}
	else if(Py_TYPE(other) == &PyCtoI_Type){
		v1 = reinterpret_cast<PyCtoIObject*>(other);
		v2 = reinterpret_cast<PyObject*>(self);
	}
	else{
  	yyerror("type ERROR");
		return NULL;	
	}
	CtoI *ctoi_moda = new CtoI(*v1->ss);
	CtoI *ctoi_modc =	value2ctoi(v2);
	CtoI *ctoi_fin;
	*ctoi_moda = *ctoi_moda + *ctoi_modc;
  ctoi_fin = ctoi_moda;
  delete ctoi_modc;

  PyCtoIObject* ret = reinterpret_cast<PyCtoIObject*>(
      PyCtoI_Type.tp_alloc(&PyCtoI_Type, 0));
	ret->ss = ctoi_fin;
	return  reinterpret_cast<PyObject*>(ret);
}

/*##vsop_minus##*/
static PyObject* vsop_minus(PyObject* self, PyObject* other)
{
	PyCtoIObject* v1;
	PyObject* v2;
	if( Py_TYPE(self) == &PyCtoI_Type ){
		v1 = reinterpret_cast<PyCtoIObject*>(self);
		v2 = reinterpret_cast<PyObject*>(other);
	}
	else if(Py_TYPE(other) == &PyCtoI_Type){
		v1 = reinterpret_cast<PyCtoIObject*>(other);
		v2 = reinterpret_cast<PyObject*>(self);
	}
	else{
  	yyerror("type ERROR");
		return NULL;	
	}

	CtoI *ctoi_moda = new CtoI(*v1->ss);
	CtoI *ctoi_modc =	value2ctoi(v2);
	CtoI *ctoi_fin; 
	*ctoi_moda = *ctoi_moda - *ctoi_modc;
  ctoi_fin = ctoi_moda;
  delete ctoi_modc;

  PyCtoIObject* ret = reinterpret_cast<PyCtoIObject*>(
      PyCtoI_Type.tp_alloc(&PyCtoI_Type, 0));
	ret->ss = ctoi_fin;
	return  reinterpret_cast<PyObject*>(ret);
}

/*##vsop_multiply##*/
static PyObject* vsop_multiply(PyObject* self, PyObject* other) {

	PyCtoIObject* v1;
	PyObject* v2;
	if( Py_TYPE(self) == &PyCtoI_Type ){
		v1 = reinterpret_cast<PyCtoIObject*>(self);
		v2 = reinterpret_cast<PyObject*>(other);
	}
	else if(Py_TYPE(other) == &PyCtoI_Type){
		v1 = reinterpret_cast<PyCtoIObject*>(other);
		v2 = reinterpret_cast<PyObject*>(self);
	}
	else{
  	yyerror("type ERROR");
		return NULL;	
	}

	CtoI *ctoi_moda = new CtoI(*v1->ss);
	CtoI *ctoi_modc =	value2ctoi(v2);
	CtoI *ctoi_fin; 
	*ctoi_moda = Product(*ctoi_moda, *ctoi_modc);
  ctoi_fin = ctoi_moda;
  delete ctoi_modc;

  PyCtoIObject* ret = reinterpret_cast<PyCtoIObject*>(
      PyCtoI_Type.tp_alloc(&PyCtoI_Type, 0));
	ret->ss = ctoi_fin;
	return  reinterpret_cast<PyObject*>(ret);
}

/*##vsop_quotiment##*/
static PyObject* vsop_quotiment(PyObject* self, PyObject* other) {

	PyCtoIObject* v1;
	PyObject* v2;
	if( Py_TYPE(self) == &PyCtoI_Type ){
		v1 = reinterpret_cast<PyCtoIObject*>(self);
		v2 = reinterpret_cast<PyObject*>(other);
	}
	else if(Py_TYPE(other) == &PyCtoI_Type){
		v1 = reinterpret_cast<PyCtoIObject*>(other);
		v2 = reinterpret_cast<PyObject*>(self);
	}
	else{
  	yyerror("type ERROR");
		return NULL;	
	}

	CtoI *ctoi_moda = new CtoI(*v1->ss);
	CtoI *ctoi_modc =	value2ctoi(v2);
	CtoI *ctoi_fin; 
	if(*ctoi_modc == 0)
  {
  	yyerror("<WARNING> Divide by zero");
		*ctoi_moda = 0;
	}
  else{
  	*ctoi_moda = *ctoi_moda / *ctoi_modc;
  }
  ctoi_fin = ctoi_moda;
  delete ctoi_modc;

  PyCtoIObject* ret = reinterpret_cast<PyCtoIObject*>(
      PyCtoI_Type.tp_alloc(&PyCtoI_Type, 0));
	ret->ss = ctoi_fin;
	return  reinterpret_cast<PyObject*>(ret);
}

/*##vsop_remainder##*/
static PyObject* vsop_remainder(PyObject* self, PyObject* other)
{
	PyCtoIObject* v1;
	PyObject* v2;
	if( Py_TYPE(self) == &PyCtoI_Type ){
		v1 = reinterpret_cast<PyCtoIObject*>(self);
		v2 = reinterpret_cast<PyObject*>(other);
	}
	else if(Py_TYPE(other) == &PyCtoI_Type){
		v1 = reinterpret_cast<PyCtoIObject*>(other);
		v2 = reinterpret_cast<PyObject*>(self);
	}
	else{
  	yyerror("type ERROR");
		return NULL;	
	}

	CtoI *ctoi_moda = new CtoI(*v1->ss);
	CtoI *ctoi_modc =	value2ctoi(v2);
	CtoI *ctoi_fin; 

	if(*ctoi_modc == 0){
  	yyerror("<WARNING> Divide by zero");
		*ctoi_moda = 0;
	}
	else{
		*ctoi_moda = *ctoi_moda % *ctoi_modc;
	}
	ctoi_fin = ctoi_moda;
  delete ctoi_modc;

  PyCtoIObject* ret = reinterpret_cast<PyCtoIObject*>(
      PyCtoI_Type.tp_alloc(&PyCtoI_Type, 0));
	ret->ss = ctoi_fin;
	return  reinterpret_cast<PyObject*>(ret);
}



/*##vsop_restrict##*/
static PyObject* vsop_restrict(PyCtoIObject* self, PyObject* args){

  PyObject* v1 = NULL;
  if (!PyArg_ParseTuple(args, "O", &v1)) return NULL;

	CtoI *ctoi_moda =new CtoI(*self->ss);
	CtoI *ctoi_modd = value2ctoi(v1);
	CtoI *ctoi_fin; 
	*ctoi_moda = ctoi_moda -> FilterRestrict(*ctoi_modd);
  ctoi_fin = ctoi_moda;
  delete ctoi_modd;

  PyCtoIObject* ret = reinterpret_cast<PyCtoIObject*>(
      PyCtoI_Type.tp_alloc(&PyCtoI_Type, 0));
	ret->ss = ctoi_fin;
	return  reinterpret_cast<PyObject*>(ret);

}
/*##vsop_permit##*/
PyObject* vsop_permit(PyCtoIObject* self, PyObject* args){
	
  PyObject* v = NULL;
  if (!PyArg_ParseTuple(args, "O", &v)) return NULL;
	CtoI *ctoi_moda = new CtoI(*self->ss);
	CtoI *ctoi_modd = value2ctoi(v);
	CtoI *ctoi_fin; 
	*ctoi_moda = ctoi_moda -> FilterPermit(*ctoi_modd);
  ctoi_fin = ctoi_moda;
  delete ctoi_modd;

  PyCtoIObject* ret = reinterpret_cast<PyCtoIObject*>(
      PyCtoI_Type.tp_alloc(&PyCtoI_Type, 0));
	ret->ss = ctoi_fin;
	return  reinterpret_cast<PyObject*>(ret);
}
/*##vsop_permitsym##*/
PyObject* vsop_permitsym(PyCtoIObject* self, PyObject* args){

  PyObject* v = NULL;
  if (!PyArg_ParseTuple(args, "O", &v)) return NULL;
	CtoI *ctoi_moda = new CtoI(*self->ss);
	CtoI *ctoi_modd = value2ctoi(v);
	CtoI *ctoi_fin; 
	*ctoi_moda = ctoi_moda -> FilterPermitSym(ctoi_modd->GetInt());
	ctoi_fin = ctoi_moda;
	delete ctoi_modd;

  PyCtoIObject* ret = reinterpret_cast<PyCtoIObject*>(
      PyCtoI_Type.tp_alloc(&PyCtoI_Type, 0));
	ret->ss = ctoi_fin;
	return  reinterpret_cast<PyObject*>(ret);
}

/*##vsop_items##*/

PyObject* vsop_items(PyCtoIObject* self){

	CtoI *ctoi_moda =new CtoI(*self->ss);
	CtoI *ctoi_fin; 
	*ctoi_moda = ctoi_moda -> TotalValItems();
  ctoi_fin = ctoi_moda;

  PyCtoIObject* ret = reinterpret_cast<PyCtoIObject*>(
      PyCtoI_Type.tp_alloc(&PyCtoI_Type, 0));
	ret->ss = ctoi_fin;
	return  reinterpret_cast<PyObject*>(ret);
}

/*##vsop_symgrp##*/
PyObject* vsop_symgrp(PyCtoIObject* self){

	CtoI *ctoi_moda =new CtoI(*self->ss);
	CtoI *ctoi_fin; 
	*ctoi_moda = ctoi_moda -> NonZero().GetZBDD().SymGrp();

  ctoi_fin = ctoi_moda;

  PyCtoIObject* ret = reinterpret_cast<PyCtoIObject*>(
      PyCtoI_Type.tp_alloc(&PyCtoI_Type, 0));
	ret->ss = ctoi_fin;
	return  reinterpret_cast<PyObject*>(ret);

}

/*##vsop_freqpatA##*/
PyObject* vsop_freqpatA(PyCtoIObject* self, PyObject* args){

  PyObject* v = NULL;
  if (!PyArg_ParseTuple(args, "O", &v)) return NULL;
	CtoI *ctoi_moda = new CtoI(*self->ss);
	CtoI *ctoi_modd = value2ctoi(v);
	CtoI *ctoi_fin; 
	*ctoi_moda = ctoi_moda -> FreqPatA(ctoi_modd->GetInt() );
  ctoi_fin = ctoi_moda;
  delete ctoi_modd;

  PyCtoIObject* ret = reinterpret_cast<PyCtoIObject*>(
      PyCtoI_Type.tp_alloc(&PyCtoI_Type, 0));
	ret->ss = ctoi_fin;
	return  reinterpret_cast<PyObject*>(ret);
}

/*##vsop_freqpatM##*/
PyObject* vsop_freqpatM(PyCtoIObject* self, PyObject* args){

  PyObject* v = NULL;
  if (!PyArg_ParseTuple(args, "O", &v)) return NULL;
	CtoI *ctoi_moda = new CtoI(*self->ss);
	CtoI *ctoi_modd = value2ctoi(v);
	CtoI *ctoi_fin; 
	*ctoi_moda = ctoi_moda -> FreqPatM(ctoi_modd->GetInt() );
  ctoi_fin = ctoi_moda;
  delete ctoi_modd;

  PyCtoIObject* ret = reinterpret_cast<PyCtoIObject*>(
      PyCtoI_Type.tp_alloc(&PyCtoI_Type, 0));
	ret->ss = ctoi_fin;
	return  reinterpret_cast<PyObject*>(ret);
}

/*##vsop_freqpatC##*/
PyObject* vsop_freqpatC(PyCtoIObject* self, PyObject* args){

  PyObject* v = NULL;
  if (!PyArg_ParseTuple(args, "O", &v)) return NULL;

	CtoI *ctoi_moda = new CtoI(*self->ss);
	CtoI *ctoi_modd = value2ctoi(v);
	CtoI *ctoi_fin; 
	*ctoi_moda = ctoi_moda -> FreqPatC(ctoi_modd->GetInt() );
	ctoi_fin = ctoi_moda;
	delete ctoi_modd;

  PyCtoIObject* ret = reinterpret_cast<PyCtoIObject*>(
      PyCtoI_Type.tp_alloc(&PyCtoI_Type, 0));
	ret->ss = ctoi_fin;
	return  reinterpret_cast<PyObject*>(ret);
}

/*##vsop_termsEQ##*/
PyObject* vsop_termsEQ(PyCtoIObject* self, PyObject* args){

  PyObject* v = NULL;
  if (!PyArg_ParseTuple(args, "O", &v)) return NULL;

	CtoI *ctoi_moda = new CtoI(*self->ss);
	CtoI *ctoi_modd = value2ctoi(v);
	CtoI *ctoi_fin; 
	*ctoi_moda = ctoi_moda -> FilterThen(ctoi_moda -> EQ_Const(ctoi_modd->GetInt()));
  ctoi_fin = ctoi_moda;
  delete ctoi_modd;

  PyCtoIObject* ret = reinterpret_cast<PyCtoIObject*>(
      PyCtoI_Type.tp_alloc(&PyCtoI_Type, 0));
	ret->ss = ctoi_fin;
	return  reinterpret_cast<PyObject*>(ret);
}

/*##vsop_termsNE##*/
PyObject* vsop_termsNE(PyCtoIObject* self, PyObject* args){

  PyObject* v = NULL;
  if (!PyArg_ParseTuple(args, "O", &v)) return NULL;

	CtoI *ctoi_moda = new CtoI(*self->ss);
	CtoI *ctoi_modd = value2ctoi(v);
	CtoI *ctoi_fin; 
	*ctoi_moda = ctoi_moda -> FilterThen(ctoi_moda -> NE_Const(ctoi_modd->GetInt()));
  ctoi_fin = ctoi_moda;
  delete ctoi_modd;

  PyCtoIObject* ret = reinterpret_cast<PyCtoIObject*>(
      PyCtoI_Type.tp_alloc(&PyCtoI_Type, 0));
	ret->ss = ctoi_fin;
	return  reinterpret_cast<PyObject*>(ret);
}

/*##vsop_termsGT##*/
PyObject* vsop_termsGT(PyCtoIObject* self, PyObject* args){

  PyObject* v = NULL;
  if (!PyArg_ParseTuple(args, "O", &v)) return NULL;

	CtoI *ctoi_moda = new CtoI(*self->ss);
	CtoI *ctoi_modd = value2ctoi(v);
	CtoI *ctoi_fin; 
	*ctoi_moda = ctoi_moda -> FilterThen(ctoi_moda -> GT_Const(ctoi_modd->GetInt()));
  ctoi_fin = ctoi_moda;

  PyCtoIObject* ret = reinterpret_cast<PyCtoIObject*>(
      PyCtoI_Type.tp_alloc(&PyCtoI_Type, 0));
	ret->ss = ctoi_fin;
	return  reinterpret_cast<PyObject*>(ret);

}

/*##vsop_termsGE##*/
PyObject* vsop_termsGE(PyCtoIObject* self, PyObject* args){

  PyObject* v = NULL;
  if (!PyArg_ParseTuple(args, "O", &v)) return NULL;

	CtoI *ctoi_moda = new CtoI(*self->ss);
	CtoI *ctoi_modd = value2ctoi(v);
	CtoI *ctoi_fin; 
	*ctoi_moda = ctoi_moda -> FilterThen(ctoi_moda -> GE_Const(ctoi_modd->GetInt()));
  ctoi_fin = ctoi_moda;
  delete ctoi_modd;

  PyCtoIObject* ret = reinterpret_cast<PyCtoIObject*>(
      PyCtoI_Type.tp_alloc(&PyCtoI_Type, 0));
	ret->ss = ctoi_fin;
	return  reinterpret_cast<PyObject*>(ret);

}

/*##vsop_termsLT##*/
PyObject* vsop_termsLT(PyCtoIObject* self, PyObject* args){

  PyObject* v = NULL;
  if (!PyArg_ParseTuple(args, "O", &v)) return NULL;

	CtoI *ctoi_moda = new CtoI(*self->ss);
	CtoI *ctoi_modd = value2ctoi(v);
	CtoI *ctoi_fin; 
	*ctoi_moda = ctoi_moda -> FilterThen(ctoi_moda -> LT_Const(ctoi_modd->GetInt()));
  ctoi_fin = ctoi_moda;
  delete ctoi_modd;

  PyCtoIObject* ret = reinterpret_cast<PyCtoIObject*>(
      PyCtoI_Type.tp_alloc(&PyCtoI_Type, 0));
	ret->ss = ctoi_fin;
	return  reinterpret_cast<PyObject*>(ret);

}

/*##vsop_termsLE##*/
PyObject* vsop_termsLE(PyCtoIObject* self, PyObject* args){

  PyObject* v = NULL;
  if (!PyArg_ParseTuple(args, "O", &v)) return NULL;

	CtoI *ctoi_moda = new CtoI(*self->ss);
	CtoI *ctoi_modd = value2ctoi(v);
	CtoI *ctoi_fin; 
	*ctoi_moda = ctoi_moda -> FilterThen(ctoi_moda -> LE_Const(ctoi_modd->GetInt()));
  ctoi_fin = ctoi_moda;
  delete ctoi_modd;

  PyCtoIObject* ret = reinterpret_cast<PyCtoIObject*>(
      PyCtoI_Type.tp_alloc(&PyCtoI_Type, 0));
	ret->ss = ctoi_fin;
	return  reinterpret_cast<PyObject*>(ret);
}

/*##vsop_meet##*/
PyObject* vsop_meet(PyCtoIObject* self, PyObject* args){

  PyObject* v = NULL;
  if (!PyArg_ParseTuple(args, "O", &v)) return NULL;

	CtoI *ctoi_moda = new CtoI(*self->ss);
	CtoI *ctoi_modc = value2ctoi(v);
	CtoI *ctoi_fin; 
	*ctoi_moda = CtoI_Meet(*ctoi_moda, *ctoi_modc);
  ctoi_fin = ctoi_moda;
  delete ctoi_modc;

  PyCtoIObject* ret = reinterpret_cast<PyCtoIObject*>(
      PyCtoI_Type.tp_alloc(&PyCtoI_Type, 0));
	ret->ss = ctoi_fin;
	return  reinterpret_cast<PyObject*>(ret);
}

/*##vsop_delta##*/
PyObject* vsop_delta(PyCtoIObject* self, PyObject* args){

  PyObject* v = NULL;
  if (!PyArg_ParseTuple(args, "O", &v)) return NULL;

	CtoI *ctoi_moda = new CtoI(*self->ss);
	CtoI *ctoi_modc = value2ctoi(v);
	CtoI *ctoi_fin; 
	*ctoi_moda = CtoI_Delta(*ctoi_moda, *ctoi_modc);
  ctoi_fin = ctoi_moda;
  delete ctoi_modc;

  PyCtoIObject* ret = reinterpret_cast<PyCtoIObject*>(
      PyCtoI_Type.tp_alloc(&PyCtoI_Type, 0));
	ret->ss = ctoi_fin;
	return  reinterpret_cast<PyObject*>(ret);
}


/*##vsop_iif##*/
static PyObject* vsop_iif(PyCtoIObject* self, PyObject* args){

  PyObject* v1 = NULL;
  PyObject* v2 = NULL;
  if (!PyArg_ParseTuple(args, "OO", &v1,&v2)) return NULL;

	CtoI *ctoi_moda =new CtoI(*self->ss);
	CtoI *ctoi_modc = value2ctoi(v1);
	CtoI *ctoi_mode = value2ctoi(v2);
	CtoI *ctoi_fin; 
	*ctoi_moda = CtoI_ITE(*ctoi_moda, *ctoi_modc, *ctoi_mode);
  ctoi_fin = ctoi_moda;
  delete ctoi_modc;
  delete ctoi_mode;
  PyCtoIObject* ret = reinterpret_cast<PyCtoIObject*>(
      PyCtoI_Type.tp_alloc(&PyCtoI_Type, 0));
	ret->ss = ctoi_fin;
	return  reinterpret_cast<PyObject*>(ret);

}

/*##vsop_same##*/
PyObject* vsop_same(PyCtoIObject* self, PyObject* args){

  PyObject* v = NULL;
  if (!PyArg_ParseTuple(args, "O", &v)) return NULL;

	CtoI *ctoi_modc = value2ctoi(v);
	int flg= (*(self->ss) == *ctoi_modc);
	delete ctoi_modc;
	if(flg){
		Py_RETURN_TRUE;
	} 
	else{
		Py_RETURN_FALSE;
	}
}

/*##vsop_diff##*/
PyObject* vsop_diff(PyCtoIObject* self, PyObject* args){

  PyObject* v = NULL;
  if (!PyArg_ParseTuple(args, "O", &v)) return NULL;
	CtoI *ctoi_modc = value2ctoi(v);
	int flg= (*(self->ss) != *ctoi_modc);
	delete ctoi_modc;
	if (flg){
		Py_RETURN_TRUE;
	} 
	else{
		Py_RETURN_FALSE;
	}
}


#define MAX_LEN 409600
#define HASH_MAX 4096000

static int* S_Var;
static int  PFflag;
static int  Depth;
static int  hashcnt;


//----------------------- vsop_hashout -------------------------------
static int PF_hash(CtoI a, PyObject* hash)
{
	if(a.IsConst())
	{
		char *valstr = new char[MAX_LEN];
		if(a.TopDigit() & 1) {strcpy(valstr,"-") ; a = -a; }
		else{ strcpy(valstr,"");}

		PFflag = 1;
		int c1 = (a != 1);
		if(c1 || Depth == 0)
		{
			if(a.TopItem() > 0) a = a.MaxVal();
			int d = a.TopDigit() / 3 + 14;
			char *valrtn = new char[d];
			int err = a.StrNum10(valrtn);
			if(err == 1)
			{
				yyerror("memory over flow");
				return 1;
			}
			strcat(valstr,valrtn);
			delete [] valrtn;
		}
		else if(!c1){
			strcat(valstr,"1");
		}
		char *keystr = new char[MAX_LEN];
		*keystr = '\0';
		for(int i=0; i<Depth; i++)
		{
			int size = strlen(keystr)+strlen(VTable.GetName(S_Var[i]))+2;
			if(size>MAX_LEN){
				yyerror("string size over flow");
			}
			if(i==0){
				strcpy(keystr,VTable.GetName(S_Var[i]));
			}
			else{
				strcat(keystr," ");
				strcat(keystr,VTable.GetName(S_Var[i]));
			}
		}
		PyDict_SetItemString(hash , keystr,PyLong_FromString(valstr,NULL,0));
		hashcnt++;
		if(hashcnt> HASH_MAX){return 2;}
		return 0;
	}
	int v = a.TopItem();
	CtoI b = a.Factor1(v);
	if(b == CtoI_Null()) return 1;
	S_Var[Depth] = v;
	Depth++;
	int chk=PF_hash(b, hash);
	if(chk > 0) return chk;
	Depth--;
	b = a.Factor0(v);
	if(b == 0) return 0;
	if(b == CtoI_Null()) return 1;

	return PF_hash(b,hash);
}

static int HashoutCtoI(CtoI a,PyObject* hash)
{
	int rtn=0;
	hashcnt=0;
	if(a == CtoI_Null()) return 1;
	if(a == 0) return 0;
	else
	{
		int lev = BDD_LevOfVar(a.TopItem());
		Depth = 0;
		S_Var = new int[lev];
		PFflag = 0;
		rtn = PF_hash(a,hash);
		if(rtn == 1){ 
			delete [] S_Var;
			return rtn; 
		}
	}
	delete [] S_Var;
	return rtn;
}
/*##vsop_hashout##*/
PyObject* vsop_hashout(PyCtoIObject* self){

	PyObject* hash = PyDict_New();
	int rtnflg = HashoutCtoI(*self->ss,hash);

	if(rtnflg==2){
	  Py_INCREF(Py_True);
 		py_partly = Py_True; 
	}
	else{ 
	  Py_INCREF(Py_False);
		py_partly =  Py_False;	
	}
	return hash;
}

//----------------------- vsop_hashout -------------------------------









//----------------------- vsop_csvout -------------------------------
static int PutNum(CtoI a, int base,ofstream &fs)
{
	if(a.TopItem() > 0) a = a.MaxVal();
	int d = a.TopDigit() / 3 + 14;
	char *s =new char[d];
	int err;
	if(base == 16) err = a.StrNum16(s);
	else err = a.StrNum10(s);
	if(err == 1)		return 1;
	fs << s<< ",";
	delete [] s;
	return 0;
}

static int PF(CtoI a, int base,ofstream &fs)
{
	if(a.IsConst())
	{
		if(PFflag == 1){fs << endl;}
		if(a.TopDigit() & 1) {fs << "-" ; a = -a; }
		
		PFflag = 1;
		int c1 = (a != 1);
		if(c1 || Depth == 0)
		{
			if(PutNum(a, base,fs) == 1) return 1;
		}
		else if(!c1){
			fs <<  "1,";
		}
		for(int i=0; i<Depth; i++)
		{
			if(i==0){ fs        << VTable.GetName(S_Var[i]);}
			else    { fs << " " << VTable.GetName(S_Var[i]);}
		}
		return 0;
	}
	int v = a.TopItem();
	CtoI b = a.Factor1(v);
	if(b == CtoI_Null()) return 1;
	S_Var[Depth] = v;
	Depth++;
	if(PF(b, base,fs) == 1) return 1;
	Depth--;
	b = a.Factor0(v);
	if(b == 0) return 0;
	if(b == CtoI_Null()) return 1;
	return PF(b, base,fs);
}

int CsvoutCtoI(CtoI a,ofstream &fs)
{
	if(a == CtoI_Null()) return 1;
	if(a == 0) return 0;
	else
	{
		S_Var =NULL;
		int lev = BDD_LevOfVar(a.TopItem());
		Depth = 0;
		S_Var = new int[lev];
		PFflag = 0;
		int err = PF(a, 10,fs);
		if(err == 1){ 
			yyerror("memory over flow\n");
			delete [] S_Var ;
			S_Var =NULL;
			return 1; 
		}
	}
	fs << endl;
	if(S_Var){
		 delete [] S_Var ;
		 S_Var =NULL;
	}

	return 0;
}

/*##vsop_csvout##*/
PyObject* vsop_csvout(PyCtoIObject* self, PyObject* args){

	char *str = NULL;
  if (!PyArg_ParseTuple(args, "s", &str)) return NULL;
	ofstream fs;
	fs.open(str);
	if(fs.is_open()){
		CsvoutCtoI(*self->ss,fs);
		fs.close();
	}
	else{
		yyerror("file oepn error\n");
	}
	return PyLong_FromLong(0);
}

/*##vsop_export##*/
PyObject* vsop_export(PyCtoIObject* self, PyObject* args){

	char *str = NULL;
  if (!PyArg_ParseTuple(args, "|s", &str)) return NULL;

	CtoI *ctoi_modd = new CtoI(*(self->ss));
	int d = ctoi_modd -> TopDigit();
	ZBDDV v = ZBDDV();
	for(int i=0; i<=d; i++){
	  v += ZBDDV(ctoi_modd -> Digit(i).GetZBDD(), i);
	}
	FILE *fp=NULL;	
	if(str){
		fp = fopen(str, "w");
		if(fp == NULL) yyerror("Can't open the file");
	}
	if(fp){
 		v.Export(fp);
	  fclose(fp);
 	}else{
		v.Export();	
	}
	if(ctoi_modd){ delete ctoi_modd; }

	Py_RETURN_TRUE;
}
/*##vsop_show##*/
// bit,hex,map,rmap,case,decomp exportのみ動くようにする
// 2ndパラメータはexortのみ
PyObject* vsop_show(PyCtoIObject* self, PyObject* args){

	char *v1 = NULL;
	char *v2 = NULL;

  if (!PyArg_ParseTuple(args, "|ss", &v1,&v2)) return NULL;

	if(v1 == NULL){
		vsop_print(self);
	}
	else if(v2!=NULL&&strcmp(v1,"export")){
		yyerror("this swhict not support 2nd parameter");
		Py_RETURN_FALSE;
	}else{
		vsop_print_arg(self,v1,v2);
	}
	Py_RETURN_TRUE;
}


PyObject* ctoi_repr(PyCtoIObject* self){
	return PyStr_FromFormat("<%s object of %p>", Py_TYPE(self)->tp_name,
                             reinterpret_cast<void*>(self->ss));
}


/*##vsop_print_size##*/
static PyObject* vsop_size(PyCtoIObject* self) {
	return PyLong_FromLong(self->ss->Size());
}        


/*##vsop_maxval##*/
static PyObject* vsop_maxval(PyCtoIObject* self) {

	CtoI *ctoi_moda =new CtoI(*self->ss);
	CtoI *ctoi_fin; 
	*ctoi_moda = ctoi_moda -> MaxVal();
  ctoi_fin = ctoi_moda;

  PyCtoIObject* ret = reinterpret_cast<PyCtoIObject*>(
      PyCtoI_Type.tp_alloc(&PyCtoI_Type, 0));
	ret->ss = ctoi_fin;
	return  reinterpret_cast<PyObject*>(ret);

}

/*##vsop_minval##*/
static PyObject* vsop_minval(PyCtoIObject* self) {

	CtoI *ctoi_moda =new CtoI(*self->ss);
	CtoI *ctoi_fin; 
	*ctoi_moda = ctoi_moda -> MinVal();
  ctoi_fin = ctoi_moda;

  PyCtoIObject* ret = reinterpret_cast<PyCtoIObject*>(
      PyCtoI_Type.tp_alloc(&PyCtoI_Type, 0));
	ret->ss = ctoi_fin;
	return  reinterpret_cast<PyObject*>(ret);
}

/*##vsop_totalval##*/
static PyObject* vsop_totalval(PyCtoIObject* self) {

	CtoI a = self->ss->TotalVal();	
	if(a.IsConst())
	{	
		if(a.TopItem() > 0) a = a.MaxVal();
		int d = a.TopDigit() / 3 + 14;
		char *s = new char[d];
	  int err = a.StrNum10(s);
	  if (err) { 
	  	yyerror("StrNum10 error"); 
			return PyLong_FromLong(0);
	  }
		return PyLong_FromString(s,NULL,0);
	}
	return PyLong_FromLong(0);
}


/*##vsop_print_count##*/
static PyObject* vsop_count(PyCtoIObject* self){

	CtoI *ctoitmp = new CtoI(*(self->ss));
	*ctoitmp = ctoitmp -> CountTerms();

	if(*ctoitmp == CtoI_Null()){
		yyerror("memory over flow\n");
		*ctoitmp = 0;
		return PyLong_FromLong(0);
	}
	int slen = ctoitmp->TopDigit() / 3 + 14;
	char *s = new char[slen];
	ctoitmp -> StrNum10(s);
	PyObject* rtn = PyLong_FromString(s,NULL,0);
	if(s){ delete s;}
	if(ctoitmp){ delete ctoitmp;}
	return rtn;
}        

/*##vsop_print_density##*/
static PyObject* vsop_density(PyCtoIObject* self){

	CtoI *ctoitmp = new CtoI(*(self->ss));
	*ctoitmp = ctoitmp -> NonZero();
	if(*ctoitmp == CtoI_Null()){
		yyerror("memory over flow\n");
		*ctoitmp = 0;
		return PyFloat_FromDouble(0);
	}
	int d = Density(ctoitmp -> GetZBDD(), VTable.Used());
	if(d == 0 && *ctoitmp != 0){
		yyerror("memory over flow\n");
		delete ctoitmp;
		return PyFloat_FromDouble(0);
	}
	delete ctoitmp;	
	return PyFloat_FromDouble((double)d / power30);
}

/*##vsop_print_cost org(value)##*/
static PyObject* vsop_cost(PyCtoIObject* self){

	CtoI *ctoitmp = new CtoI(*(self->ss));
	
	if(OVF.Check()!= 0){
		yyerror("memory over flow\n");
		delete ctoitmp;
		return PyFloat_FromDouble(0);
	}
	int d = Value(*ctoitmp);
	if(ctoitmp){ delete ctoitmp;}
	return PyFloat_FromDouble((double)d/power16);
}        

/*##vsop_print_maxcover##*/
static PyObject* vsop_maxcover(PyCtoIObject* self){

	CtoI *ctoitmp = new CtoI(*(self->ss));
	CtoI *ctoi_fin; 
	*ctoitmp = ctoitmp -> NonZero();

	if(*ctoitmp == CtoI_Null()){
		*ctoitmp = 0;
		yyerror("Memory overflow");
	}

	if(*ctoitmp == 0){
		ctoi_fin = new CtoI(0);
	}
	else{
		ZBDD f = ctoitmp -> GetZBDD();
		if(MaxCost(f)==0){
			ctoi_fin = new CtoI(1);
		}
		else{

			CtoI ctmp(1);
			while(1){
				int var = f.Top();
				if(var == 0) break;
				ZBDD f0 = f.OffSet(var);
				ZBDD f1 = f.OnSet0(var);
				int c1 = MaxCost(f1) + VTable.GetValue(var);
				if(MaxCost(f0) < c1){
					f = f1;
					ctmp = ctmp.AffixVar(var);
				}
				else f = f0;
			}
			ctoi_fin = new CtoI(ctmp);
		}
	}

	if(ctoitmp) { delete ctoitmp;}
  PyCtoIObject* ret = reinterpret_cast<PyCtoIObject*>(
      PyCtoI_Type.tp_alloc(&PyCtoI_Type, 0));
	ret->ss = ctoi_fin;
	return  reinterpret_cast<PyObject*>(ret);

}        

/*##vsop_maxcost##*/
static PyObject* vsop_maxcost(PyCtoIObject* self){

	CtoI *ctoitmp = new CtoI(*(self->ss));
	*ctoitmp = ctoitmp -> NonZero();

	if(*ctoitmp == CtoI_Null()){
		*ctoitmp = 0;
		yyerror("09Memory overflow");
	}
	if(*ctoitmp == 0){
		if(ctoitmp){ delete ctoitmp;}
		Py_RETURN_NONE ;
	}
	else{
		int c = MaxCost(ctoitmp -> GetZBDD());
		if(ctoitmp){ delete ctoitmp;}
		return PyFloat_FromDouble((double)c/power16);
	}
}        

/*##vsop_print_mincover##*/
static PyObject* vsop_mincover(PyCtoIObject* self){

	CtoI *ctoitmp = new CtoI(*(self->ss));	
	CtoI *ctoi_fin; 

	*ctoitmp = ctoitmp -> NonZero();

	if(*ctoitmp == CtoI_Null()){
		*ctoitmp = 0;
		yyerror("10Memory overflow");
	}
	if(*ctoitmp == 0){
		ctoi_fin = new CtoI(0);
	}
	else
	{
		ZBDD f = ctoitmp -> GetZBDD();
		if(MinCost(f)==0){
			ctoi_fin = new CtoI(1);
		}
		else{
			CtoI ctmp(1);
			while(1)
			{
				int var = f.Top();
				if(var == 0) break;
				ZBDD f0 = f.OffSet(var);
				ZBDD f1 = f.OnSet0(var);
				int c1 = MinCost(f1) + VTable.GetValue(var);
				if(MinCost(f0) > c1)
				{
					f = f1;
					ctmp = ctmp.AffixVar(var);
				}
				else f = f0;
			}
			ctoi_fin = new CtoI(ctmp);
		}
	}
	if(ctoitmp){ delete ctoitmp;}
  PyCtoIObject* ret = reinterpret_cast<PyCtoIObject*>(
      PyCtoI_Type.tp_alloc(&PyCtoI_Type, 0));
	ret->ss = ctoi_fin;
	return  reinterpret_cast<PyObject*>(ret);

}        

/*##vsop_print_mincost##*/
static PyObject* vsop_mincost(PyCtoIObject* self){

	CtoI *ctoitmp = new CtoI(*(self->ss));	
	
	*ctoitmp = ctoitmp -> NonZero();
	if(*ctoitmp == CtoI_Null()){
		*ctoitmp = 0;
		if(ctoitmp){ delete ctoitmp;}
		Py_RETURN_NONE ;
	}
	else
	{
		int c = MinCost(ctoitmp -> GetZBDD());
		if(ctoitmp){ delete ctoitmp;}
		return PyFloat_FromDouble((double)c/power16);
	}

}        






static PyObject* ctoi_richcompare(PyCtoIObject* self, PyObject* obj, int op) {

	CtoI *ctoi_moda =new CtoI(*(self->ss));
	CtoI *ctoi_modc = value2ctoi(obj);
	CtoI *ctoi_fin ;

	switch (op) {
    case Py_EQ:
    	*ctoi_moda = CtoI_EQ(*ctoi_moda, *ctoi_modc);
    	break;
    case Py_NE:
    	*ctoi_moda = CtoI_NE(*ctoi_moda, *ctoi_modc);
    	break;
    case Py_LE:
    	*ctoi_moda = CtoI_LE(*ctoi_moda, *ctoi_modc);
    	break;
    case Py_GE:
    	*ctoi_moda = CtoI_GE(*ctoi_moda, *ctoi_modc);
    	break;
    case Py_LT:
    	*ctoi_moda = CtoI_LT(*ctoi_moda, *ctoi_modc);
    	break;
    case Py_GT:
    	*ctoi_moda = CtoI_GT(*ctoi_moda, *ctoi_modc);
    	break;
    default:
    	yyerror("NotImplemented");
    	break;
  } 
  ctoi_fin = ctoi_moda;
  delete ctoi_modc;
  PyCtoIObject* ret = reinterpret_cast<PyCtoIObject*>(
      PyCtoI_Type.tp_alloc(&PyCtoI_Type, 0));
	ret->ss = ctoi_fin;
	return  reinterpret_cast<PyObject*>(ret);

}



static PyNumberMethods ctoi_as_number = {
  reinterpret_cast<binaryfunc>(vsop_plus),  /*nb_add*/
  reinterpret_cast<binaryfunc>(vsop_minus), /*nb_subtract*/
  reinterpret_cast<binaryfunc>(vsop_multiply), /*nb_multiply*/
#if PY_MAJOR_VERSION < 3
  reinterpret_cast<binaryfunc>(vsop_quotiment), /*nb_divide*/
#endif
  reinterpret_cast<binaryfunc>(vsop_remainder), /*nb_remainder*/
  0,                                  /*nb_divmod*/
  0,                                  /*nb_power*/
  reinterpret_cast<unaryfunc>(vsop_minus_op),/*nb_negative*/
  reinterpret_cast<unaryfunc>(vsop_plus_op),/*nb_positive*/
  0,                                  /*nb_absolute*/
  0,                                  /*nb_nonzero or nb_bool*/
  0,                                  /*nb_invert*/
  0,                                  /*nb_lshift*/
  0,                                  /*nb_rshift*/
  0,                                  /*nb_and*/
  0,                                  /*nb_xor*/
  0,                                  /*nb_or*/
#if PY_MAJOR_VERSION < 3
  0, /*nb_coerce*/
#endif
  0,                                  /*nb_int*/
  0,                                  /*nb_long or *nb_reserved*/
  0,                                  /*nb_float*/
#if PY_MAJOR_VERSION < 3
  0,                                  /*nb_oct*/
  0,                                  /*nb_hex*/
#endif
  0,                                  /*nb_inplace_add*/
  0,                                  /*nb_inplace_subtract*/
  0,                                  /*nb_inplace_multiply*/
#if PY_MAJOR_VERSION < 3
  0,                                  /*nb_inplace_divide*/
#endif
  0,                                  /*nb_inplace_remainder*/
  0,                                  /*nb_inplace_power*/
  0,                                  /*nb_inplace_lshift*/
  0,                                  /*nb_inplace_rshift*/
  0,                                  /*nb_inplace_and*/
  0,                                  /*nb_inplace_xor*/
  0,                                  /*nb_inplace_or*/
#if PY_MAJOR_VERSION >= 3
  0,                                   /*nb_floor_divide*/
  reinterpret_cast<binaryfunc>(vsop_quotiment), /*nb_true_divide*/
  0,                                   /*nb_inplace_floor_divide*/
  0,                                   /*nb_inplace_true_divide*/
  0                                   /*nb_index*/
	// for 3.5?
	//  0, /*nb_matrix_multiply*/
	//  0 /*nb_inplace_matrix_multiply*/
#endif
};


static PyMethodDef ctoi_obj_methods[] = {
  {"iif", reinterpret_cast<PyCFunction>(vsop_iif), METH_VARARGS, ""},
  {"same", reinterpret_cast<PyCFunction>(vsop_same), METH_VARARGS, ""},
  {"diff", reinterpret_cast<PyCFunction>(vsop_diff), METH_VARARGS, ""},
  {"csvout", reinterpret_cast<PyCFunction>(vsop_csvout), METH_VARARGS, ""},
  {"export", reinterpret_cast<PyCFunction>(vsop_export), METH_VARARGS, ""},
  {"show",reinterpret_cast<PyCFunction>(vsop_show), METH_VARARGS, ""},
 	{"permit",reinterpret_cast<PyCFunction>(vsop_permit), METH_VARARGS, ""},
  {"permitsym",reinterpret_cast<PyCFunction>(vsop_permitsym), METH_VARARGS, ""},
  {"restrict",reinterpret_cast<PyCFunction>(vsop_restrict), METH_VARARGS, ""},
  {"freqpatC",reinterpret_cast<PyCFunction>(vsop_freqpatC), METH_VARARGS, ""},
  {"freqpatM",reinterpret_cast<PyCFunction>(vsop_freqpatM), METH_VARARGS, ""},
  {"freqpatA",reinterpret_cast<PyCFunction>(vsop_freqpatA), METH_VARARGS, ""},
  {"termsLE",reinterpret_cast<PyCFunction>(vsop_termsLE), METH_VARARGS, ""},
  {"termsLT",reinterpret_cast<PyCFunction>(vsop_termsLT), METH_VARARGS, ""},
  {"termsGE",reinterpret_cast<PyCFunction>(vsop_termsGE), METH_VARARGS, ""},
  {"termsGT",reinterpret_cast<PyCFunction>(vsop_termsGT), METH_VARARGS, ""},
  {"termsNE",reinterpret_cast<PyCFunction>(vsop_termsNE), METH_VARARGS, ""},
  {"termsEQ",reinterpret_cast<PyCFunction>(vsop_termsEQ), METH_VARARGS, ""},
  {"meet",reinterpret_cast<PyCFunction>(vsop_meet), METH_VARARGS, ""},
  {"delta",reinterpret_cast<PyCFunction>(vsop_delta), METH_VARARGS, ""},
	{"hashout",reinterpret_cast<PyCFunction>(vsop_hashout), METH_NOARGS,""},
	{"maxweight",reinterpret_cast<PyCFunction>(vsop_maxval),METH_NOARGS,""},
	{"minweight",reinterpret_cast<PyCFunction>(vsop_minval),METH_NOARGS,""},
	{"totalweight",reinterpret_cast<PyCFunction>(vsop_totalval), METH_NOARGS,""},
	{"items", reinterpret_cast<PyCFunction>(vsop_items), METH_NOARGS,""},
	{"symgrp", reinterpret_cast<PyCFunction>(vsop_symgrp), METH_NOARGS,""},
	{"size",reinterpret_cast<PyCFunction>(vsop_size), METH_NOARGS, ""},
	{"totalsize",reinterpret_cast<PyCFunction>(vsop_totalsize), METH_NOARGS, ""},
	{"count",reinterpret_cast<PyCFunction>(vsop_count),  METH_NOARGS, ""},
	{"density",reinterpret_cast<PyCFunction>(vsop_density), METH_NOARGS, ""},
	{"cost",reinterpret_cast<PyCFunction>(vsop_cost), METH_NOARGS, ""},
	{"maxcover",reinterpret_cast<PyCFunction>(vsop_maxcover), METH_NOARGS, ""},
	{"maxcost",reinterpret_cast<PyCFunction>(vsop_maxcost), METH_NOARGS, ""},
	{"mincover",reinterpret_cast<PyCFunction>(vsop_mincover), METH_NOARGS, ""},
	{"mincost",reinterpret_cast<PyCFunction>(vsop_mincost), METH_NOARGS, ""},
  {"itemiter", reinterpret_cast<PyCFunction>(vsop_itemiter), METH_NOARGS, ""},
  {NULL}  /* Sentinel */
};




PyTypeObject PyCtoI_Type = {
  PyVarObject_HEAD_INIT(NULL, 0)
  "_vsoplib.ctoi",              /*tp_name*/
  sizeof(PyCtoIObject),             /*tp_basicsize*/
  0,                                  /*tp_itemsize*/
  reinterpret_cast<destructor>(ctoi_dealloc), /*tp_dealloc*/
  0,  /*tp_print*/
  0,                                  /*tp_getattr*/
  0,                                  /*tp_setattr*/
#if PY_MAJOR_VERSION >= 3
  0,                                  /*tp_reserved at 3.4*/
#else
  0,                                  /*tp_compare*/
#endif
  reinterpret_cast<reprfunc>(ctoi_repr), /*tp_repr*/
  &ctoi_as_number,                  /*tp_as_number*/
  0,                                  /*tp_as_sequence*/
  0,                                  /*tp_as_mapping*/
  0,                                  /*tp_hash */
  0,                                  /*tp_call*/
  0,                                  /*tp_str*/
  0,                                  /*tp_getattro*/
  0,                                  /*tp_setattro*/
  0,                                  /*tp_as_buffer*/
#if PY_MAJOR_VERSION >= 3
  Py_TPFLAGS_DEFAULT | Py_TPFLAGS_BASETYPE , /*tp_flags*/
#else
  Py_TPFLAGS_DEFAULT | Py_TPFLAGS_BASETYPE | Py_TPFLAGS_CHECKTYPES, /*tp_flags*/
#endif
  0,                         /* tp_doc */
  0,		                      /* tp_traverse */
  0,		                      /* tp_clear */
  reinterpret_cast<richcmpfunc>(ctoi_richcompare), /* tp_richcompare */
  0,		                      /* tp_weaklistoffset */
  vsop_iter,                          /* tp_iter */
  reinterpret_cast<iternextfunc>(ctoiiter_next), /* tp_iternext */
  ctoi_obj_methods,                     /* tp_methods */
  0,//ctoi_members,                     /* tp_members */
  0,                                  /* tp_getset */
  0,                                  /* tp_base */
  0,                                  /* tp_dict */
  0,                                  /* tp_descr_get */
  0,                                  /* tp_descr_set */
  0,                                  /* tp_dictoffset */
  reinterpret_cast<initproc>(ctoi_init), /* tp_init */
  PyType_GenericAlloc,                /* tp_alloc */
  ctoi_new,                         /* tp_new */
#if PY_MAJOR_VERSION >= 3
  0, /* tp_free */
  0, /* tp_is_gc */
  0, /* *tp_bases */
  0, /* *tp_mro */
  0, /* *tp_cache */
  0, /* *tp_subclasses */
  0, /* *tp_weaklist */
  0, /* tp_version_tag */
  0, /* tp_finalize */
#endif
};


#if PY_MAJOR_VERSION >= 3
static struct PyModuleDef moduledef = {
    PyModuleDef_HEAD_INIT,
    "_vsoplib",      /* m_name */
    NULL,							     /* m_doc */
    -1,                  /* m_size */
    ctoi_methods,      /* m_methods */
    NULL,                /* m_reload */
    NULL,                /* m_traverse */
    NULL,                /* m_clear */
    NULL,                /* m_free */
};

PyMODINIT_FUNC
PyInit__vsoplib(void){
	PyObject* m;
  if (PyType_Ready(&PyCtoI_Type) < 0) return NULL;
  if (PyType_Ready(&PyCtoIIter_Type) < 0) return NULL;
  m = PyModule_Create(&moduledef);

  Py_INCREF(Py_False);
	py_partly = Py_False;

	if(m==NULL){ return m; }
  Py_INCREF(&PyCtoI_Type);
  Py_INCREF(&PyCtoIIter_Type);
	PyModule_AddObject(m, "ctoi", reinterpret_cast<PyObject*>(&PyCtoI_Type));
  PyModule_AddObject(m, "ctoi_iterator",
                     reinterpret_cast<PyObject*>(&PyCtoIIter_Type));
	PyModule_AddObject(m, "partly", reinterpret_cast<PyObject*>(py_partly));

	return m;
}

#else

void init_vsoplib(void){
	PyObject* m;
  if (PyType_Ready(&PyCtoI_Type) < 0) return ;
  if (PyType_Ready(&PyCtoIIter_Type) < 0) return;
	m = Py_InitModule("_vsoplib", ctoi_methods);

  Py_INCREF(Py_False);
	py_partly = Py_False;

	if(m==NULL){ return ; }
  Py_INCREF(&PyCtoI_Type);
  Py_INCREF(&PyCtoIIter_Type);
	PyModule_AddObject(m, "ctoi", reinterpret_cast<PyObject*>(&PyCtoI_Type));
  PyModule_AddObject(m, "ctoi_iterator",
                     reinterpret_cast<PyObject*>(&PyCtoIIter_Type));
	PyModule_AddObject(m, "partly", reinterpret_cast<PyObject*>(py_partly));

	return ;
}

#endif
