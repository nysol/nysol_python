#include "Python.h"
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <stdio.h>
#include <kgmaketra.h>

#define MAXC 1024000




#if PY_MAJOR_VERSION >= 3
extern "C" {
	PyMODINIT_FUNC PyInit__mmaketralib(void);
}
#else
extern "C" {
	void init_mmaketralib(void);
}
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


PyObject* mmaketra_run(PyObject* self, PyObject* args, PyObject* kwds)
{


	kgEnv  env;
	kgArgs modargs;
  kgmod::kgMaketra kgmod;

	static const char *kwlist[] = {"i","o","k","f","s","q",NULL};
	char * inf =NULL;
	char * outf=NULL;
  PyObject* kval=NULL;
  PyObject* sval=NULL;
  PyObject* fval=NULL;
	int qval = 0;

	if (! PyArg_ParseTupleAndKeywords(
					args, kwds, "ss|OOOi", const_cast<char**>(kwlist), 
					&inf,&outf,&kval,&fval,&sval,&qval))
	{
    return NULL;
  }

	modargs.add("i=",inf);
	modargs.add("o=",outf);
	
	if(qval){ modargs.add("-q",""); }

  if(kval){
  	if(strCHECK(kval)){
	  	modargs.add("k=", kgstr_t(strGET(kval)));
  	}
		else if(PyList_Check(kval)){
			kgstr_t kstr; 
			size_t lsizek = PyList_Size(kval);
			for(size_t i=0 ; i<lsizek ;i++){
				if(!strCHECK(PyList_GetItem(kval ,i))){
					cerr << "not suport TYPE" << endl;
	 		   	return PyLong_FromLong(-1);
				}
				if(i==0){
					kstr +=  kgstr_t( strGET( PyList_GetItem(kval ,i)) );
				}else{
					kstr +=  "," + kgstr_t( strGET( PyList_GetItem(kval ,i)) );
				}
			}
	  	modargs.add("k=", kstr);
		}
  }
  if(fval){
  	if(strCHECK(fval)){
	  	modargs.add("f=", kgstr_t(strGET(fval)));
  	}
		else if(PyList_Check(fval)){
			kgstr_t fstr; 
			size_t lsizef = PyList_Size(fval);
			for(size_t i=0 ; i< lsizef;i++){
				if(!strCHECK(PyList_GetItem(fval ,i))){
					cerr << "not suport TYPE" << endl;
	 		   	return PyLong_FromLong(-1);
				}
				if(i==0){
					fstr +=  kgstr_t( strGET( PyList_GetItem(fval ,i)) );
				}else{
					fstr +=  "," + kgstr_t( strGET( PyList_GetItem(fval ,i)) );
				}
			}
	  	modargs.add("f=", fstr);
		}
  }
  if(sval){
  	if(strCHECK(sval)){
	  	modargs.add("s=", kgstr_t(strGET(sval)));
  	}
		else if(PyList_Check(sval)){
			kgstr_t sstr; 
			size_t lsizes = PyList_Size(sval);
			for(size_t i=0 ; i< lsizes;i++){
				if(!strCHECK(PyList_GetItem(sval ,i))){
					cerr << "not suport TYPE" << endl;
	 		   	return PyLong_FromLong(-1);
				}
				if(i==0){
					sstr +=  kgstr_t( strGET( PyList_GetItem(sval ,i)) );
				}else{
					sstr +=  "," + kgstr_t( strGET( PyList_GetItem(sval ,i)) );
				}
			}
	  	modargs.add("F=", sstr);
		}
  }
	int rtn=0;
	try {
		kgmod.init(modargs, &env);
	  rtn = kgmod.run();
	}catch(...){
		rtn = 1;
	}

	return PyLong_FromLong(rtn);
}

static PyMethodDef takemethods[] = {
	{"mmaketra_run", reinterpret_cast<PyCFunction>(mmaketra_run), METH_VARARGS|METH_KEYWORDS },
	{NULL}
};

#if PY_MAJOR_VERSION >= 3
static struct PyModuleDef moduledef = {
    PyModuleDef_HEAD_INIT,
    "_mmaketralib",      /* m_name */
    NULL,							     /* m_doc */
    -1,                  /* m_size */
    takemethods,      /* m_methods */
    NULL,                /* m_reload */
    NULL,                /* m_traverse */
    NULL,                /* m_clear */
    NULL,                /* m_free */
};

PyMODINIT_FUNC
PyInit__mmaketralib(void){
	PyObject* m;
	m = PyModule_Create(&moduledef);
	return m;
}

#else

void init_mmaketralib(void){
	Py_InitModule("_mmaketralib", takemethods);
}

#endif

