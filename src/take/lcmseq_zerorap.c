#include "Python.h"
#include "lcmseq/lcm_seq_zero.c"

#if PY_MAJOR_VERSION >= 3
PyMODINIT_FUNC PyInit__lcmseq_zerolib(void);
#else
void init_lcmseq_zerolib(void);
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
LCMseq: [FCfQIq] [options] input-filename support [output-filename]
%:show progress, _:no message, +:write solutions in append mode
F:position occurrence, C:document occurrence
m:output extension maximal patterns only, c:output extension closed patterns only
 f,Q:output frequency following/preceding to each output sequence
A:output coverages for positive/negative transactions
I(J):output ID's of transactions including each pattern, if J is given, an occurrence is written in a complete stype; transaction ID, starting position and ending position
i:do not output itemset to the output file (only rules)
s:output confidence and item frequency by absolute values
t:transpose the input database (item i will be i-th transaction, and i-th transaction will be item i)
[options]
-K [num]: output [num] most frequent sequences
-l,-u [num]: output sequences with size at least/most [num]
-U [num]: upper bound for support(maximum support)
-g [num]: restrict gap length of each consequtive items by [num]
-G [num]: restrict window size of the occurrence by [num]
-w [filename]:read weights of transactions from the file
-i [num]: find association rule for item [num]
-a,-A [ratio]: find association rules of confidence at least/most [ratio]
-r,-R [ratio]: find association rules of relational confidence at least/most [ratio]
-f,-F [ratio]: output sequences with frequency no less/greater than [ratio] times the frequency given by the product of appearance probability of each item
-p,-P [num]: output sequence only if (frequency)/(abusolute frequency) is no less/no greater than [num]
-n,-N [num]: output sequence only if its negative frequency is no less/no greater than [num] (negative frequency is the sum of weights of transactions having negative weights)
-o,-O [num]: output sequence only if its positive frequency is no less/no greater than [num] (positive frequency is the sum of weights of transactions having positive weights)
-s [num]: output itemset rule (of the form (a,b,c) => (d,e)) with confidence at least [num] (only those whose frequency of the result is no less than the support)
-# [num]: stop after outputting [num] solutions
-, [char]:give the separator of the numbers in the output
-Q [filename]:replace the output numbers according to the permutation table given by [filename]
# the 1st letter of input-filename cannot be '-'.
# if the output file name is -, the solutions will be output to standard output.
*/
/*static const char * paraLIST[]={ 
	"type","i","sup","o","K","l","u","U","g","G","w","item","a","A","r","R",
	"f","F","p","P","n","N","opos","Opos","s",
	"Q","stop","separator",NULL
};//29

static const char * paraLIST_i[]={
	"","","","",
	"-K","-l","-u","-U","-g","-G","-w","-i","-a","-A","-r","-R",
	"-f","-F","-p","-P","-n","-N","-o","-O","-s","-Q","-#","-,",""
};//26
*/

PyObject* lcmseq_zero_run(PyObject* self, PyObject* args)
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

	vv[0] = "lcm_seq";
	Py_ssize_t i=0 ;
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

	int sts = LCMseq_main(psize+1,vv);
	//if(sts){//ERRORにはしない
	//	PyErr_SetString(PyExc_RuntimeError,"TAKE Module RUN ERROR");
	//	PyErr_Print();
	//}

	if(vv){ free(vv);}

	return PyLong_FromLong(sts);

}

/*
PyObject* lcmseq_zero_run_dict(PyObject* self, PyObject* args){

	PyObject *params;

	char * pval[28];
	 
	const unsigned int maxParaCnt=28;
	const unsigned int singleParaCnt=4;
	const unsigned int nlimit=3;

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
	vv[pos++]="lcm_seq";
	vv[pos++]= pval[0];
	for(unsigned int i=singleParaCnt; i<maxParaCnt;i++ ){
		if(pval[i]!=NULL){
			vv[pos++]=(char*)paraLIST_i[i]; 
			vv[pos++]=pval[i];
		}
	}
	vv[pos++]=pval[1];
	vv[pos++]=pval[2];
	if(pval[3]!=NULL){ vv[pos++]=pval[3];}

	// DEBUG
	//for(int i=0; i<pos;i++){ printf("%s ",vv[i]); }
	//printf("\n");

	int sts = LCMseq_main(vsize,vv);
	if(sts){
		PyErr_SetString(PyExc_RuntimeError,"TAKE Module RUN ERROR");
		PyErr_Print();
	}

	if(vv){ free(vv);}

	return PyLong_FromLong(sts);

}
*/
/*

PyObject* lcmseq_zero_run(PyObject* self, PyObject* args, PyObject* kwds){
	try{

	 char * pval[28];
	 
	 unsigned int maxParaCnt=28;
	 unsigned int singleParaCnt=4;
	 unsigned int vsize=1;

 	for(unsigned int i=0;i<maxParaCnt;i++){ pval[i]=NULL;}

	if (!PyArg_ParseTupleAndKeywords(args, kwds, "sss|ssssssssssssssssssssssssss", 
					const_cast<char**>(paraLIST), 
					&pval[0],&pval[1],&pval[2],&pval[3],&pval[4],
					&pval[5],&pval[6],&pval[7],&pval[8],&pval[9],
					&pval[10],&pval[11],&pval[12],&pval[13],&pval[14],
					&pval[15],&pval[16],&pval[17],&pval[18],&pval[19],
					&pval[20],&pval[21],&pval[22],&pval[23],&pval[24],
					&pval[25],&pval[26],&pval[27])){
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
		vv[pos++]=const_cast<char*>("lcm_seq");
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

		int sts = LCMseq_main(vsize,vv);

		if(vv){ delete[] vv;}
		
		return PyLong_FromLong(sts);

	}
	catch(...){
		// std::cerr << "exceptipn" << std::endl;
		return PyLong_FromLong(1);
	}
}
*/

static PyMethodDef takemethods[] = {
//	{"lcmseq_zero_run", reinterpret_cast<PyCFunction>(lcmseq_zero_run), METH_VARARGS|METH_KEYWORDS  },
	{"lcmseq_zero_run", (PyCFunction)lcmseq_zero_run, METH_VARARGS  },
	{NULL}
};

#if PY_MAJOR_VERSION >= 3
static struct PyModuleDef moduledef = {
    PyModuleDef_HEAD_INIT,
    "_lcmseq_zerolib",      /* m_name */
    NULL,							     /* m_doc */
    -1,                  /* m_size */
    takemethods,      /* m_methods */
    NULL,                /* m_reload */
    NULL,                /* m_traverse */
    NULL,                /* m_clear */
    NULL,                /* m_free */
};

PyMODINIT_FUNC
PyInit__lcmseq_zerolib(void){
	PyObject* m;
	m = PyModule_Create(&moduledef);
	return m;
}

#else

void init_lcmseq_zerolib(void){
	Py_InitModule("_lcmseq_zerolib", takemethods);
}

#endif