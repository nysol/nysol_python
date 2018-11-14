#include "Python.h"
#include "simset/simset.c"


#if PY_MAJOR_VERSION >= 3
	PyMODINIT_FUNC PyInit__medestlib(void);
#else
	void init_medestlib(void);
#endif

static int strCHECK(PyObject* data){

#if PY_MAJOR_VERSION >= 3
	return PyUnicode_Check(data);
#else		
	return PyString_Check(data);
#endif

}

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


/*
simset ISCMOt [options] similarity-graph-filename similarity-threshold degree-threshold output-filename
%:show progress, _:no message, +:write solutions in append mode, =:do not remove temporal files
@:do not execute data polishing, E:read edge list file
i:set similarity measure to the ratio of one is included in the other
I:set similarity measure to the ratio of both are included in the other
S:set similarity measure to |A\cap B|/max(|A|,|B|)
s:set similarity measure to |A\cap B|/min(|A|,|B|)
C:set similarity measure to the cosign distance, the inner product of the normalized characteristic vectors
T:set similarity measure to the intersection size, i.e., |A\cap B|
R:(recemblance) set similarity measure to |A\cap B|/|A\cup B|
P(PMI): set similarity measure to log (|A\capB|*|all| / (|A|*|B|)) where |all| is the number of all items
M:output intersection of each clique, instead of IDs of its members
v (with M): output ratio of records, including each item
m:do not remove edges in the data polishing phase
O:repeatedly similarity clustering until convergence
Y:take intersection of original graph and polished graph, instead of clique mining
1:do not remove the same items in a record (with -G)
W:load weight of each element
t:transpose the input database, so that each line will be considered as a record

[options]
-G [similarity] [threshold]:use [similarity] of [threshold] in the first phase (file is regarded as a transaction database)
-k [threshold]:find only k-best for each record in -G option
-M [num]:merge similar cliques of similarity in [num] of recemblance (changes to 'x' by giving '-Mx')
-m [num]:take independently cliques from similar cliques of similarity in [num] of recemblance, and merge the neighbors of each independent clique ('recemblance' changes to 'x' by giving '-Mx')
-v [num]:specify majority threshold (default=0.5) (if negative is given, items of frequency no more than -[num] are output)
-u [num]:ignore vertices of degree more than [num]
-l [num]:ignore vertices of degree less than [num]
-U [num]:ignore transactions of size more than [num] (with -G)
-L [num]:ignore transactions of size less than [num] (with -G)
-I [num]:ignore items of frequency more than [num] (with -G)
-i [num]:ignore items of frequency less than [num] (with -G)
 (-II,-ii [ratio]:give thresholds by the ratio of #ransactions/#items)
-T,t [num]:ignore pairs whose intersection size is less than [num] (T for first phase with -G option, and t for polishing)
-O [num]:specify the number of repetitions
-9 [num]:shrink records of similarity more than [num]
-X [num]:multiply the weight by [num] (and trancate by 1, for C command)
-x [num]:power the weight by [num] (and normalize, for C command)
-y [num]:at last, remove edges with weight less than [num]
-w [filename]:load weight of elements from the file
-! [num]:use multicores of [num] (compile by 'make multicore')
-W [dir]:specify the working directory (folder). The last letter of the directory has to be '/' ('\')
-, [char]:give the separator of the numbers in the output
-Q [filename]:replace the output numbers according to the permutation table given by [filename]
# the 1st letter of input-filename cannot be '-'.
if similarity-threshold is 0, skip the similarity graph construction phase
*/
/*
static const char * paraLIST[]={ 
		"type","i","th_s","th_d","o","G","k","M","m","v","u","l","U",
		"L","Iig","iig","II","ii","T","t","O","9","X","x","y",
		"w","multi","W","separator","-Q",NULL
};//28

static const char * paraLIST_i[]={
	"","","","","","-G","-k","-M","-m","-v","-u","-l","-U"
	"-L","-I","-i","-II","-ii","-T","-t","-O","-9","-X",
	"-x","-y","-w","-!","-W","-,","-Q",""
};//24
*/
PyObject* simset_run(PyObject* self, PyObject* args){


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

	vv[0] = "simset";
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

	//DEBUG
	//for(int i=0; i<pos;i++){ printf("%s ",vv[i]); }
	//printf("\n");

	int sts = SIMSET_main(psize+1,vv);
	//if(sts){//ERRORにはしない
	//	PyErr_SetString(PyExc_RuntimeError,"TAKE Module RUN ERROR");
	//	PyErr_Print();
	//}

	if(vv){ free(vv);}
	return PyLong_FromLong(sts);

}

