/* ////////// LICENSE INFO ////////////////////

 * Copyright (C) 2013 by NYSOL CORPORATION
 *
 * Unless you have received this program directly from NYSOL pursuant
 * to the terms of a commercial license agreement with NYSOL, then
 * this program is licensed to you under the terms of the GNU Affero General
 * Public License (AGPL) as published by the Free Software Foundation,
 * either version 3 of the License, or (at your option) any later version.
 * 
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY EXPRESS OR IMPLIED WARRANTY, INCLUDING THOSE OF 
 * NON-INFRINGEMENT, MERCHANTABILITY OR FITNESS FOR A PARTICULAR PURPOSE.
 *
 * Please refer to the AGPL (http://www.gnu.org/licenses/agpl-3.0.txt)
 * for more details.

 ////////// LICENSE INFO ////////////////////*/
#include "Python.h"
#include <fcntl.h>
#include <kgmod.h>
#include <kgEnv.h>
#include <kgCSV.h>
#include <kgArgs.h>
#include <kgchkcsv.h>
#include <kgCSVout.h>
#include <kgMessage.h>
#include <kgMethod.h>


#if PY_MAJOR_VERSION >= 3
extern "C" {
	PyMODINIT_FUNC PyInit__utillib(void);
}
#else
extern "C" {
	void init_utillib(void);
}
#endif


#if PY_MAJOR_VERSION >= 3
 #define strGET PyUnicode_AsUTF8
 #define strCHECK PyUnicode_Check
 #define numCHECK(data) (PyLong_Check(data))
#else		
 #define strGET PyString_AsString
 #define strCHECK PyString_Check
 #define numCHECK(data) (PyInt_Check(data)||PyLong_Check(data))
#endif

/*
static bool numCHECK(PyObject* data){

#if PY_MAJOR_VERSION >= 3
	return PyLong_Check(data);
#else		
	return PyInt_Check(data) || PyLong_Check(data);
#endif
}
*/


// =============================================================================
// mrecountメソッド
// =============================================================================
/*
 * call-seq:
 * mrecount -> int
 *
 * <b>説明</b> レコード数を返す。
 *
 * <b>書式</b> MCMD::mrecount(options)
 *
 * i=ファイル名
 * -nfn : 一行目を項目名行とみなさない。
 */
PyObject* mrecount(PyObject* self, PyObject* args){
	kgEnv  env;
	int recNo=0; // 返値

	try { // kgmodのエラーはrubyのエラーとは別に検知する(メッセージ表示のため)

		// 引数をopetionsにセット
		char *fname=NULL;
		int nfn = 0;

		if(!PyArg_ParseTuple(args, "s|i", &fname,&nfn)){
			Py_RETURN_NONE ;
		}
		kgCSVrec csv;
		csv.open( fname, &env, nfn, 4 );
    csv.read_header();

		while(EOF!=csv.read()) recNo++;
		csv.close();

	}catch(kgError& err){ // kgmod関係エラーのchatch
		err.addModName("mrecount");
		kgMsg msg(kgMsg::ERR, &env);
		msg.output(err.message());
		return PyLong_FromLong(-1);
	}
	// 件数を返す
	return PyLong_FromLong(recNo);

}
// =============================================================================
// mheaderメソッド
// =============================================================================
/*
 * call-seq:
 * mheader -> int
 *
 * <b>説明</b> 項目名配列を返す。
 *
 * <b>書式</b> MCMD::mheader(options)
 *
 * i=ファイル名
 */
