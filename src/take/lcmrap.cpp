
#include "Python.h"
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>
namespace {
extern "C" {
#include "lcm/lcm.c"
}
}

#if PY_MAJOR_VERSION >= 3
extern "C" {
	PyMODINIT_FUNC PyInit__lcmlib(void);
}
#else
extern "C" {
	void init_lcmlib(void);
}
#endif

static char* strGET(PyObject* data){
#if PY_MAJOR_VERSION >= 3
	return PyUnicode_AsUTF8(data);
#else		
	return PyString_AsString(data);
#endif
}

static bool strCHECK(PyObject* data){

#if PY_MAJOR_VERSION >= 3
	return PyUnicode_Check(data);
#else		
	return PyString_Check(data);
#endif

}

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

PyObject* lcm_run_dict(PyObject* self, PyObject* args){

	try{

		PyObject *params;

		char * pval[30];
	 
		unsigned int maxParaCnt=29;
		unsigned int singleParaCnt=4;
		unsigned int vsize=1;

		for(unsigned int i=0;i<maxParaCnt+1;i++){ pval[i]=NULL;}

		if (!PyArg_ParseTuple(args, "O", &params)){ return NULL;}//err
		if(!PyDict_Check(params)){ return NULL; }//err

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
		}

		// ここ以下は同じ
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

		//return PyLong_FromLong(0);
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
		if(vv){ delete[] vv;}
		return PyLong_FromLong(sts);


	}catch(...){
		//std::cerr << "exceptipn" << std::endl;
		return PyLong_FromLong(1);
	}
}

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

static PyMethodDef takemethods[] = {
	{"lcm_run", reinterpret_cast<PyCFunction>(lcm_run), METH_VARARGS|METH_KEYWORDS  },
	{"lcm_runByDict", reinterpret_cast<PyCFunction>(lcm_run_dict), METH_VARARGS  },
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

