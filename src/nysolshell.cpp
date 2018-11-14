// helloWrapper.c
#include "Python.h"
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

//static char* strGET(PyObject* data){
//static bool strCHECK(PyObject* data){
#if PY_MAJOR_VERSION >= 3
 #define strGET   PyUnicode_AsUTF8
 #define strCHECK PyUnicode_Check
#else		
 #define strGET   PyString_AsString
 #define strCHECK PyString_Check
#endif


static PyObject* setRtnData(long k,char * v){
	PyObject* rtn = NULL;

	if(*v=='\0'){
		if(k==0){
			rtn = PyUnicode_FromStringAndSize(v, strlen(v));
		}
		else{
			Py_INCREF(Py_None);
			rtn = Py_None;
		}
	}
	else if(k==0){ //str

		rtn = PyUnicode_FromStringAndSize(v, strlen(v));

	}else if(k==1){ // int

		rtn = PyLong_FromLong(atol(v)); 

	}else if(k==2){ // double

		rtn = PyFloat_FromDouble(atof(v));

	}else if(k==3){ // bool

		if(strlen(v)==1 && *v=='0'){

			Py_INCREF(Py_False);
			rtn = Py_False;

		}	
		else{

			Py_INCREF(Py_True);
			rtn = Py_True;
		
		}
	}
	else{
		rtn = Py_None;
		Py_INCREF(Py_None);
	}
	return rtn;

}

