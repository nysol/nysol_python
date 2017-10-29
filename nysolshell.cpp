// helloWrapper.c
#include "Python.h"
//#undef tolower
//#undef toupper
//#undef isalpha
//#undef islower
//#undef isupper
//#undef isspace
//#undef isalnum
#include <kgEnv.h>
#include <kgMethod.h>
#include <kgCSV.h>
#include <kgshell.h>

using namespace kgmod;
using namespace kglib;

#if PY_MAJOR_VERSION >= 3
extern "C" {
	PyMODINIT_FUNC PyInit__nysolshell_core(void);
}
#else
extern "C" {
	void init_nysolshell_core(void);
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

int run_sub(PyObject* tlist,
	vector< cmdCapselST >& cmdcap,
	vector< vector<int> > & p_list, // i,o,tp(0:i=,1:m=)
	int lcnt
){
	int pos = lcnt;

	int ipos = -1;
	int mpos = -1;

	cmdCapselST cmpcaplocal;

	cmpcaplocal.cmdname = strGET(PyList_GetItem(tlist, 0));

	kgstr_t para_part = "";
	if(strCHECK(PyList_GetItem(tlist, 1))){
		para_part = strGET(PyList_GetItem(tlist, 1));
	}
	cmpcaplocal.paralist=kglib::splitTokenQ(para_part, ' ',true);


	PyObject *ilink = PyList_GetItem(tlist, 2);
	PyObject *mlink = PyList_GetItem(tlist, 3);

	if(strCHECK(ilink)){
		cmpcaplocal.paralist.push_back( kgstr_t("i=")+ strGET(ilink) );
		cmdcap.push_back(cmpcaplocal);
	}
	else if(PyList_Check(ilink)){
		if(PyList_Check(PyList_GetItem(ilink, 0))){
			cmdcap.push_back(cmpcaplocal);
			cmdCapselST cmpcapmload;
			//list data mload追加
			ipos = lcnt+1;
			cmpcapmload.cmdname="mload";
			cmpcapmload.iobj=ilink;
			cmdcap.push_back(cmpcapmload);

			lcnt++;
			vector<int> pno_i;
			pno_i.push_back(pos-1);
			pno_i.push_back(ipos-1);
			pno_i.push_back(0);
			p_list.push_back(pno_i);

		}
		else{
			cmdcap.push_back(cmpcaplocal);
			ipos = lcnt+1;
			lcnt = run_sub(ilink,cmdcap,p_list,lcnt+1);

			vector<int> pno_i;
			pno_i.push_back(pos-1);
			pno_i.push_back(ipos-1);
			pno_i.push_back(0);
			p_list.push_back(pno_i);
		}
	}

	if(strCHECK(mlink)){
		cmdcap[pos-1].paralist.push_back( kgstr_t("m=")+ strGET(mlink) );
	}
	else if(PyList_Check(mlink)){
		if(PyList_Check(PyList_GetItem(mlink, 0))){
			cmdCapselST cmpcapmload;
			//list data mload追加
			mpos = lcnt+1;
			cmpcapmload.cmdname="mload";
			cmpcapmload.iobj=mlink;
			cmdcap.push_back(cmpcapmload);

			lcnt++;
			vector<int> pno_o;
			pno_o.push_back(pos-1);
			pno_o.push_back(mpos-1);
			pno_o.push_back(1);
			p_list.push_back(pno_o);
		}	
		else{
			mpos = lcnt+1;	
			lcnt = run_sub(mlink,cmdcap,p_list,lcnt+1);
			vector<int> pno_o;
			pno_o.push_back(pos-1);
			pno_o.push_back(mpos-1);
			pno_o.push_back(1);
			p_list.push_back(pno_o);
		}
	}

	if(ilink==Py_None &&mlink==Py_None ){
		cmdcap.push_back(cmpcaplocal);
	}
	
	return lcnt;
}

PyObject* run(PyObject* self, PyObject* args)try
{
	PyObject *sh;
	PyObject *list;
	PyObject *rlist;
	int tp;
	if (!PyArg_ParseTuple(args, "OOiO", &sh , &list ,&tp ,&rlist)){
    return NULL;
  }
/*
	kgshell *ksh	= (kgshell *)PyCapsule_GetPointer(sh,"kgshellP");

	if(!PyList_Check(list)){
		cerr << "cannot run " << PyList_Check(list) << " "<<PyList_Size(list)<< endl;
		return Py_BuildValue("");
	}

	// list [ cmd para ilink mlink]
	vector< vector<int> > p_list;
	vector< cmdCapselST > cmdCapsel;

	int  lsize = run_sub(list,cmdCapsel,p_list,1);

	// debug
	cerr <<  "------" << endl;
	cerr <<  "lsize " << lsize << endl;
	for(int i=0;i<cmdCapsel.size();i++){
		cerr << i << " " << cmdCapsel[i].cmdname << endl;
	}
	cerr <<  "------" << endl;
	for(int i=0;i<p_list.size();i++){
		cerr << i << " i:" << p_list[i][0] << "<<o:" << p_list[i][1]<< endl;
	}
	//kgshell kgshell;
	// args : cmdList ,pipe_conect_List , runTYPE, return_LIST

	//old
	// ksh->run(cmdCapsel,p_list,tp,rlist);

	if(tp){
		return PyLong_FromLong(0);
	}else{
		return PyLong_FromLong(0);
	}	
	*/
}catch(...){
	cerr << "exceptipn" << endl;
	return PyLong_FromLong(1);
}


int run_subL(PyObject* tlist,
	vector< cmdCapselST >& cmdcap,
	vector< vector<int> > & p_list, // i,o,tp(0:i=,1:m=)
	int lcnt
){
	int pos = lcnt;

	int ipos = -1;
	int mpos = -1;

	cmdCapselST cmpcaplocal;

	cmpcaplocal.cmdname = strGET(PyList_GetItem(tlist, 0));

	kgstr_t para_part = "";
	if(strCHECK(PyList_GetItem(tlist, 1))){
		para_part = strGET(PyList_GetItem(tlist, 1));
	}
	cmpcaplocal.paralist=kglib::splitTokenQ(para_part, ' ',true);


	PyObject *ilink = PyList_GetItem(tlist, 2);
	PyObject *mlink = PyList_GetItem(tlist, 3);

	if(strCHECK(ilink)){
		cmpcaplocal.paralist.push_back( kgstr_t("i=")+ strGET(ilink) );
		cmdcap.push_back(cmpcaplocal);
	}
	else if(PyList_Check(ilink)){
		if(PyList_Check(PyList_GetItem(ilink, 0))){
			cmdcap.push_back(cmpcaplocal);
			cmdCapselST cmpcapmload;
			//list data mload追加
			ipos = lcnt+1;
			cmpcapmload.cmdname="mload";
			cmpcapmload.iobj=ilink;
			cmdcap.push_back(cmpcapmload);

			lcnt++;
			vector<int> pno_i;
			pno_i.push_back(pos-1);
			pno_i.push_back(ipos-1);
			pno_i.push_back(0);
			p_list.push_back(pno_i);

		}
		else{
			cmdcap.push_back(cmpcaplocal);
			ipos = lcnt+1;
			lcnt = run_sub(ilink,cmdcap,p_list,lcnt+1);

			vector<int> pno_i;
			pno_i.push_back(pos-1);
			pno_i.push_back(ipos-1);
			pno_i.push_back(0);
			p_list.push_back(pno_i);
		}
	}

	if(strCHECK(mlink)){
		cmdcap[pos-1].paralist.push_back( kgstr_t("m=")+ strGET(mlink) );
	}
	else if(PyList_Check(mlink)){
		if(PyList_Check(PyList_GetItem(mlink, 0))){
			cmdCapselST cmpcapmload;
			//list data mload追加
			mpos = lcnt+1;
			cmpcapmload.cmdname="mload";
			cmpcapmload.iobj=mlink;
			cmdcap.push_back(cmpcapmload);

			lcnt++;
			vector<int> pno_o;
			pno_o.push_back(pos-1);
			pno_o.push_back(mpos-1);
			pno_o.push_back(1);
			p_list.push_back(pno_o);
		}	
		else{
			mpos = lcnt+1;	
			lcnt = run_sub(mlink,cmdcap,p_list,lcnt+1);
			vector<int> pno_o;
			pno_o.push_back(pos-1);
			pno_o.push_back(mpos-1);
			pno_o.push_back(1);
			p_list.push_back(pno_o);
		}
	}

	if(ilink==Py_None &&mlink==Py_None ){
		cmdcap.push_back(cmpcaplocal);
	}
	
	return lcnt;
}


PyObject* runL(PyObject* self, PyObject* args)try
{
	PyObject *sh;
	PyObject *mlist;
	PyObject *iolist;
	PyObject *linklist;
	PyObject *rlist;
	bool tp = false;
	if (!PyArg_ParseTuple(args, "OOOOO", &sh , &mlist ,&iolist ,&linklist,&rlist)){
    return NULL;
  }

	kgshell *ksh	= (kgshell *)PyCapsule_GetPointer(sh,"kgshellP");

	if(!PyList_Check(mlist)){
		cerr << "cannot run " << PyList_Check(mlist) << " "<<PyList_Size(mlist)<< endl;
		return Py_BuildValue("");
	}
	cerr << "st3" << endl;
	vector< cmdCapselST > cmdCapsel;

/*
	// list [ cmd para ilink mlink]
	vector< vector<int> > p_list;
	vector< cmdCapselST > cmdCapsel;
struct cmdCapselST{
	kgstr_t cmdname;
	vector<kgstr_t> paralist;
	kgstr_t istr;
	PyObject* iobj;
	kgstr_t mstr;
	PyObject* mobj;

};

	int  lsize = run_sub(mlist,cmdCapsel,p_list,1);
*/

	Py_ssize_t msize = PyList_Size(mlist);
	for(Py_ssize_t i=0 ; i<msize;i++){
		PyObject *modinfo = PyList_GetItem(mlist ,i);
		cmdCapselST cmpcaplocal;
		cmpcaplocal.cmdname  = strGET(PyList_GetItem(modinfo ,0));
		kgstr_t para_part    = strGET(PyList_GetItem(modinfo ,1));
		PyObject *addinfo    = PyList_GetItem(modinfo ,2);

		cmpcaplocal.paralist = kglib::splitTokenQ(para_part, ' ',true);

		PyObject *key, *value;
		Py_ssize_t pos = 0;
		while (PyDict_Next(addinfo, &pos, &key, &value)) {
			if(strCHECK(value)){
				cmpcaplocal.paralist.push_back( kgstr_t(strGET(key)) + "="+ strGET(value) );
			}
		}
		cmdCapsel.push_back(cmpcaplocal);
	}		
	/*
	struct linkST{
	kgstr_t frTP;
	int frID;
	kgstr_t toTP;
	int toID;
	};*/
	vector< linkST > p_list;
	Py_ssize_t lsize = PyList_Size(linklist);
	for(Py_ssize_t i=0 ; i<lsize;i++){
		linkST linklocal;
		PyObject *linkinfo   = PyList_GetItem(linklist ,i);
		PyObject *linkinfoFR = PyList_GetItem(linkinfo ,0);
		PyObject *linkinfoTO = PyList_GetItem(linkinfo ,1);
		linklocal.frTP = strGET(PyList_GetItem(linkinfoFR ,0));
		linklocal.frID = PyInt_AsLong(PyList_GetItem(linkinfoFR ,1));
		linklocal.toTP = strGET(PyList_GetItem(linkinfoTO ,0));
		linklocal.toID = PyInt_AsLong(PyList_GetItem(linkinfoTO ,1));
		p_list.push_back(linklocal);
	}

	// debug
/*	cerr <<  "------" << endl;
	for(int i=0;i<cmdCapsel.size();i++){
		cerr << i << " " << cmdCapsel[i].cmdname << endl;
	}
	cerr <<  "------" << endl;
	for(int i=0;i<p_list.size();i++){
		cerr << i << " " <<  p_list[i].frTP <<":" << p_list[i].frID ;
		cerr << " >> " <<  p_list[i].toTP << ":" << p_list[i].toID << endl; 
	}
*/
	//kgshell kgshell;
	// args : cmdList ,pipe_conect_List , runTYPE, return_LIST

	ksh->run(cmdCapsel,p_list,tp,rlist);

/*

	if(tp){
		return PyLong_FromLong(0);
	}else{
		return PyLong_FromLong(0);
	}	
*/
	return PyLong_FromLong(0);

}catch(...){
	cerr << "exceptipn" << endl;
	return PyLong_FromLong(1);
}

PyObject* runP(PyObject* self, PyObject* args)
{

	PyObject *sh;
	PyObject *list;
	int tp;
	if (!PyArg_ParseTuple(args, "OOi", &sh , &list ,&tp)){
    return NULL;
  }

	kgshell *ksh	= (kgshell *)PyCapsule_GetPointer(sh,"kgshellP");

	if(!PyList_Check(list)){
		cerr << "cannot run " << PyList_Check(list) << " "<<PyList_Size(list)<< endl;
		return Py_BuildValue("");
	}

	// list [ cmd para ilink mlink]
	vector< vector<int> > p_list;
	vector< cmdCapselST > cmdCapsel;

	run_sub(list,cmdCapsel,p_list,1);


	// debug
	//	cerr <<  "------" << endl;
	//	cerr <<  "lsize " << lsize << endl;
	//for(int i=0;i<cmd_vv.size();i++){
	//	cerr << i << " " << cmd_vv[i] << endl;
	//}
	//	cerr <<  "------" << endl;
	//for(int i=0;i<p_list.size();i++){
	//	cerr << i << " i:" << p_list[i][0] << "<<o:" << p_list[i][1]<< endl;
	//}
	PyObject* rlist = PyList_New(0);
	//kgshell kgshell;
	kgCSVfld* rtn = ksh->runiter(cmdCapsel,p_list,tp,rlist);
	return PyCapsule_New(rtn,"kgCSVfldP",NULL);
}

PyObject* readline(PyObject* self, PyObject* args)
{

	PyObject *csvin;
	//PyObject *list;
	//int tp;
	if (!PyArg_ParseTuple(args, "O", &csvin)){
    return Py_BuildValue("");
  }

	kgCSVfld *kcfld	= (kgCSVfld *)PyCapsule_GetPointer(csvin,"kgCSVfldP");

	if( kcfld->read() == EOF){
		return Py_BuildValue("");
	}
	int fcnt = kcfld->fldSize();
	PyObject* rlist = PyList_New(0);
	for(size_t j=0 ;j<fcnt;j++){
		PyList_Append(rlist,Py_BuildValue("s", kcfld->getVal(j)));
	}
	return rlist;
}

void py_kgshell_free(PyObject *obj){
	kgshell *ksh	= (kgshell *)PyCapsule_GetPointer(obj,"kgshellP");
	delete ksh;
}

PyObject* getparams(PyObject* self, PyObject* args){

	char *cmdname;
	if (!PyArg_ParseTuple(args, "s", &cmdname)){
    return Py_BuildValue("");
  }
	PyObject* rlist = PyList_New(0);	
	kgshell ksh;
	ksh.getparams(cmdname,rlist);
	return rlist;
	
}


PyObject* start(PyObject* self, PyObject* args){
	int mflg;
	if (!PyArg_ParseTuple(args, "i", &mflg)){
		return PyCapsule_New(new kgshell(false),"kgshellP",py_kgshell_free);
  }else{
		return PyCapsule_New(new kgshell(mflg),"kgshellP",py_kgshell_free);
  }
}

static PyMethodDef hellomethods[] = {
	{"init", reinterpret_cast<PyCFunction>(start), METH_VARARGS },
	{"run", reinterpret_cast<PyCFunction>(run), METH_VARARGS },
	{"runL", reinterpret_cast<PyCFunction>(runL), METH_VARARGS },
	{"runiter", reinterpret_cast<PyCFunction>(runP), METH_VARARGS },
	{"readline", reinterpret_cast<PyCFunction>(readline), METH_VARARGS },
	{"getparalist", reinterpret_cast<PyCFunction>(getparams), METH_VARARGS },
	{NULL}
};

#if PY_MAJOR_VERSION >= 3
static struct PyModuleDef moduledef = {
    PyModuleDef_HEAD_INIT,
    "_nysolshell_core",      /* m_name */
    NULL,							     /* m_doc */
    -1,                  /* m_size */
    hellomethods,      /* m_methods */
    NULL,                /* m_reload */
    NULL,                /* m_traverse */
    NULL,                /* m_clear */
    NULL,                /* m_free */
};

PyMODINIT_FUNC
PyInit__nysolshell_core(void){
	PyObject* m;
	m = PyModule_Create(&moduledef);
	return m;
}

#else

void init_nysolshell_core(void){
	Py_InitModule("_nysolshell_core", hellomethods);
}

#endif
