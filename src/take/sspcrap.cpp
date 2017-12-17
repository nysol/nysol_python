
#include "Python.h"

namespace {
extern "C" {
#include "sspc/sspc.c"
}
}

#if PY_MAJOR_VERSION >= 3
extern "C" {
	PyMODINIT_FUNC PyInit__sspclib(void);
}
#else
extern "C" {
	void init_sspclib(void);
}
#endif

static char* strGET(PyObject* data){
#if PY_MAJOR_VERSION >= 3
	return PyUnicode_AsUTF8(data);
#else		
	return PyString_AsString(data);
#endif

}

PyObject* sspc_run(PyObject* self, PyObject* args, PyObject* kwds)
{
	try{
		char * paraLIST[]={ 
			"type","i","th","o","2","9","K","k","w","W","l","u","L","U",
			"c","b","B","T","TT","Q","stop","separator",NULL
		};//22

		char * paraLIST_i[]={
			"","","","",
			"-2","-9","-K","-k","-w","-W","-l","-u","-L","-U",
			"-c","-b","-B","-T","-TT","-Q","-#","-,",""
		};//18

		// stop=> # , separator => ,
		char * pval[22];
	 
		unsigned int maxParaCnt=22;
		unsigned int singleParaCnt=4;
		unsigned int paraCnt=0;
		unsigned int vsize=1;

		for(unsigned int i=0;i<maxParaCnt;i++){ pval[i]=NULL;}

		if (!PyArg_ParseTupleAndKeywords(args, kwds, "sss|sssssssssssssssssss", paraLIST, 
					&pval[0],&pval[1],&pval[2],&pval[3],&pval[4],
					&pval[5],&pval[6],&pval[7],&pval[8],&pval[9],
					&pval[10],&pval[11],&pval[12],&pval[13],&pval[14],
					&pval[15],&pval[16],&pval[17],&pval[18],&pval[19],
					&pval[20],&pval[21])){
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
		vv[pos++]=const_cast<char*>("sspc");
		vv[pos++]= pval[0];
		for(unsigned int i=singleParaCnt; i<maxParaCnt;i++ ){
			if(pval[i]!=NULL){
				vv[pos++]=paraLIST_i[i]; 
				vv[pos++]=pval[i];
			}
		}
		vv[pos++]=pval[1];
		vv[pos++]=pval[2];
		if(pval[3]!=NULL){ vv[pos++]=pval[3];}

		//for(int i=0; i<pos;i++){ printf("%s ",vv[i]); }
		//printf("\n");

		SSPC_main(vsize,vv);
		if(vv){ delete[] vv;}
		return PyLong_FromLong(0);

	}
	catch(...){
		//std::cerr << "exceptipn" << std::endl;
		return PyLong_FromLong(1);
	}
}

static PyMethodDef takemethods_sspc[] = {
	{"sspc_run", reinterpret_cast<PyCFunction>(sspc_run), METH_VARARGS|METH_KEYWORDS  },
	{NULL}
};

#if PY_MAJOR_VERSION >= 3
static struct PyModuleDef moduledef = {
    PyModuleDef_HEAD_INIT,
    "_sspclib",      /* m_name */
    NULL,							     /* m_doc */
    -1,                  /* m_size */
    takemethods_sspc,      /* m_methods */
    NULL,                /* m_reload */
    NULL,                /* m_traverse */
    NULL,                /* m_clear */
    NULL,                /* m_free */
};
PyMODINIT_FUNC
PyInit__sspclib(void){
	PyObject* m;
	m = PyModule_Create(&moduledef);
	return m;
}
#else

void init_sspclib(void){
	Py_InitModule("_sspclib", takemethods_sspc);
}

#endif