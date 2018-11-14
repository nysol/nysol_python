#include "Python.h"
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <stdio.h>
#include "lcm/lcm.c"

#if PY_MAJOR_VERSION >= 3
PyMODINIT_FUNC PyInit__lcmlib(void);
#else
void init_lcmlib(void);
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
static const char * paraLIST[]={ 
	"type","i","sup","o","K","l","u","U","w","c","C","item","a","A","r","R",
	"f","F","p","P","n","N","opos","Opos",
	"m","M","Q","stop","separator","so",NULL
};//30

static const char * paraLIST_i[]={
	"","","","",
	"-K","-l","-u","-U","-w","-c","-C","-i","-a","-A","-r","-R",
	"-f","-F","-p","-P","-n","-N","-o","-O","-m","-M","-Q","-#","-,",""
};//26
*/

PyObject* lcm_run(PyObject* self, PyObject* args){

	PyObject *params;
	char *newstdout=NULL;
	if(!PyArg_ParseTuple(args, "O|s", &params,&newstdout)){ 
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

	vv[0] = "lcm";
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

	int backup, fd;
	if(newstdout!=NULL){		// 標準出力きりかえ
		backup = dup(1);
		if(backup<0) { return PyLong_FromLong(errno); }
		fd = open(newstdout, O_WRONLY|O_TRUNC|O_CREAT|O_APPEND, S_IRWXU);
		if(fd<0) { return PyLong_FromLong(errno); }
		int dp2 = dup2(fd, 1);
		if(dp2<0) { return PyLong_FromLong(errno); }
		FILE* newfp = fdopen(fd, "w");
		if(!newfp){return PyLong_FromLong(errno); } 
		stdout = newfp;
	}
	int sts = LCM_main(psize+1,vv);
	//if(sts){ //ERRORにはしない
	//	PyErr_SetString(PyExc_RuntimeError,"TAKE Module RUN ERROR");
	//	PyErr_Print();
	//}
	if(newstdout!=NULL){		// 標準出力きりかえ
		if(fflush(stdout)){ return PyLong_FromLong(errno); }
		if(dup2(backup, 1)){ return PyLong_FromLong(errno); } 
		FILE* bfp = fdopen(backup, "w");
		if(!bfp){return PyLong_FromLong(errno); } 
		stdout = bfp;
		if(close(backup)<0){return PyLong_FromLong(errno); } 
	}
	if(vv){ free(vv); }
	return PyLong_FromLong(sts);
}

/*

PyObject* lcm_run_dict(PyObject* self, PyObject* args){

	PyObject *params;

	char * pval[30];
	 
	const unsigned int maxParaCnt=29;
	const unsigned int singleParaCnt=4;
	const unsigned int nlimit=3;

	unsigned int vsize=1;
	for(unsigned int i=0;i<maxParaCnt+1;i++){ pval[i]=NULL;}

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
			for(unsigned int i=0;i<maxParaCnt+1;i++ ){
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
	if(vv==NULL){
		// ERROR
		PyErr_SetString(PyExc_RuntimeError,"Memory alloc ERROR");
		PyErr_Print();
		return PyLong_FromLong(1);
	}
	unsigned int pos = 0;
	vv[pos++]="lcm";
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

	int backup, fd;
	if(pval[29]!=NULL){		// 標準出力きりかえ
		backup = dup(1);
		if(backup<0) { return PyLong_FromLong(errno); }
		fd = open(pval[29], O_WRONLY|O_TRUNC|O_CREAT|O_APPEND, S_IRWXU);
		if(fd<0) { return PyLong_FromLong(errno); }
		int dp2 = dup2(fd, 1);
		if(dp2<0) { return PyLong_FromLong(errno); }
		FILE* newfp = fdopen(fd, "w");
		if(!newfp){return PyLong_FromLong(errno); } 
		stdout = newfp;
	}
	int sts = LCM_main(vsize,vv);
	if(sts){
		PyErr_SetString(PyExc_RuntimeError,"TAKE Module RUN ERROR");
		PyErr_Print();
	}
	if(pval[29]!=NULL){		// 標準出力きりかえ
		if(fflush(stdout)){ return PyLong_FromLong(errno); }
		if(dup2(backup, 1)){ return PyLong_FromLong(errno); } 
		FILE* bfp = fdopen(backup, "w");
		if(!bfp){return PyLong_FromLong(errno); } 
		stdout = bfp;
		if(close(backup)<0){return PyLong_FromLong(errno); } 
	}
	if(vv){ free(vv); }
	return PyLong_FromLong(sts);
}
*/
/*
PyObject* lcm_run(PyObject* self, PyObject* args, PyObject* kwds){
	try{

	 char * pval[30];
	 
	 unsigned int maxParaCnt=29;
	 unsigned int singleParaCnt=4;
	 unsigned int vsize=1;

 	for(unsigned int i=0;i<maxParaCnt+1;i++){ pval[i]=NULL;}

	if (!PyArg_ParseTupleAndKeywords(args, kwds, "sss|sssssssssssssssssssssssssss",
					const_cast<char**>(paraLIST), 
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

		char** vv = new char*[vsize];
		unsigned int pos = 0;
		vv[pos++]=const_cast<char*>("lcm");
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

		int backup, fd;
		if(pval[29]!=NULL){		// 標準出力きりかえ
			backup = dup(1);
			fd = open(pval[29], O_WRONLY|O_TRUNC|O_CREAT|O_APPEND, S_IRWXU);
			dup2(fd, 1);
 			stdout = fdopen(fd, "w");
		}
		int sts = LCM_main(vsize,vv);
		if(pval[29]!=NULL){		// 標準出力きりかえ
			fflush (stdout);
			dup2(backup, 1); 
		 	stdout = fdopen(backup, "w");
			close(backup);
		}
		if(vv) { delete[] vv; }
		return PyLong_FromLong(sts);

	}
	catch(...){
		//std::cerr << "exceptipn" << std::endl;
		return PyLong_FromLong(1);
	}
}
*/

static PyMethodDef takemethods[] = {
//	{"lcm_run", reinterpret_cast<PyCFunction>(lcm_run), METH_VARARGS|METH_KEYWORDS  },
	{"lcm_run", (PyCFunction)lcm_run, METH_VARARGS  },
	{NULL}
};

#if PY_MAJOR_VERSION >= 3
static struct PyModuleDef moduledef = {
    PyModuleDef_HEAD_INIT,
    "_lcmlib",      /* m_name */
    NULL,							     /* m_doc */
    -1,                  /* m_size */
    takemethods,      /* m_methods */
    NULL,                /* m_reload */
    NULL,                /* m_traverse */
    NULL,                /* m_clear */
    NULL,                /* m_free */
};

PyMODINIT_FUNC
PyInit__lcmlib(void){
	PyObject* m;
	m = PyModule_Create(&moduledef);
	return m;
}

#else

void init_lcmlib(void){
	Py_InitModule("_lcmlib", takemethods);
}

#endif

