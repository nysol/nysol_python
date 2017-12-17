
#include "Python.h"

namespace {
extern "C" {
#include "mace/mace.c"
}
}

#if PY_MAJOR_VERSION >= 3
extern "C" {
	PyMODINIT_FUNC PyInit__macelib(void);
}
#else
extern "C" {
	void init_macelib(void);
}
#endif

static char* strGET(PyObject* data){
#if PY_MAJOR_VERSION >= 3
	return PyUnicode_AsUTF8(data);
#else		
	return PyString_AsString(data);
#endif

}
/*
mace MCqVe [options] input-filename [output-filename]
%:show progress, _:no message, +:write solutions in append mode
C:enumerate cliques, M:enumerate maximal cliques, e:edge_list format
[options]
-l [num]:output cliques with size at least [num]
-u [num]:output cliques with size at most [num]
-S [num]:stop after outputting [num] solutions
-, [char]:give the separator of the numbers in the output
-Q [filename]:replace the output numbers according to the permutation table given by [filename]
if the 1st letter of input-filename is '-', be considered as 'parameter list'
*/
PyObject* mace_run(PyObject* self, PyObject* args, PyObject* kwds){
	try{
		char * paraLIST[]={ 
			"type","i","o","l","u","S","Q","separator",NULL
		};//8

		char * paraLIST_i[]={
			"","","",
			"-l","-u","-S","-,","-Q",""
		};

		// stop=> # , separator => ,
	 char * pval[8];
	 
	 unsigned int maxParaCnt=8;
	 unsigned int singleParaCnt=3;
	 unsigned int paraCnt=0;
	 unsigned int vsize=1;

 	for(unsigned int i=0;i<maxParaCnt;i++){ pval[i]=NULL;}

	if (!PyArg_ParseTupleAndKeywords(args, kwds, "sss|sssss", paraLIST, 
					&pval[0],&pval[1],&pval[2],&pval[3],&pval[4],
					&pval[5],&pval[6],&pval[7])){
			return NULL;
		}

		for(unsigned int i=0;i<maxParaCnt;i++ ){
			if(pval[i]!=NULL){
				if(i<singleParaCnt){vsize++;}
				else{ vsize +=2;}
			}
		}

		char** vv = new char*[vsize];
		unsigned int pos = 0;
		vv[pos++]=const_cast<char*>("mace");
		vv[pos++]= pval[0];
		for(unsigned int i=singleParaCnt; i<maxParaCnt;i++ ){
			if(pval[i]!=NULL){
				vv[pos++]=paraLIST_i[i]; 
				vv[pos++]=pval[i];
			}
		}
		vv[pos++]=pval[1];
	  if(pval[2]!=NULL){ vv[pos++]=pval[2];}

		//for(int i=0; i<pos;i++){ printf("%s ",vv[i]); }
		//printf("\n");

		MACE_main(vsize,vv);

		if(vv){ delete[] vv;}

		return PyLong_FromLong(0);

	}
	catch(...){
//		std::cerr << "exceptipn" << std::endl;
		return PyLong_FromLong(1);
	}
}

static PyMethodDef takemethods_mace[] = {
	{"mace_run", reinterpret_cast<PyCFunction>(mace_run), METH_VARARGS|METH_KEYWORDS  },
	{NULL}
};

#if PY_MAJOR_VERSION >= 3
static struct PyModuleDef moduledef = {
    PyModuleDef_HEAD_INIT,
    "_macelib",      /* m_name */
    NULL,							     /* m_doc */
    -1,                  /* m_size */
    takemethods_mace,      /* m_methods */
    NULL,                /* m_reload */
    NULL,                /* m_traverse */
    NULL,                /* m_clear */
    NULL,                /* m_free */
};

PyMODINIT_FUNC
PyInit__macelib(void){
	PyObject* m;
	m = PyModule_Create(&moduledef);
	return m;
}

#else

void init_macelib(void){
	Py_InitModule("_macelib", takemethods_mace);
}

#endif