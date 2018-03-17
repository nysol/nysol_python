#include "Python.h"
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <stdio.h>

#define MAXC 1024000

#if PY_MAJOR_VERSION >= 3
extern "C" {
	PyMODINIT_FUNC PyInit__lcmtranslib(void);
}
#else
extern "C" {
	void init_lcmtranslib(void);
}
#endif
/*
usage) lcm_trans(filename,p|t,filename)
filename: output file by lcm(lcm_seq)
p: output patterns and their frequency
t: output transaction number with pattern ID

ex. of input file(output file by lcm)\
-----------
 3
 0 1 2
4 2
 0 2
4 8 1
 0
4 8 6 1
 0
-----------
odd line: pattern with frequency
even line: transaction number 

standard output (2nd parameter is 'p')
pattern,count,size,pid
,3,0,0
4,2,1,1
4 8,1,2,2
4 8 6,1,3,3

standard output (2nd parameter is 't')
__tid,pid
0,0
1,0
2,0
0,1
2,1
0,2
0,3
*/

int lcmtrans(char *inf ,char * para ,char *outf){

	char mode = para[0];
	if(mode!='p'&&mode!='e'&&mode!='t'){
		fprintf(stderr,"argument error2: %c\n",mode);
		return -1;
	}
	FILE *fp;
	if((fp=fopen(inf,"rb"))==NULL){
		fprintf(stderr,"file open error\n");
		return EXIT_FAILURE;
	}
	FILE *ofp;
	if(outf==NULL){
		ofp = stdout;
	}
	else{
		if((ofp=fopen(outf,"w"))==NULL){	
			fprintf(stderr,"file open error\n");
			return EXIT_FAILURE;
		}	
	}
	/*
	else if((ofp=fopen(outf,"w"))==NULL){	
		fprintf(stderr,"file open error\n");
		return EXIT_FAILURE;
	}
	*/

	int odd=1; // 奇数行フラグ
	char *buf =NULL; // 出力バッファ
	char *buf2=NULL; // 出力バッファ
	try{
		buf = new char[MAXC];
		buf2 = new char[MAXC];
	}catch(...){
		fprintf(stderr,"memory alloc error\n");
		return -1;
	}

	// enumLcmSeq.rb,enumLcmIs.rbでのパターン
	// 4 5 (2)
	if(mode=='p'){
		fprintf(ofp,"pattern,count,size,pid\n");
		size_t recNo=0;
		//char buf[MAXC]; // 出力バッファ
		//char buf2[MAXC]; // 出力バッファ

		int spcCount=0; // スペースのカウント
		int opos=-1;
		while(1){
			int rsize = fread(buf, sizeof(char), MAXC, fp);
			if( rsize < 0 ){ 
				fprintf(stderr,"file read error\n");
				if(buf) { delete [] buf ;}
				if(buf2) { delete [] buf2 ;}
				return EXIT_FAILURE;
			}
			if( rsize == 0 ) { break;}
			for(int i=0 ; i<rsize ;i++){
				if(buf[i]=='\n'){
					if(odd==1){
						fprintf(ofp,"%s,%d,%ld\n",buf2,spcCount,recNo);
						recNo++;
					}
					spcCount=0;
					opos=-1;
					odd*=(-1);
					continue;
				}
				if(odd!=1) { continue;}
				if(buf[i]==' '){
					buf2[++opos]=buf[i];
					if(opos!=0){ spcCount++; }
				}
				else if(buf[i]=='('){
					buf2[opos]=',';
				}
				else if(buf[i]==')'){
					buf2[++opos]='\0';
				}
				else { buf2[++opos]=buf[i];}
			}
		}
	}
	else if(mode=='e'){
		fprintf(ofp,"pattern,countP,countN,size,pid\n");
		size_t recNo=0;
		//char buf[MAXC]; // 出力バッファ
		//char buf2[MAXC]; // 出力バッファ
		int spcCount=0; // スペースのカウント
		int comCount=0; // スペースのカウント
		int opos=-1;

		while(1){
			int rsize = fread(buf, sizeof(char), MAXC, fp);
			if( rsize < 0 ){ 
				fprintf(stderr,"file read error\n");
				if(buf) { delete [] buf ;}
				if(buf2) { delete [] buf2 ;}
				return EXIT_FAILURE;
			}
			if( rsize == 0 ) { break;}
			int i;
			for(i=0 ; i<rsize ;i++){
				if(buf[i]=='\n'){
					if(odd==1){
						fprintf(ofp,"%s,%d,%ld\n",buf2,spcCount,recNo);
						recNo++;
					}
					spcCount=0;
					comCount=0;
					opos=-1;
					odd*=(-1);
					continue;
				}
				if(odd!=1) { continue;}
				if(buf[i]==' '){
					buf2[++opos]=buf[i];
					if(opos!=0){ spcCount++; }
				}
				else if(buf[i]=='('){
					buf2[opos]=',';
				}
				else if(buf[i]==','){
					comCount++;
					if(comCount==2){ buf2[++opos]='\0';}
					else{ buf2[++opos]=buf[i];}
				}
				else { buf2[++opos]=buf[i];}
			}
		}
	}else if(mode=='t'){
		fprintf(ofp,"__tid,pid\n");
		size_t recNo=0;
		int opos=-1;
		//char buf[MAXC]; // 出力バッファ
		//char buf2[MAXC]; // 出力バッファ

		while(1){
			int rsize = fread(buf, sizeof(char), MAXC, fp);
			if( rsize < 0 ){ 
				fprintf(stderr,"file read error\n");
				if(buf) { delete [] buf ;}
				if(buf2) { delete [] buf2 ;}
				return EXIT_FAILURE;
			}
			if( rsize == 0 ) { break;}
			int i;
			for(i=0 ; i<rsize ;i++){
				if(odd==1){//奇数行は¥nまでなにもしない
					if(buf[i]=='\n'){
						opos=-1;
						odd*=(-1);
						buf2[0]='\0';
					}
				}
				else{
					if(buf[i]==' '||buf[i]=='\n'){
						buf2[++opos]='\0';
						if(buf2[0] != '\0'){
							fprintf(ofp,"%s,%ld\n",buf2,recNo);
						}
						opos=-1;
						if(buf[i]=='\n'){ 
							odd*=(-1);
							recNo++;
						}
					}
					else{ buf2[++opos]=buf[i]; }
				}
			}
		}
	}
	if(buf) { delete [] buf ;}
	if(buf2) { delete [] buf2 ;}
	if(0!=fclose(fp)){
		fprintf(stderr,"file close error\n");
		return EXIT_FAILURE;
	}
	if(0!=fclose(ofp)){
		fprintf(stderr,"file close error\n");
		return EXIT_FAILURE;
	}
	return 0;
}