/*
PyObject* simset_run_dict(PyObject* self, PyObject* args){

	PyObject *params;

	char * pval[30];
	 
	const unsigned int maxParaCnt=30;
	const unsigned int singleParaCnt=5;
	const unsigned int nlimit=5;

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

		// ここ以下は同じ
	char** vv = (char**)malloc(sizeof(char*)*(vsize));
	unsigned int pos = 0;
	vv[pos++]="simset";
	vv[pos++]= pval[0];
	for(unsigned int i=singleParaCnt; i<maxParaCnt;i++ ){
		if(pval[i]!=NULL){
			vv[pos++]=(char *)(paraLIST_i[i]); 
			vv[pos++]=pval[i];
		}
	}
	vv[pos++]=pval[1];
	vv[pos++]=pval[2];
	vv[pos++]=pval[3];
	vv[pos++]=pval[4];

	//DEBUG
	//for(int i=0; i<pos;i++){ printf("%s ",vv[i]); }
	//printf("\n");

	int sts = SIMSET_main(vsize,vv);
	if(sts){
		PyErr_SetString(PyExc_RuntimeError,"TAKE Module RUN ERROR");
		PyErr_Print();
	}

	if(vv){ free(vv);}
	return PyLong_FromLong(sts);

}
*/

/*
PyObject* simset_run(PyObject* self, PyObject* args, PyObject* kwds){

//	try{


	 char * pval[30];
	 
	 unsigned int maxParaCnt=30;
	 unsigned int singleParaCnt=5;
	 unsigned int vsize=1;

 	for(unsigned int i=0;i<maxParaCnt;i++){ pval[i]=NULL;}

	if (!PyArg_ParseTupleAndKeywords(args, kwds, "sssss|sssssssssssssssssssssssss", 
					(char**)paraLIST, 
					&pval[0],&pval[1],&pval[2],&pval[3],&pval[4],
					&pval[5],&pval[6],&pval[7],&pval[8],&pval[9],
					&pval[10],&pval[11],&pval[12],&pval[13],&pval[14],
					&pval[15],&pval[16],&pval[17],&pval[18],&pval[19],
					&pval[20],&pval[21],&pval[22],&pval[23],&pval[24],
					&pval[25],&pval[26],&pval[27],&pval[28],&pval[29])){
			return NULL;
		}

		for(unsigned int i=0;i<maxParaCnt;i++ ){
			if(pval[i]!=NULL){
				if(i<singleParaCnt){vsize++;}
				else{ vsize +=2;}
			}
		}

//		char** vv = new char*[vsize];
		char** vv = (char**)malloc(sizeof(char*)*(vsize));
		unsigned int pos = 0;
		vv[pos++]="simset";
		vv[pos++]= pval[0];
		for(unsigned int i=singleParaCnt; i<maxParaCnt;i++ ){
			if(pval[i]!=NULL){
				vv[pos++]=(char *)(paraLIST_i[i]); 
				vv[pos++]=pval[i];
			}
		}
		vv[pos++]=pval[1];
		vv[pos++]=pval[2];
		vv[pos++]=pval[3];
		vv[pos++]=pval[4];

		for(int i=0; i<pos;i++){ printf("%s ",vv[i]); }
		printf("\n");

		int sts = SIMSET_main(vsize,vv);
		if(vv){ free(vv);}

		return PyLong_FromLong(sts);

	}
	catch(...){
		//	std::cerr << "exceptipn" << std::endl;
		return PyLong_FromLong(1);
	}
}

*/
static PyMethodDef takemethods_simset[] = {
//	{"simset_run", (PyCFunction)simset_run, METH_VARARGS|METH_KEYWORDS },
	{"simset_run", (PyCFunction)simset_run, METH_VARARGS  },
	{NULL}
};

#if PY_MAJOR_VERSION >= 3
static struct PyModuleDef moduledef = {
    PyModuleDef_HEAD_INIT,
    "_simsetlib",      /* m_name */
    NULL,							     /* m_doc */
    -1,                  /* m_size */
    takemethods_simset,      /* m_methods */
    NULL,                /* m_reload */
    NULL,                /* m_traverse */
    NULL,                /* m_clear */
    NULL,                /* m_free */
};

PyMODINIT_FUNC
PyInit__simsetlib(void){
	PyObject* m;
	m = PyModule_Create(&moduledef);
	return m;
}

#else

void init_simsetlib(void){
	Py_InitModule("_simsetlib", takemethods_simset);
}

#endif