
#include "Python.h"

#include "medset/medset.c"

#if PY_MAJOR_VERSION >= 3
	PyMODINIT_FUNC PyInit__medsetlib(void);
#else
	void init_medsetlib(void);
#endif
/*
static char* strGET(PyObject* data){
#if PY_MAJOR_VERSION >= 3
	return PyUnicode_AsUTF8(data);
#else		
	return PyString_AsString(data);
#endif

}*/
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
static const char * paraLIST[]={ 
			"ci","si","th","o",
			"H","R","V","T","I","i","t",
			"progress","nomsg","l",NULL
};//12

static const char * paraLIST_i[]={
	"","","",	"",
	"-H","-R","-V","-T","-I","-i",
	"-t","-%","-_","-l",""
};

*/

PyObject* medset_run(PyObject* self, PyObject* args){

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

	vv[0] = "medset";
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

	int sts = MEDSET_main(psize+1,vv);
	//if(sts){//ERRORにはしない
	//	PyErr_SetString(PyExc_RuntimeError,"TAKE Module RUN ERROR");
	//	PyErr_Print();
	//}

	if(vv){ free(vv);}
	return PyLong_FromLong(sts);

}


/*
usage: medset [-HRTIitl] cluster-filename set-filename threshold output-filename
if threshold is negative, output the items whose frequencies are no more than the threshold
-%: show progress, -_: no message
-H: do not use histgram, just output the items
-R: do not output singleton clusters
-V: output ratio of appearances of all items
-T: clustering by connected component (read edge type file)
-I: find an independent set, and clustering by using the vertices in it as seeds (read edge type files)
-i: output for each item, ratio of records including the item
-t: transpose the input database, (transaction will be item, and vice varsa)
-l [num]: output clusters of size at least [num]
# the 1st letter of input-filename cannot be '-'.
*/
/*
PyObject* medset_run_dict(PyObject* self, PyObject* args){

	PyObject *params;

		// stop=> # , separator => ,
	char * pval[14];
	 
	unsigned int maxParaCnt=14;

	const unsigned int singleParaCnt=13;
	const unsigned int nlimit=4;

	unsigned int paraCnt=0;
	unsigned int vsize=1;

	for(unsigned int i=0;i<maxParaCnt;i++){ pval[i]=NULL;}

	if (!PyArg_ParseTuple(args, "O", &params)){ 
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

	// ここ以下は同じ
	char** vv = (char**)malloc(sizeof(char*)*(vsize));
	unsigned int pos = 0;
	vv[pos++]="simset";
	vv[pos++]= pval[0];
	for(unsigned int i=nlimit; i<maxParaCnt;i++ ){
		if(pval[i]!=NULL){
			if(i<singleParaCnt){
				if(!strcmp(pval[i],"True")||!strcmp(pval[i],"true")){
					vv[pos++]=(char *)(paraLIST_i[i]); 
				}
			}
			else{
				vv[pos++]=(char *)(paraLIST_i[i]); 
				vv[pos++]=pval[i];
			}
		}
	}
	vv[pos++]=pval[1];
	vv[pos++]=pval[2];
	if(pval[3]!=NULL){ vv[pos++]=pval[3];}

	// DEBUG
	//for(int i=0; i<pos;i++){ printf("%s ",vv[i]); }
	//printf("\n");

	int sts = MEDSET_main(vsize,vv);
	if(sts){
		PyErr_SetString(PyExc_RuntimeError,"TAKE Module RUN ERROR");
		PyErr_Print();
	}

	if(vv){ free(vv);}
	return PyLong_FromLong(sts);

}
*/

/*
PyObject* medset_run(PyObject* self, PyObject* args, PyObject* kwds){

	// stop=> # , separator => ,
	//	 char * pval[5];
	 char * pval[14];
	 int pval_b[9];
	 
	 unsigned int maxParaCnt=14;
	 unsigned int stringParaCnt=5;

	 unsigned int singleParaCnt=4;
	 unsigned int binaryParaCnt=9;

	 unsigned int paraCnt=0;
	 unsigned int vsize=1;
 	for(unsigned int i=0;i<maxParaCnt;i++){ pval[i]=NULL;}
 	for(unsigned int i=0;i<binaryParaCnt;i++){ pval_b[i]=0;}


	if (!PyArg_ParseTupleAndKeywords(args, kwds, "ssss|siiiiiiiii", 
//	if (!PyArg_ParseTupleAndKeywords(args, kwds, "ssss|ssssssssss", 
					(char**)paraLIST, 
//					&pval[0],&pval[1],&pval[2],&pval[3],&pval[4],
//					&pval[5],&pval[6],&pval[7],&pval[8],&pval[9],
//					&pval[10],&pval[11],&pval[12],&pval[13])){
					&pval[0],&pval[1],&pval[2],&pval[3],&pval[4],
					&pval_b[0],&pval_b[1],&pval_b[2],&pval_b[3],&pval_b[4],
					&pval_b[5],&pval_b[6],&pval_b[7],&pval_b[8])){
			return NULL;
		}
	printf("xxx3-9 %d\n",pval_b[0]);
		for(unsigned int i=0;i<maxParaCnt;i++ ){
			if(i<stringParaCnt){
				if(pval[i]!=NULL){
					if(i<singleParaCnt){vsize++;}
					else{ vsize +=2;}
				}
			}
			else if(pval_b[i-stringParaCnt]!=NULL){
				vsize++;
			}
		}

	printf("xxx4\n");
		//char** vv = new char*[vsize];
		char** vv = (char**)malloc(sizeof(char*)*(vsize));
		unsigned int pos = 0;
		vv[pos++]=(char*)("medest");
		vv[pos++]= pval[0];
		for(unsigned int i=singleParaCnt; i<maxParaCnt;i++ ){
			if(i<stringParaCnt){
				if(pval[i]!=NULL){
					vv[pos++]=(char*)paraLIST_i[i]; 
					vv[pos++]=pval[i];
				}
			}
			else if(pval_b[i-stringParaCnt]!=0){
				vv[pos++]=(char*)paraLIST_i[i]; 
			}
		}
		vv[pos++]=pval[1];
	  vv[pos++]=pval[2];
	  vv[pos++]=pval[3];

		for(int i=0; i<pos;i++){ printf("%s ",vv[i]); }
		printf("\n");
	printf("xxx4\n");

		int sts = MEDSET_main(vsize,vv);

		//if(vv){ delete[] vv;}
		if(vv){ free(vv);}

		return PyLong_FromLong(sts);

}
*/
static PyMethodDef takemethods_medset[] = {
	{"medset_run", (PyCFunction)medset_run, METH_VARARGS  },
//	{"medset_runByDict", reinterpret_cast<PyCFunction>(medset_run_dict), METH_VARARGS  },
	{NULL}
};

#if PY_MAJOR_VERSION >= 3
static struct PyModuleDef moduledef = {
    PyModuleDef_HEAD_INIT,
    "_medsetlib",      /* m_name */
    NULL,							     /* m_doc */
    -1,                  /* m_size */
    takemethods_medset,      /* m_methods */
    NULL,                /* m_reload */
    NULL,                /* m_traverse */
    NULL,                /* m_clear */
    NULL,                /* m_free */
};

PyMODINIT_FUNC
PyInit__medsetlib(void){
	PyObject* m;
	m = PyModule_Create(&moduledef);
	return m;
}

#else

void init_medsetlib(void){
	Py_InitModule("_medsetlib", takemethods_medset);
}

#endif