PyObject* lcmtrans_run(PyObject* self, PyObject* args)
{
	
	char * inf =NULL;
	char * para=NULL; 
	char * outf=NULL;

	if (!PyArg_ParseTuple(args, "ss|s", &inf , &para  ,&outf)){
    return NULL;
  }
	int rtn = lcmtrans(inf,para,outf);
	return PyLong_FromLong(rtn);
}

static PyMethodDef takemethods[] = {
	{"lcmtrans_run", reinterpret_cast<PyCFunction>(lcmtrans_run), METH_VARARGS},
	{NULL}
};

#if PY_MAJOR_VERSION >= 3
static struct PyModuleDef moduledef = {
    PyModuleDef_HEAD_INIT,
    "_lcmtranslib",      /* m_name */
    NULL,							     /* m_doc */
    -1,                  /* m_size */
    takemethods,      /* m_methods */
    NULL,                /* m_reload */
    NULL,                /* m_traverse */
    NULL,                /* m_clear */
    NULL,                /* m_free */
};

PyMODINIT_FUNC
PyInit__lcmtranslib(void){
	PyObject* m;
	m = PyModule_Create(&moduledef);
	return m;
}

#else

void init_lcmtranslib(void){
	Py_InitModule("_lcmtranslib", takemethods);
}

#endif