PyObject* mheader(PyObject* self, PyObject* args){

	kgEnv  env;
	string name; // rubyスクリプト名
	string argstr;

	try { // kgmodのエラーはrubyのエラーとは別に検知する(メッセージ表示のため)

		// 引数をopetionsにセット
		char *fname=NULL;
		int nfn = 0;

		if(!PyArg_ParseTuple(args, "s|i", &fname,&nfn)){
			Py_RETURN_NONE ;
		}

		kgCSVrec csv;
		csv.open( fname, &env, nfn, 4 );
    csv.read_header();
		size_t fldSize=csv.fldSize();
		PyObject* names = PyList_New(fldSize);

		if(nfn){
			for(size_t i=0; i<fldSize; i++){
				PyList_SetItem( names, i, PyLong_FromLong(i));
			}
		}else{
			for(size_t i=0; i<fldSize; i++){
				PyList_SetItem( names, i, Py_BuildValue("s",csv.fldName(i).c_str()) );
			}
		}
		csv.close();

		return names;

	}catch(kgError& err){ // kgmod関係エラーのchatch
		err.addModName("mheader");
		kgMsg msg(kgMsg::ERR, &env);
		msg.output(err.message());
			Py_RETURN_NONE ;
	}
}

PyObject* mchkcsv(PyObject* self, PyObject* args){

	kgEnv  env;
	string name; // rubyスクリプト名
	string argstr;
	kgArgs modargs;
  kgmod::kgChkcsv kgmod;

	try { // kgmodのエラーはrubyのエラーとは別に検知する(メッセージ表示のため)

		// 引数をopetionsにセット
		char *fname=NULL;
		int nfn = 0;
		int local = 0;

		if(!PyArg_ParseTuple(args, "s|ii", &fname,&nfn,&local)){
			Py_RETURN_NONE ;
		}
		
		modargs.add("i=",fname);
		if(nfn){ modargs.add("-nfn",""); }
		if(local){ modargs.add("-diagl",""); }
		else     { modargs.add("-diag",""); }
	  kgmod.init(modargs, &env);
  	kgmod.run();

		Py_RETURN_NONE ;

	}catch(kgError& err){ // kgmod関係エラーのchatch
		err.addModName("mchkcsv");
		kgMsg msg(kgMsg::ERR, &env);
		msg.output(err.message());
			Py_RETURN_NONE ;
	}
}

typedef struct {
  PyObject_HEAD
  kgCSVout* ss;
	kgEnv  *env;
	size_t fldcnt;
	char * truestr;
	char * falsestr;

} PyMcsvoutObject;


static PyObject* mcsvout_write(PyMcsvoutObject* self,PyObject* args) {

	PyObject* rcvobj = NULL;	
  if (!PyArg_ParseTuple(args, "O", &rcvobj)) return NULL;

	if(!PyList_Check(rcvobj)){
		cerr << "cannot run " << endl;
		return Py_BuildValue("");
	}
	
	size_t msize = PyList_Size(rcvobj);

	size_t lastsize = (self->fldcnt==0) ? msize : self->fldcnt;
	size_t osize = ( self->fldcnt==0 || self->fldcnt > msize )? msize :self->fldcnt;
	
	for(size_t i=0 ; i< osize ; i++){
		PyObject* v = PyList_GetItem(rcvobj ,i);
		
		if(strCHECK(v)){
			self->ss->writeStr(strGET(v),i==lastsize-1); 
		}
		else if ( v== Py_True){
			self->ss->writeStr(self->truestr,i==lastsize-1); 
		}
		else if ( v== Py_False){
			self->ss->writeStr(self->falsestr,i==lastsize-1); 
		}
		else if(numCHECK(v)){
			self->ss->writeLong(PyLong_AsLong(v),i==lastsize-1);
		}
		else if(PyFloat_Check(v)){
			self->ss->writeDbl(PyFloat_AsDouble(v),i==lastsize-1);
		}
		else{
			self->ss->writeStr("",i==lastsize-1);
		}
	}

	// 配列のサイズが項目名サイズより小さい時は、null値を出力。
	if(osize < lastsize){
		for(size_t i=osize; i<lastsize; i++){
			self->ss->writeStr("",i==lastsize-1);
		}
	}
	return PyLong_FromLong(0);
}        

static PyObject* mcsvout_close(PyMcsvoutObject* self) {
	self->ss->close();
	Py_RETURN_TRUE;
}        

static PyObject* mcsvout_enter(PyMcsvoutObject* self) {
	Py_INCREF(self); 
	return reinterpret_cast<PyObject*>(self);
}        

