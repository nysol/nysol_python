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

void runCore(PyObject* mlist,PyObject* linklist ,vector< cmdCapselST > & cmdCapsel, vector< linkST > & p_list){

	Py_ssize_t msize = PyList_Size(mlist);
	
	for(Py_ssize_t i=0 ; i<msize;i++){
		PyObject *modinfo = PyList_GetItem(mlist ,i);
		cmdCapselST cmpcaplocal;
		cmpcaplocal.cmdname  = strGET(PyList_GetItem(modinfo ,0));
		//kgstr_t para_part    = strGET(PyList_GetItem(modinfo ,1));

		PyObject * para_LIST    = PyList_GetItem(modinfo ,1);

		PyObject *addinfo    = PyList_GetItem(modinfo ,2);
		cmpcaplocal.tag      = strGET(PyList_GetItem(modinfo ,3));

		Py_ssize_t psize = PyList_Size(para_LIST);//LISTチェック入れる？
		for(Py_ssize_t j=0 ; j<psize;j++){
			PyObject *parainfo = PyList_GetItem(para_LIST ,j);

			if( strCHECK(parainfo) ){
				cmpcaplocal.paralist.push_back(strGET(parainfo));
			}
			else if( PyList_Check( parainfo ) ){
				char * kwd = strGET(PyList_GetItem(parainfo ,0));
				if( !strcmp(kwd,"runfunc_func") ){
					cmpcaplocal.fobj=PyList_GetItem(parainfo ,1);
				}
				else if( !strcmp(kwd,"runfunc_args") ){
					cmpcaplocal.aobj=PyList_GetItem(parainfo ,1);
				}
				else if( !strcmp(kwd,"runfunc_kwd") ){
					cmpcaplocal.kobj=PyList_GetItem(parainfo ,1);
				}
			}
		}

		//cmpcaplocal.paralist = kglib::splitTokenQ(para_part, ' ',true);

		PyObject *key, *value;
		Py_ssize_t pos = 0;
		while (PyDict_Next(addinfo, &pos, &key, &value)) {
			if(strCHECK(value)){
				cmpcaplocal.paralist.push_back( kgstr_t(strGET(key)) + "="+ strGET(value) );
			}
			else if(PyList_Check(value)){
				// ioの種類によって in or out 
				if( kgstr_t(strGET(key)) == "i" ||kgstr_t(strGET(key)) == "m" ){
					cmpcaplocal.iobj=value;
				}
				else if( kgstr_t(strGET(key)) == "o" ||kgstr_t(strGET(key)) == "u" ){
					cmpcaplocal.oobj=value;
				}
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
	Py_ssize_t lsize = PyList_Size(linklist);
	for(Py_ssize_t i=0 ; i<lsize;i++){
		linkST linklocal;
		PyObject *linkinfo   = PyList_GetItem(linklist ,i);
		PyObject *linkinfoFR = PyList_GetItem(linkinfo ,0);
		PyObject *linkinfoTO = PyList_GetItem(linkinfo ,1);
		linklocal.frTP = strGET(PyList_GetItem(linkinfoFR ,0));
		linklocal.frID = PyLong_AsLong(PyList_GetItem(linkinfoFR ,1));
		linklocal.toTP = strGET(PyList_GetItem(linkinfoTO ,0));
		linklocal.toID = PyLong_AsLong(PyList_GetItem(linkinfoTO ,1));
		p_list.push_back(linklocal);
	}

	// debug
	//cerr <<  "------" << endl;
	//for(int i=0;i<cmdCapsel.size();i++){
	//	cerr << i << " " << cmdCapsel[i].cmdname << endl;
	//}
	//cerr <<  "------" << endl;
	//for(int i=0;i<p_list.size();i++){
	//	cerr << i << " " <<  p_list[i].frTP <<":" << p_list[i].frID ;
	//	cerr << " >> " <<  p_list[i].toTP << ":" << p_list[i].toID << endl; 
	//}
	//kgshell kgshell;
	// args : cmdList ,pipe_conect_List , runTYPE, return_LIST
}


PyObject* runL(PyObject* self, PyObject* args)
{
	try {

	PyObject *sh;
	PyObject *mlist;
	PyObject *linklist;
	if (!PyArg_ParseTuple(args, "OOO", &sh , &mlist  ,&linklist)){
    return NULL;
  }

	kgshell *ksh	= (kgshell *)PyCapsule_GetPointer(sh,"kgshellP");

	if(!PyList_Check(mlist)){
		cerr << "cannot run " << PyList_Check(mlist) << " "<<PyList_Size(mlist)<< endl;
		return Py_BuildValue("");
	}
	vector< cmdCapselST > cmdCapsel;
	vector< linkST > p_list;
	runCore(mlist,linklist,cmdCapsel,p_list);


	ksh->run(cmdCapsel,p_list);
	return PyLong_FromLong(0);


	}
	catch(kgError& err){
		cerr << "run Error [ " << err.message(0) << " ]" << endl;

	}catch (const exception& e) {
		cerr << "run Error [ " << e.what() << " ]" << endl;

	}catch(char * er){
		cerr << "run Error [ " << er << " ]" << endl;

	}catch(...){
		cerr << "run Error [ unKnown ERROR ]" << endl;
	}
	return PyLong_FromLong(1);
}


PyObject* runLx(PyObject* self, PyObject* args)
{
	try {

		if(!Py_IsInitialized()){ 
			Py_Initialize();
		}

		if (!PyEval_ThreadsInitialized())	{ 
			PyEval_InitThreads();
		}


		PyObject *sh;
		PyObject *mlist;
		PyObject *linklist;
		if (!PyArg_ParseTuple(args, "OOO", &sh , &mlist  ,&linklist)){
 	   return NULL;
 	 }

		kgshell *ksh	= (kgshell *)PyCapsule_GetPointer(sh,"kgshellP");

		if(!PyList_Check(mlist)){
			cerr << "cannot run " << PyList_Check(mlist) << " "<<PyList_Size(mlist)<< endl;
			return Py_BuildValue("");
		}
		vector< cmdCapselST > cmdCapsel;
		vector< linkST > p_list;
		runCore(mlist,linklist,cmdCapsel,p_list);


		ksh->runx(cmdCapsel,p_list);
		return PyLong_FromLong(0);


	}
	catch(kgError& err){
		cerr << "run Error [ " << err.message(0) << " ]" << endl;

	}catch (const exception& e) {
		cerr << "run Error [ " << e.what() << " ]" << endl;

	}catch(char * er){
		cerr << "run Error [ " << er << " ]" << endl;

	}catch(...){
		cerr << "run Error [ unKnown ERROR ]" << endl;
	}
	return PyLong_FromLong(1);
}


PyObject* cancel(PyObject* self, PyObject* args)
{
	try {

	PyObject *sh;
	PyObject *mlist;
	PyObject *linklist;
	if (!PyArg_ParseTuple(args, "O", &sh )){
    return NULL;
  }
	kgshell *ksh	= (kgshell *)PyCapsule_GetPointer(sh,"kgshellP");
	ksh->cancel();
	return PyLong_FromLong(0);


	}
	catch(kgError& err){
		cerr << "cancel Error [ " << err.message(0) << " ]" << endl;

	}catch (const exception& e) {
		cerr << "cancel Error [ " << e.what() << " ]" << endl;

	}catch(char * er){
		cerr << "cancel Error [ " << er << " ]" << endl;

	}catch(...){
		cerr << "cancel Error [ unKnown ERROR ]" << endl;
	}
	return PyLong_FromLong(1);
}


PyObject* csvclose(PyObject* self, PyObject* args)
{
	try {


	PyObject *csvin;
	//PyObject *list;
	//int tp;
	if (!PyArg_ParseTuple(args, "O", &csvin)){
    return Py_BuildValue("");
  }
	kgCSVfld *kcfld	= (kgCSVfld *)PyCapsule_GetPointer(csvin,"kgCSVfldP");
	kcfld->close();

	return PyLong_FromLong(0);


	}
	catch(kgError& err){
		cerr << "cancel Error [ " << err.message(0) << " ]" << endl;

	}catch (const exception& e) {
		cerr << "cancel Error [ " << e.what() << " ]" << endl;

	}catch(char * er){
		cerr << "cancel Error [ " << er << " ]" << endl;

	}catch(...){
		cerr << "cancel Error [ unKnown ERROR ]" << endl;
	}
	return PyLong_FromLong(1);
}


PyObject* runLs(PyObject* self, PyObject* args)
{
	try {

	PyObject *sh;
	PyObject *mlists;
	PyObject *linklists;
	if (!PyArg_ParseTuple(args, "OO",  &mlists  ,&linklists)){
    return NULL;
  }

//	kgshell *ksh	= (kgshell *)PyCapsule_GetPointer(sh,"kgshellP");

	if(!PyList_Check(mlists)){
		cerr << "cannot run " << PyList_Check(mlists) << " "<<PyList_Size(mlists)<< endl;
		return Py_BuildValue("");
	}


	Py_ssize_t lsize = PyList_Size(mlists);
	Py_ssize_t limit = 1;
	Py_ssize_t nowcount = 0;

	PyObject *runmod =PyList_New(0);
	PyObject *runlink =PyList_New(0);

	for(Py_ssize_t i=0;i<lsize;i++){

		PyObject *mlist = PyList_GetItem(mlists ,i);
		PyObject *llist = PyList_GetItem(linklists ,i);
		Py_ssize_t msize = PyList_Size(mlist);
		Py_ssize_t llsize = PyList_Size(llist);


		if(nowcount + msize >limit ){
			vector< cmdCapselST > cmdCapsel;
			vector< linkST > p_list;
			runCore(runmod,runlink ,cmdCapsel,p_list);
			kgshell *ksht =  new kgshell;
			ksht->run(cmdCapsel,p_list);
			delete ksht;
			//ksh->run(cmdCapsel,p_list);
			runmod =PyList_New(0);
			runlink =PyList_New(0);
			nowcount = 0;
		}

		nowcount += msize;
		
		for(Py_ssize_t j=0;j<msize;j++){
			PyList_Append(runmod,PyList_GetItem(mlist,j));
		}
		for(Py_ssize_t j=0;j<llsize;j++){
			PyList_Append(runlink,PyList_GetItem(llist,j));
		}
	}
	if(PyList_Size(runmod)>0){
		vector< cmdCapselST > cmdCapsel;
		vector< linkST > p_list;
		runCore(runmod,runlink,cmdCapsel,p_list);
		kgshell *ksht =  new kgshell;
		ksht->run(cmdCapsel,p_list);
		delete ksht;
	}


	return PyLong_FromLong(0);


	}
	catch(kgError& err){
		cerr << "run Error [ " << err.message(0) << " ]" << endl;

	}catch (const exception& e) {
		cerr << "run Error [ " << e.what() << " ]" << endl;

	}catch(char * er){
		cerr << "run Error [ " << er << " ]" << endl;

	}catch(...){
		cerr << "run Error [ unKnown ERROR ]" << endl;
	}
	return PyLong_FromLong(1);
}




PyObject* runP(PyObject* self, PyObject* args)
{
	PyObject *sh;
	PyObject *mlist;
	PyObject *linklist;
	if (!PyArg_ParseTuple(args, "OOO", &sh , &mlist  ,&linklist)){
    return NULL;
  }

	kgshell *ksh	= (kgshell *)PyCapsule_GetPointer(sh,"kgshellP");

	if(!PyList_Check(mlist)){
		cerr << "cannot run " << PyList_Check(mlist) << " " << PyList_Size(mlist)<< endl;
		return Py_BuildValue("");
	}
	vector< cmdCapselST > cmdCapsel;
	vector< linkST > p_list;
	runCore(mlist,linklist,cmdCapsel,p_list);

	kgCSVfld* rtn = ksh->runiter(cmdCapsel,p_list);

	return PyCapsule_New(rtn,"kgCSVfldP",NULL);

}

PyObject* runPK(PyObject* self, PyObject* args)
{
	PyObject *sh;
	PyObject *mlist;
	PyObject *linklist;
	PyObject *keys;
	if (!PyArg_ParseTuple(args, "OOOO", &sh , &mlist  ,&linklist,&keys)){
    return NULL;
  }

	kgshell *ksh	= (kgshell *)PyCapsule_GetPointer(sh,"kgshellP");

	if(!PyList_Check(mlist)){
		cerr << "cannot run " << PyList_Check(mlist) << " " << PyList_Size(mlist)<< endl;
		return Py_BuildValue("");
	}
	vector< cmdCapselST > cmdCapsel;
	vector< linkST > p_list;
	runCore(mlist,linklist,cmdCapsel,p_list);

	vector< string > k_list;

	//key set
	Py_ssize_t ksize = PyList_Size(keys);
	for(Py_ssize_t i=0 ; i<ksize;i++){
		k_list.push_back(strGET(PyList_GetItem(keys ,i)));
	}

	kgCSVkey* rtn = ksh->runkeyiter(cmdCapsel,p_list,k_list);

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
	size_t fcnt = kcfld->fldSize();
	PyObject* rlist = PyList_New(fcnt);
	for(size_t j=0 ;j<fcnt;j++){
		PyList_SetItem(rlist,j,Py_BuildValue("s", kcfld->getVal(j)));
	}
	return rlist;
}

PyObject* readkeyline(PyObject* self, PyObject* args)
{

	PyObject *csvin;
	//PyObject *list;
	//int tp;
	if (!PyArg_ParseTuple(args, "O", &csvin)){
    return Py_BuildValue("");
  }
	kgCSVkey *kcfld	= (kgCSVkey *)PyCapsule_GetPointer(csvin,"kgCSVfldP");

	PyObject* rlist = PyList_New(0);
	size_t fcnt = kcfld->fldSize();

	if((kcfld->status() & kgCSV::End )){ return Py_BuildValue("");}

	while(kcfld->read()!=EOF){

		//一行目読み込み時は何もしない
		if(( kcfld->status() & kgCSV::Begin )){continue;}

		PyObject* rllist = PyList_New(fcnt);
		for(size_t j=0 ;j<fcnt;j++){
			PyList_SetItem(rllist,j,Py_BuildValue("s", kcfld->getOldVal(j)));
		}
		PyList_Append(rlist,rllist);
		Py_DECREF(rllist);

		if( kcfld->keybreak() ){
			break;
		}
	}
	return rlist;
}

PyObject* readkeylineDict(PyObject* self, PyObject* args)
{

	PyObject *csvin;
	//PyObject *list;
	//int tp;
	if (!PyArg_ParseTuple(args, "O", &csvin)){
    return Py_BuildValue("");
  }
	kgCSVkey *kcfld	= (kgCSVkey *)PyCapsule_GetPointer(csvin,"kgCSVfldP");

	size_t fcnt = kcfld->fldSize();


	if((kcfld->status() & kgCSV::End )){ return Py_BuildValue("");}

	vector< PyObject* > rlists(fcnt); 

	for(size_t j=0 ;j<fcnt;j++){
		rlists[j] = PyList_New(0);
	}

	while(kcfld->read()!=EOF){

		//一行目読み込み時は何もしない
		if(( kcfld->status() & kgCSV::Begin )){continue;}

		for(size_t j=0 ;j<fcnt;j++){
			PyObject* setobj = Py_BuildValue("s", kcfld->getOldVal(j));
			PyList_Append(rlists[j],setobj);
			Py_DECREF(setobj);
		}
		if( kcfld->keybreak() ){
			break;
		}
	}

  PyObject* rlist = PyDict_New();
	for(size_t j=0 ;j<fcnt;j++){
		PyDict_SetItemString(rlist,kcfld->fldName(j).c_str(),rlists[j]);
		Py_DECREF(rlists[j]);
	}
	
	return rlist;
}


PyObject* readkeyline_with_flag(PyObject* self, PyObject* args)
{

	PyObject *csvin;
	//PyObject *list;
	//int tp;
	if (!PyArg_ParseTuple(args, "O", &csvin)){
    return Py_BuildValue("");
  }
	kgCSVkey *kcfld	= (kgCSVkey *)PyCapsule_GetPointer(csvin,"kgCSVfldP");

	if((kcfld->status() & kgCSV::End )){ 
		return Py_BuildValue("");
	}

	if( kcfld->read() == EOF){
		return Py_BuildValue("");
	}


	if( kcfld->begin() ){ 
		if( kcfld->read() == EOF){
			return Py_BuildValue("");
		}
	}

	PyObject* finlist = PyList_New(2);

	size_t fcnt = kcfld->fldSize();
	PyObject* rllist = PyList_New(fcnt);

	for(size_t j=0 ;j<fcnt;j++){
		PyList_SetItem(rllist,j,Py_BuildValue("s", kcfld->getOldVal(j)));
	}

	PyList_SetItem(finlist,0,rllist);

	if( kcfld->keybreak() ){
		PyList_SetItem(finlist,1,Py_True);
		Py_INCREF(Py_True);

		//PyList_Append(finlist,Py_True);
	}
	else{
		PyList_SetItem(finlist,1,Py_False);
		Py_INCREF(Py_False);
		//PyList_Append(finlist,Py_False);
	}
	return finlist;
}



void py_kgshell_free(PyObject *obj){
	kgshell *ksh	= (kgshell *)PyCapsule_GetPointer(obj,"kgshellP");
	if(ksh){
		delete ksh;
	}
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
	{"runL", reinterpret_cast<PyCFunction>(runL), METH_VARARGS },
	{"runLs", reinterpret_cast<PyCFunction>(runLs), METH_VARARGS },
	{"runLx", reinterpret_cast<PyCFunction>(runLx), METH_VARARGS },
	{"runiter", reinterpret_cast<PyCFunction>(runP), METH_VARARGS },
	{"runkeyiter", reinterpret_cast<PyCFunction>(runPK), METH_VARARGS },
	{"readline", reinterpret_cast<PyCFunction>(readline), METH_VARARGS },
	{"readkeyline", reinterpret_cast<PyCFunction>(readkeyline), METH_VARARGS },
	{"readkeylineDict", reinterpret_cast<PyCFunction>(readkeylineDict), METH_VARARGS },
	{"readkeylineWithFlag", reinterpret_cast<PyCFunction>(readkeyline_with_flag), METH_VARARGS },
	{"getparalist", reinterpret_cast<PyCFunction>(getparams), METH_VARARGS },
	{"cancel", reinterpret_cast<PyCFunction>(cancel), METH_VARARGS },
	{"close", reinterpret_cast<PyCFunction>(csvclose), METH_VARARGS },
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
