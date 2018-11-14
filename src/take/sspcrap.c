
#include "Python.h"
#include "sspc/sspc.c"

#if PY_MAJOR_VERSION >= 3
PyMODINIT_FUNC PyInit__sspclib(void);
#else
void init_sspclib(void);
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


static int strCHECK(PyObject* data){
#if PY_MAJOR_VERSION >= 3
	return PyUnicode_Check(data);
#else		
	return PyString_Check(data);
#endif
}
/*
const char * paraLIST[]={ 
	"type","i","th","o","2","9","K","k","w","W","l","u","L","U",
	"c","b","B","T","TT","Q","stop","separator",NULL
};//22

const char * paraLIST_i[]={
	"","","","",
	"-2","-9","-K","-k","-w","-W","-l","-u","-L","-U",
	"-c","-b","-B","-T","-TT","-Q","-#","-,",""
};//18

*/

PyObject* sspc_run(PyObject* self, PyObject* args)
{

	PyObject *params;

	if(!PyArg_ParseTuple(args, "O", &params)){ 
		PyErr_SetString(PyExc_RuntimeError,"parameter ERROR");
		PyErr_Print();
		return PyLong_FromLong(1);
	}//err
	if(!PyList_Check(params)){
		PyErr_SetString(PyExc_RuntimeError,"parameter ERROR");
		PyErr_Print();
		return PyLong_FromLong(1); 
	}//err

	Py_ssize_t psize = PyList_Size(params);
	char** vv = (char**)malloc(sizeof(char*)*(psize+1));
	if(vv==NULL){
		// ERROR
		PyErr_SetString(PyExc_RuntimeError,"Memory alloc ERROR");
		PyErr_Print();
		return PyLong_FromLong(1);
	}

	vv[0] = "sspc";
	Py_ssize_t i;
	for(i=0 ; i< psize;i++){
		PyObject *param = PyList_GetItem(params ,i);
		if(strCHECK(param)){
			vv[i+1] = strGET(param);
		}
		else{
			PyErr_SetString(PyExc_RuntimeError,"parameter ERROR : not str");
			if(vv){ free(vv); }
			return PyLong_FromLong(1); 
		}
	}

	// DEBUG
	//for(int i=0; i<pos;i++){ printf("%s ",vv[i]); }
	//printf("\n");

	int sts = SSPC_main(psize+1,vv);

	//if(sts){//ERRORにはしない
	//	PyErr_SetString(PyExc_RuntimeError,"TAKE Module RUN ERROR");
	//	PyErr_Print();
	//}
	if(vv){ free(vv);}
	return PyLong_FromLong(sts);
}

/*
PyObject* sspc_run_dict(PyObject* self, PyObject* args, PyObject* kwds)
{

	PyObject *params;

		// stop=> # , separator => ,
	char * pval[22];
	 
	const	unsigned int maxParaCnt=22;
	const	unsigned int singleParaCnt=4;
	const unsigned int nlimit=3;
	const unsigned int spParaNo = 5;

	unsigned int vsize=1;

	for(unsigned int i=0;i<maxParaCnt;i++){ pval[i]=NULL;}

	if(!PyArg_ParseTuple(args, "O", &params)){ 
		PyErr_SetString(PyExc_RuntimeError,"parameter ERROR");
		PyErr_Print();
		return PyLong_FromLong(1);
	}//err
	if(!PyDict_Check(params)){
		PyErr_SetString(PyExc_RuntimeError,"parameter ERROR");
		PyErr_Print();
		return PyLong_FromLong(1); 
	}//err

	PyObject *key, *value;
	Py_ssize_t ppos = 0;
	while (PyDict_Next(params, &ppos, &key, &value)) {
		if(strCHECK(key)&&strCHECK(value)){
			char *k = strGET(key);
			char *v = strGET(value);
			//パラメータチェック
			for(unsigned int i=0;i<maxParaCnt;i++ ){
				if(!strcmp(k,paraLIST[i])){ pval[i] = v; break;}
			}
		}
	}
	for(unsigned int i=0;i<maxParaCnt;i++ ){
		if(pval[i]!=NULL){
			if(i<singleParaCnt){vsize++;}
			else{ vsize +=2;}
		}
		else if(i<nlimit){
			PyErr_SetString(PyExc_RuntimeError,"nessaery parameter ERROR");
			PyErr_Print();
			return PyLong_FromLong(1);
		}
	}
	for(unsigned int i=0;i<maxParaCnt;i++ ){
		if(pval[i]!=NULL){
			if(i<singleParaCnt){vsize++;}
			else{ vsize +=2;}
			if(i==spParaNo){vsize++;}
		}
	}
	
	// ここ以下は同じ
	char** vv = (char**)malloc(sizeof(char*)*(vsize));
	if(vv==NULL){
		// ERROR
		PyErr_SetString(PyExc_RuntimeError,"Memory alloc ERROR");
		PyErr_Print();
		return PyLong_FromLong(1);
	}
	unsigned int pos = 0;
	vv[pos++]="sspc";
	vv[pos++]= pval[0];
	for(unsigned int i=singleParaCnt; i<maxParaCnt;i++ ){
		if(pval[i]!=NULL){
				vv[pos++]=(char*)paraLIST_i[i]; 
				vv[pos++]=pval[i];
				if(i==spParaNo){
					
					
				}
		}
	}
	vv[pos++]=pval[1];
	vv[pos++]=pval[2];
	if(pval[3]!=NULL){ vv[pos++]=pval[3];}

	// DEBUG
	//for(int i=0; i<pos;i++){ printf("%s ",vv[i]); }
	//printf("\n");

	int sts = SSPC_main(vsize,vv);
	if(sts){
		PyErr_SetString(PyExc_RuntimeError,"TAKE Module RUN ERROR");
		PyErr_Print();
	}

	if(vv){ free(vv);}
	return PyLong_FromLong(sts);
}
*/
/*
PyObject* sspc_run(PyObject* self, PyObject* args, PyObject* kwds)
{
	try{
		const char * paraLIST[]={ 
			"type","i","th","o","2","9","K","k","w","W","l","u","L","U",
			"c","b","B","T","TT","Q","stop","separator",NULL
		};//22

		const char * paraLIST_i[]={
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

		if (!PyArg_ParseTupleAndKeywords(args, kwds, "sss|sssssssssssssssssss", 
					const_cast<char**>(paraLIST), 
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
				vv[pos++]=const_cast<char*>(paraLIST_i[i]); 
				vv[pos++]=pval[i];
			}
		}
		vv[pos++]=pval[1];
		vv[pos++]=pval[2];
		if(pval[3]!=NULL){ vv[pos++]=pval[3];}

		//for(int i=0; i<pos;i++){ printf("%s ",vv[i]); }
		//printf("\n");

		int sts = SSPC_main(vsize,vv);
		if(vv){ delete[] vv;}
		return PyLong_FromLong(sts);

	}
	catch(...){
		//std::cerr << "exceptipn" << std::endl;
		return PyLong_FromLong(1);
	}
}
*/
static PyMethodDef takemethods_sspc[] = {
	{"sspc_run", (PyCFunction)sspc_run, METH_VARARGS  },
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