static PyObject* mcsvout_exit(PyMcsvoutObject* self,PyObject* args) {

	PyObject* exc_type;
	PyObject* exc_value;
	PyObject* traceback;

	if(self->ss){
		self->ss->close();
	}

  if (!PyArg_ParseTuple(args, "OOO", &exc_type,&exc_value,&traceback)){
  	return NULL;
  }

	if(exc_type != Py_None){ 
		Py_RETURN_FALSE;
	}
	
	Py_RETURN_TRUE;



}        

static void mcsvout_dealloc(PyMcsvoutObject* self) {
	if(self->ss){
		self->ss->close();
 		delete self->ss;
 	}
 	if(self->env){ delete self->env; }
 	if(self->truestr){ delete [] self->truestr; }
 	if(self->falsestr){ delete [] self->falsestr; }

  Py_TYPE(self)->tp_free(reinterpret_cast<PyObject*>(self));
}

static int mcsvout_init(PyMcsvoutObject* self, PyObject* args, PyObject* kwds) 
{
	try {
	static const char *kwlist[] = {"o","f", "size","precision","bool",NULL};
  char *fname =NULL;
  PyObject* head=NULL;
  int fsize=0;
  int precision=KG_PRECISION;
  PyObject* boollist=NULL;

	if (! PyArg_ParseTupleAndKeywords(
					args, kwds, "s|OiiO", const_cast<char**>(kwlist), 
					&fname,&head,&fsize,&precision,&boollist))
	{
    return -1;
  }
	self->env = new kgEnv;

	if(precision!=KG_PRECISION){
		self->env->precision(precision);
	}

	self->ss = new kgCSVout;
	self->ss->open(fname, self->env, false);
	self->fldcnt = fsize;

  if(head){
  	if(strCHECK(head)){
  		kgstr_t hstr(strGET(head));
			vector<kgstr_t> heads = splitToken(hstr, ',');
			if( self->fldcnt==0 || self->fldcnt > heads.size() ){
				self->fldcnt = heads.size();
			}					
			for(size_t i=0 ; i< self->fldcnt; i++){
				self->ss->writeStr(heads[i].c_str(),i==self->fldcnt-1);
			}
  	}
		else if(PyList_Check(head)){
			size_t lsize = PyList_Size(head);
			if( self->fldcnt==0 || self->fldcnt > lsize ){
				self->fldcnt = PyList_Size(head);
			}
			for(size_t i=0 ; i<self->fldcnt;i++){
				if(!strCHECK(PyList_GetItem(head ,i))){
					PyErr_SetString(PyExc_RuntimeError,"not suport TYPE");
				 	return -1;
				}
				const char * v = strGET( PyList_GetItem(head ,i) );
				self->ss->writeStr(v,i==self->fldcnt-1);
			}
		}
		else{
			PyErr_SetString(PyExc_RuntimeError,"not suport TYPE");
		 	return -1;
		}
  }
	const char * tval="1";
	const char * fval="0";
  if(boollist){
  	if(strCHECK(boollist)){
  		string bstr(strGET(boollist));
			vector<string> blist = splitToken(bstr, ',');
			if(blist.size()>0){ tval =blist[0].c_str();}
			if(blist.size()>1){ fval =blist[1].c_str();}
  	}
		else if(PyList_Check(boollist)){
			if(PyList_Size(boollist)>0){  tval = strGET( PyList_GetItem(boollist ,0)); } 
			if(PyList_Size(boollist)>1){  fval = strGET( PyList_GetItem(boollist ,1)); } 
		}
		else{
			PyErr_SetString(PyExc_RuntimeError,"not suport TYPE");
		 	return -1;
		}
  }
  self->truestr = new char[strlen(tval)+1];
  self->falsestr = new char[strlen(fval)+1];
 	strcpy(self->truestr,tval);
	strcpy(self->falsestr,fval);
  }
	catch(kgError& err){
		PyErr_SetString(PyExc_RuntimeError,err.message(0).c_str());
	 	return -1;
	}
  return 0;
}