PyObject* cancel(PyObject* self, PyObject* args)
{
	try {

		PyObject *sh;

		if (!PyArg_ParseTuple(args, "O", &sh )){ return NULL; }

		kgshell *ksh	= (kgshell *)PyCapsule_GetPointer(sh,"kgshellP");

		if(ksh) ksh->cancel();

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

		if (!PyArg_ParseTuple(args, "O", &csvin)){
  	  return Py_BuildValue("");
 		}

		kgCSVfld *kcfld	= (kgCSVfld *)PyCapsule_GetPointer(csvin,"kgCSVfldP");

		if(kcfld){ kcfld->close(); }

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

void py_kgcsv_free(PyObject *obj){

	kgCSVfld *kcsv	= (kgCSVfld *)PyCapsule_GetPointer(obj,"kgCSVfldP");
	if(kcsv){
		delete kcsv;
	}
}


void runCore(PyObject* mlist,PyObject* linklist ,vector< cmdCapselST > & cmdCapsel, vector< linkST > & p_list){

	Py_ssize_t msize = PyList_Size(mlist);
	
	for(Py_ssize_t i=0 ; i<msize;i++){
		PyObject *modinfo = PyList_GetItem(mlist ,i);
		cmdCapselST cmpcaplocal;
		cmpcaplocal.cmdname  = strGET(PyList_GetItem(modinfo ,0));
		//kgstr_t para_part    = strGET(PyList_GetItem(modinfo ,1));

		PyObject * para_LIST    = PyList_GetItem(modinfo ,1);

		PyObject *addinfo_i    = PyList_GetItem(modinfo ,2);
		PyObject *addinfo_o    = PyList_GetItem(modinfo ,3);
		cmpcaplocal.tag      = strGET(PyList_GetItem(modinfo ,4));
		cmpcaplocal.weight   = PyLong_AsLong(PyList_GetItem(modinfo ,6));


		Py_ssize_t psize = PyList_Size(para_LIST);//LISTチェック入れる？
		for(Py_ssize_t j=0 ; j<psize;j++){
			PyObject *parainfo = PyList_GetItem(para_LIST ,j);

			if( strCHECK(parainfo) ){
				cmpcaplocal.paralist.push_back(strGET(parainfo));
			}
			else if( PyList_Check( parainfo ) ){
				const char * kwd = strGET(PyList_GetItem(parainfo ,0));
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

		PyObject *key_i, *value_i;
		Py_ssize_t pos_i = 0;
		while (PyDict_Next(addinfo_i, &pos_i, &key_i, &value_i)) {
			if(strCHECK(value_i)){
				cmpcaplocal.paralist.push_back( kgstr_t(strGET(key_i)) + "="+ strGET(value_i) );
			}
			else if(PyList_Check(value_i)){
				cmpcaplocal.iobj=value_i;
			}
		}
		PyObject *key_o, *value_o;
		Py_ssize_t pos_o = 0;
		while (PyDict_Next(addinfo_o, &pos_o, &key_o, &value_o)) {
			if(strCHECK(value_o)){
				cmpcaplocal.paralist.push_back( kgstr_t(strGET(key_o)) + "="+ strGET(value_o) );
			}
			else if(PyList_Check(value_o)){
				cmpcaplocal.oobj=value_o;
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
	/*
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
	*/
}


PyObject* run(PyObject* self, PyObject* args)
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
		if(ksh->runx(cmdCapsel,p_list)){
			return PyLong_FromLong(1); 
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

PyObject* runITER(PyObject* self, PyObject* args)
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
		PyObject *keys=NULL;

		if (!PyArg_ParseTuple(args, "OOO|O", &sh , &mlist  ,&linklist,&keys)){
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

		if(keys !=NULL){
			vector< string > k_list;
			//key set
			Py_ssize_t ksize = PyList_Size(keys);
			for(Py_ssize_t i=0 ; i<ksize;i++){
				k_list.push_back(strGET(PyList_GetItem(keys ,i)));
			}
			kgCSVkey* rtn = ksh->runkeyiter(cmdCapsel,p_list,k_list);
			
			if(rtn==NULL){ return Py_BuildValue("");}

			PyGILState_STATE gstate;
			gstate = PyGILState_Ensure();
			PyObject *pry = PyCapsule_New(rtn,"kgCSVfldP",py_kgcsv_free);
			PyGILState_Release(gstate);
			return pry;
		}
		else{

			//PyGILState_STATE _save = PyGILState_Ensure();
			kgCSVfld* rtn = ksh->runiter(cmdCapsel,p_list);	
			if(rtn==NULL){ return Py_BuildValue("");}
			PyGILState_STATE gstate;
			gstate = PyGILState_Ensure();
			PyObject *pry = PyCapsule_New(rtn,"kgCSVfldP",py_kgcsv_free);
			PyGILState_Release(gstate);

			return pry;
		}

		return Py_BuildValue("");
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
	return NULL;

}

PyObject* fldtp(PyObject* self, PyObject* args)
{

	PyObject *csvin;
	PyObject *tpmap;
	//int tp;
	if (!PyArg_ParseTuple(args, "OO", &csvin,&tpmap)){
    return Py_BuildValue("");
  }


	kgCSVfld *kcfld	= (kgCSVkey *)PyCapsule_GetPointer(csvin,"kgCSVfldP");
	
	size_t fcnt = kcfld->fldSize();
	PyObject* rlist = PyList_New(fcnt);

	for(size_t j=0 ;j<fcnt;j++){
		PyObject * key = Py_BuildValue("s",kcfld->fldName(j).c_str());
		PyObject * v = PyDict_GetItem(tpmap,key);
		Py_DECREF(key);
		if(v){
			const char *vv = strGET(v);
			if ( !strcmp(vv,"str")){
				PyList_SetItem(rlist,j,Py_BuildValue("i", 0));
						
			}
			else if ( !strcmp(vv,"int")){
				PyList_SetItem(rlist,j,Py_BuildValue("i", 1));
			}
			else if ( !strcmp(vv,"double")){
				PyList_SetItem(rlist,j,Py_BuildValue("i", 2));
			}
			else if ( !strcmp(vv,"float")){
				PyList_SetItem(rlist,j,Py_BuildValue("i", 2));
			}
			else if ( !strcmp(vv,"bool")){
				PyList_SetItem(rlist,j,Py_BuildValue("i", 3));
			}
			else{
				PyList_SetItem(rlist,j,Py_BuildValue("i", 0));
			}
		}
		else{
			PyList_SetItem(rlist,j,Py_BuildValue("i", 0));
		}
	}
	return rlist;

}


PyObject* getLineList(PyObject* self, PyObject* args)
{

	PyObject *csvin;
	PyObject *ptn = NULL;

	//PyObject *list;
	//int tp;
	if (!PyArg_ParseTuple(args, "O|O", &csvin,&ptn)){

		PyErr_SetString(PyExc_RuntimeError,"parameter ERROR");
		PyErr_Print();

    return Py_BuildValue("");
  }

	kgCSVfld *kcfld	= (kgCSVfld *)PyCapsule_GetPointer(csvin,"kgCSVfldP");

	if( kcfld->read() == EOF){
		return Py_BuildValue("");
	}

	size_t fcnt = kcfld->fldSize();

	PyObject* rlist = PyList_New(fcnt);

	for(size_t j=0 ;j<fcnt;j++){
		long k = 0;
		if(ptn!=NULL){ k = PyLong_AsLong ( PyList_GetItem(ptn,j) ); }
		PyList_SET_ITEM(rlist,j,setRtnData(k,kcfld->getVal(j)));
	}


	return rlist;
}



PyObject* getLineDict(PyObject* self, PyObject* args)
{

	PyObject *csvin;
	PyObject *ptn;
	//int tp;
	if (!PyArg_ParseTuple(args, "OO", &csvin, &ptn)){
    return Py_BuildValue("");
  }
	kgCSVfld *kcfld	= (kgCSVfld *)PyCapsule_GetPointer(csvin,"kgCSVfldP");

	if( kcfld->read() == EOF){
		return Py_BuildValue("");
	}
	size_t fcnt = kcfld->fldSize();


  PyObject* rlist = PyDict_New();
	for(size_t j=0 ;j<fcnt;j++){
		PyObject* vv = setRtnData(PyLong_AsLong(PyList_GetItem(ptn,j)) , kcfld->getVal(j));
		PyDict_SetItemString(rlist,kcfld->fldName(j).c_str(),vv);
		Py_DECREF(vv);
	}
	return rlist;
}



PyObject* getBlkList(PyObject* self, PyObject* args)
{

	PyObject *csvin;
	PyObject *ptn;
	//PyObject *list;
	//int tp;
	if (!PyArg_ParseTuple(args, "OO", &csvin, &ptn)){
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
			long k = PyLong_AsLong ( PyList_GetItem(ptn,j) );
			char* v = kcfld->getOldVal(j);
			PyList_SetItem(rllist,j,setRtnData(k,v));
		}
		PyList_Append(rlist,rllist);
		Py_DECREF(rllist);

		if( kcfld->keybreak() ){
			break;
		}
	}
	return rlist;
}

PyObject* getBlkDict(PyObject* self, PyObject* args)
{

	PyObject *csvin;
	PyObject *ptn;

	if (!PyArg_ParseTuple(args, "OO", &csvin,&ptn)){
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
		
			long k = PyLong_AsLong ( PyList_GetItem(ptn,j) );
			char* v = kcfld->getOldVal(j);
			PyObject* setobj = setRtnData(k,v);
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


PyObject* getLineDictWithInfo(PyObject* self, PyObject* args)
{

	PyObject *csvin;
	PyObject *ptn;
	//int tp;
	if (!PyArg_ParseTuple(args, "OO", &csvin, &ptn)){
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
	//PyObject* rllist = PyList_New(fcnt);

  PyObject* rllist = PyDict_New();

	for(size_t j=0 ;j<fcnt;j++){
		long k = PyLong_AsLong ( PyList_GetItem(ptn,j) );
		char* v = kcfld->getOldVal(j);
		PyObject* setobj = setRtnData(k,v);
		PyDict_SetItemString(rllist,kcfld->fldName(j).c_str(),setobj);
		Py_DECREF(setobj);
	}

	PyList_SetItem(finlist,0,rllist);

	if( kcfld->keybreak() ){
		PyList_SetItem(finlist,1,Py_True);
		Py_INCREF(Py_True);

	}
	else{
		PyList_SetItem(finlist,1,Py_False);
		Py_INCREF(Py_False);
	}
	return finlist;
}

PyObject* fldname(PyObject* self, PyObject* args)
{
	
	PyObject *csvin;
	//int tp;
	if (!PyArg_ParseTuple(args, "O", &csvin)){
    return Py_BuildValue("");
  }

	kgCSV *kcfld	= (kgCSV *)PyCapsule_GetPointer(csvin,"kgCSVfldP");

	vector<kgstr_t> flds = kcfld->fldName();
	PyObject* rlist = PyList_New(flds.size());
	for(size_t i=0; i < flds.size();i++){
		PyList_SetItem(rlist,i,Py_BuildValue("s", flds[i].c_str()));
	}
	return rlist;

}

PyObject* getLineListWithInfo(PyObject* self, PyObject* args)
{

	PyObject *csvin;
	PyObject *ptn;
	//int tp;
	if (!PyArg_ParseTuple(args, "OO", &csvin, &ptn)){

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
		long k = PyLong_AsLong ( PyList_GetItem(ptn,j) );
		char* v = kcfld->getOldVal(j);
		PyList_SetItem(rllist,j,setRtnData(k,v));
	}

	PyList_SetItem(finlist,0,rllist);

	if( kcfld->keybreak() ){
		PyList_SetItem(finlist,1,Py_True);
		Py_INCREF(Py_True);

	}
	else{
		PyList_SetItem(finlist,1,Py_False);
		Py_INCREF(Py_False);
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
	int paratype=0;
	if (!PyArg_ParseTuple(args, "s|i", &cmdname,&paratype)){
    return Py_BuildValue("");
  }

	kgshell ksh;
	
	if(paratype==0){
		PyObject* rlist = PyList_New(0);	
		ksh.getparams(cmdname,rlist);
		return rlist;
	}
	else if(paratype==1){
		PyObject* rlist = PyList_New(0);	
		ksh.getIoInfo(cmdname,rlist,0);
		return rlist;
	}
	else if(paratype==2){
		PyObject* rlist = PyList_New(0);	
		ksh.getIoInfo(cmdname,rlist,1);
		return rlist;
	}
	else if(paratype==3){
		PyObject* rlist  = PyList_New(0);
		PyObject* rlist1 = PyList_New(0);	
		PyObject* rlist2 = PyList_New(0);	
		PyObject* rlist3 = PyList_New(0);	
		ksh.getparams(cmdname,rlist1);
		ksh.getIoInfo(cmdname,rlist2,0);
		ksh.getIoInfo(cmdname,rlist3,1);

		PyList_Append(rlist,rlist1);
		Py_DECREF(rlist1);
		PyList_Append(rlist,rlist2);
		Py_DECREF(rlist2);
		PyList_Append(rlist,rlist3);
		Py_DECREF(rlist3);
		return rlist;

	}
  return Py_BuildValue("");
	
}




PyObject* start(PyObject* self, PyObject* args){
	int mflg;
	int rulim;
	int pymsg;
	size_t ttlmem;
	if (!PyArg_ParseTuple(args, "iiLi", &mflg , &rulim,&ttlmem,&pymsg)){
		return PyCapsule_New(new kgshell(false),"kgshellP",py_kgshell_free);
  }else{
		return PyCapsule_New(new kgshell(mflg,rulim,ttlmem,pymsg),"kgshellP",py_kgshell_free);
  }
}


static PyMethodDef callmethods[] = {
	{"init", 		reinterpret_cast<PyCFunction>(start), METH_VARARGS },
	{"runLx", 	reinterpret_cast<PyCFunction>(run), METH_VARARGS },
	{"runiter", reinterpret_cast<PyCFunction>(runITER), METH_VARARGS },
	{"cancel",  reinterpret_cast<PyCFunction>(cancel), METH_VARARGS },
	{"close",   reinterpret_cast<PyCFunction>(csvclose), METH_VARARGS },
	{"fldtp",   reinterpret_cast<PyCFunction>(fldtp), METH_VARARGS },
	{"fldname",   reinterpret_cast<PyCFunction>(fldname), METH_VARARGS },
	{"getLineList", reinterpret_cast<PyCFunction>(getLineList), METH_VARARGS },
	{"getLineDict",	reinterpret_cast<PyCFunction>(getLineDict), METH_VARARGS },
	{"getBlkList",	reinterpret_cast<PyCFunction>(getBlkList), METH_VARARGS },
	{"getBlkDict",	reinterpret_cast<PyCFunction>(getBlkDict), METH_VARARGS },
	{"getLineListWithInfo", reinterpret_cast<PyCFunction>(getLineListWithInfo), METH_VARARGS },
	{"getLineDictWithInfo",	reinterpret_cast<PyCFunction>(getLineDictWithInfo), METH_VARARGS },
	{"getparalist", reinterpret_cast<PyCFunction>(getparams), METH_VARARGS },
	{NULL}
};

#if PY_MAJOR_VERSION >= 3
static struct PyModuleDef moduledef = {
    PyModuleDef_HEAD_INIT,
    "_nysolshell_core",      /* m_name */
    NULL,							     /* m_doc */
    -1,                  /* m_size */
    callmethods,      /* m_methods */
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
	Py_InitModule("_nysolshell_core", callmethods);
}

#endif