static PyObject* mcsvout_new(PyTypeObject* type, PyObject* args, PyObject* kwds) {
  PyMcsvoutObject* self;
  self = reinterpret_cast<PyMcsvoutObject*>(type->tp_alloc(type, 0));
  if (self == NULL) return NULL;
  self->ss=NULL;
  self->env=NULL;
  self->truestr=NULL;
  self->falsestr=NULL;
  return reinterpret_cast<PyObject*>(self);
}

static PyMethodDef mcsvout_obj_methods[] = {
	{"write",reinterpret_cast<PyCFunction>(mcsvout_write),METH_VARARGS,""},
	{"close",reinterpret_cast<PyCFunction>(mcsvout_close), METH_NOARGS,""},
	{"__enter__",reinterpret_cast<PyCFunction>(mcsvout_enter), METH_NOARGS,""},
	{"__exit__",reinterpret_cast<PyCFunction>(mcsvout_exit), METH_VARARGS,""},
  {NULL}  /* Sentinel */
};

PyTypeObject PyMcsvout_Type = {
  PyVarObject_HEAD_INIT(NULL, 0)
  "_utillib.mcsvout",              /*tp_name*/
  sizeof(PyMcsvoutObject),             /*tp_basicsize*/
  0,                                  /*tp_itemsize*/
  reinterpret_cast<destructor>(mcsvout_dealloc), /*tp_dealloc*/
  0,  /*tp_print*/
  0,                                  /*tp_getattr*/
  0,                                  /*tp_setattr*/
#if PY_MAJOR_VERSION >= 3
  0,                                  /*tp_reserved at 3.4*/
#else
  0,                                  /*tp_compare*/
#endif
  0, /*tp_repr*/
  0,                  /*tp_as_number*/
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
  0, /* tp_richcompare */
  0,		                      /* tp_weaklistoffset */
  0,                          /* tp_iter */
  0, /* tp_iternext */
  mcsvout_obj_methods,                     /* tp_methods */
  0,//ctoi_members,                     /* tp_members */
  0,                                  /* tp_getset */
  0,                                  /* tp_base */
  0,                                  /* tp_dict */
  0,                                  /* tp_descr_get */
  0,                                  /* tp_descr_set */
  0,                                  /* tp_dictoffset */
  reinterpret_cast<initproc>(mcsvout_init), /* tp_init */
  PyType_GenericAlloc,                /* tp_alloc */
  mcsvout_new,                         /* tp_new */
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


static PyMethodDef utilmethods[] = {
	{"mrecount",(PyCFunction)mrecount, METH_VARARGS  },
	{"mheader" ,(PyCFunction)mheader , METH_VARARGS  },
	{"mchkcsv" ,(PyCFunction)mchkcsv , METH_VARARGS  },
	{NULL}
};


#if PY_MAJOR_VERSION >= 3
static struct PyModuleDef moduledef = {
    PyModuleDef_HEAD_INIT,
    "_utillib",      /* m_name */
    NULL,							     /* m_doc */
    -1,                  /* m_size */
    utilmethods,      /* m_methods */
    NULL,                /* m_reload */
    NULL,                /* m_traverse */
    NULL,                /* m_clear */
    NULL,                /* m_free */
};

PyMODINIT_FUNC
PyInit__utillib(void){
	PyObject* m;
  if (PyType_Ready(&PyMcsvout_Type) < 0) return NULL;
	m = PyModule_Create(&moduledef);
	if(m==NULL){ return m; }
  Py_INCREF(&PyMcsvout_Type);
	PyModule_AddObject(m, "mcsvout", reinterpret_cast<PyObject*>(&PyMcsvout_Type));

	return m;
}

#else

void init_utillib(void){
	PyObject* m;
  if (PyType_Ready(&PyMcsvout_Type) < 0) return ;
	m = Py_InitModule("_utillib", utilmethods);
	if(m==NULL){ return ; }
  Py_INCREF(&PyMcsvout_Type);
	PyModule_AddObject(m, "mcsvout", reinterpret_cast<PyObject*>(&PyMcsvout_Type));

}

#endif

