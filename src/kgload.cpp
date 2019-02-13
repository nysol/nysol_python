/* ////////// LICENSE INFO ////////////////////

 * Copyright (C) 2013 by NYSOL CORPORATION
 *
 * Unless you have received this program directly from NYSOL pursuant
 * to the terms of a commercial license agreement with NYSOL, then
 * this program is licensed to you under the terms of the GNU Affero General
 * Public License (AGPL) as published by the Free Software Foundation,
 * either version 3 of the License, or (at your option) any later version.
 * 
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY EXPRESS OR IMPLIED WARRANTY, INCLUDING THOSE OF 
 * NON-INFRINGEMENT, MERCHANTABILITY OR FITNESS FOR A PARTICULAR PURPOSE.
 *
 * Please refer to the AGPL (http://www.gnu.org/licenses/agpl-3.0.txt)
 * for more details.

 ////////// LICENSE INFO ////////////////////*/
// =============================================================================
// kgLoad.cpp 行の複製
// =============================================================================
#include <cstdio>
#include <math.h>
#include <kgload.h>
#include <kgError.h>
#include <kgConfig.h>
#include <stdarg.h>

using namespace std;
using namespace kglib;
using namespace kgmod;

/*static char* strGET(PyObject* data){
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



static bool strCHECK(PyObject* data){

#if PY_MAJOR_VERSION >= 3
	return PyUnicode_Check(data);
#else		
	return PyString_Check(data);
#endif

}


// -----------------------------------------------------------------------------
// コンストラクタ(モジュール名，バージョン登録,パラメータ)
// -----------------------------------------------------------------------------
const char * kgLoad::_ipara[] = {"i",""};
const char * kgLoad::_opara[] = {"o",""};

kgLoad::kgLoad(void)
{
	_name    = "kgload";
	_version = "###VERSION###";

	_paralist = "i=,o=";
	_paraflg = kgArgs::COMMON|kgArgs::IODIFF;

	_titleL = _title = "";
}
// -----------------------------------------------------------------------------
// パラメータセット＆入出力ファイルオープン
// -----------------------------------------------------------------------------
void kgLoad::setArgs(void)
{
	// パラメータチェック
	_args.paramcheck("i=,o=",kgArgs::COMMON|kgArgs::IODIFF);

	// 入出力ファイルオープン
	_iFile.open(_args.toString("i=",false), _env, _nfn_i);
	_oFile.open(_args.toString("o=",false), _env, _nfn_o);

	_iFile.read_header();
	
}

// -----------------------------------------------------------------------------
// パラメータセット＆入出力ファイルオープン
// -----------------------------------------------------------------------------
void kgLoad::setArgs(int inum,int *i_p,int onum,int* o_p)
{
	int iopencnt = 0;
	int oopencnt = 0;

	try{
		// パラメータチェック
		_args.paramcheck("i=,o=",kgArgs::COMMON|kgArgs::IODIFF);

		if(inum>1 || onum>1){
			throw kgError("no match IO");
		}

		// 入出力ファイルオープン
		if(inum==1 && *i_p > 0){ _iFile.popen(*i_p, _env,_nfn_i); }
		else     { 
			_iFile.open(_args.toString("i=",false), _env,_nfn_i);
		}
		iopencnt++;

		if(onum==1 && *o_p > 0){ _oFile.popen(*o_p, _env,_nfn_o); }
		else     { _oFile.open(_args.toString("o=",false), _env,_nfn_o);}
		oopencnt++;


		_iFile.read_header();

	}catch(...){
		for(int i=iopencnt; i<inum ;i++){
			if(*(i_p+i)>0){ ::close(*(i_p+i)); }
		}
		for(int i=oopencnt; i<onum ;i++){
			if(*(o_p+i)>0){ ::close(*(o_p+i)); }
		}
		throw;
	}
	
}

// -----------------------------------------------------------------------------
// 実行
// -----------------------------------------------------------------------------
int kgLoad::run(void)
{
	try {
		//size_t fcnt=0;
		// パラメータセット＆入出力ファイルオープン
		setArgs();

		// headerがあるとき
		if(!_nfn_i){
			vector<string> head;
			if(EOF != _iFile.read()){
				char * data = _iFile.getRec();
				string hdata = data;
				head = splitToken(hdata,',');
				//fcnt = head.size();
			}
			// headerを出力するとき
			if(!_nfn_o){ _oFile.writeFldName(head);}
		}
		// 行数を取得してデータ出力
		while( EOF != _iFile.read() ){
			_oFile.writeRec(_iFile.getRec());
		}
		// 終了処理
		_iFile.close();
		_oFile.close();
		successEnd();
		return 0;
	}catch(kgError& err){
		_iFile.close();
		_oFile.close();
		errorEnd(err);
		return 1;
	}catch (const exception& e) {
		_iFile.close();
		_oFile.close();
		kgError err(e.what());
		errorEnd(err);
		return 1;
	}catch(char * er){
		_iFile.close();
		_oFile.close();
		kgError err(er);
		errorEnd(err);
		return 1;
	}catch(...){
		_iFile.close();
		_oFile.close();
		kgError err("unknown error" );
		errorEnd(err);
		return 1;
	}
	return 1;
}

// -----------------------------------------------------------------------------
// 実行
// -----------------------------------------------------------------------------
int kgLoad::run(int inum,int *i_p,int onum, int* o_p,string &msg)
{
	try {
		//size_t fcnt=0;
		// パラメータセット＆入出力ファイルオープン
		setArgs(inum, i_p,onum, o_p);
		// headerがあるとき
		if(!_nfn_i){
			vector<string> head;
			if(EOF != _iFile.read()){
				char * data = _iFile.getRec();
				string hdata = data;
				head = splitToken(hdata,',');
				//fcnt = head.size();
			}
			// headerを出力するとき
			if(!_nfn_o){ _oFile.writeFldName(head);}
		}
		// 行数を取得してデータ出力
		while( EOF != _iFile.read() ){
			_oFile.writeRec(_iFile.getRec());
		}
		// 終了処理
		_iFile.close();
		_oFile.close();
		msg.append(successEndMsg());
		return 0;

	}catch(kgError& err){
		_iFile.close();
		_oFile.close();
		msg.append(errorEndMsg(err));

	}catch (const exception& e) {
		_iFile.close();
		_oFile.close();
		kgError err(e.what());
		msg.append(errorEndMsg(err));

	}catch(char * er){
		_iFile.close();
		_oFile.close();
		kgError err(er);
		msg.append(errorEndMsg(err));

	}
	//KG_ABI_CATCH
	catch(...){
		_iFile.close();
		_oFile.close();
		kgError err("unknown error" );
		msg.append(errorEndMsg(err));
	}
	return 1;
}

// -----------------------------------------------------------------------------
// 実行
// -----------------------------------------------------------------------------
int kgLoad::run(PyObject* i_p,int onum,int *o_p,string &msg) 
{
	try {
		// パラメータチェック
		_args.paramcheck("o=",kgArgs::COMMON|kgArgs::IODIFF);

		if(onum>1){
			for(int i=0; i<onum ;i++){
				if(*(o_p+i)>0){ ::close(*(o_p+i)); }
			}
			throw kgError("no match IO");
		}
		if(onum==1 && *o_p > 0){ _oFile.popen(*o_p, _env,_nfn_o); }
		else{ 
			try{
				_oFile.open(_args.toString("o=",true), _env,_nfn_o);
			}catch(...){
				for(int i=0; i<onum ;i++){
					if(*(o_p+i)>0){ ::close(*(o_p+i)); }
				}
				throw;
			}
		}

		if(PyList_Check(i_p)){
			Py_ssize_t max = PyList_Size(i_p);
			Py_ssize_t fldsize = 0;
			Py_ssize_t nowlin = 0;
			vector<string> headdata;
			if ( max > 0 ){
				// headerがあるとき
				if(!_nfn_i){
					PyObject* head = PyList_GetItem(i_p, nowlin);
					fldsize = PyList_Size(head);
					for(Py_ssize_t i=0 ; i<fldsize;i++){
						if(!strCHECK(PyList_GetItem(head,i))){
							throw kgError("unsupport data type");
						}
						headdata.push_back(strGET(PyList_GetItem(head,i)));
					}		
					nowlin++;
				}
				else{
					fldsize = PyList_Size(PyList_GetItem(i_p, nowlin));
				}
				// headerを出力するとき
				if(!_nfn_o){ _oFile.writeFldName(headdata);}
				// 行数を取得してデータ出力
				while( nowlin < max ){
					PyObject* ddata = PyList_GetItem(i_p, nowlin);
					if( fldsize != PyList_Size(ddata) ){
						throw kgError("unmatch field size" );	
					}
					for(Py_ssize_t i=0 ; i<fldsize;i++){
						PyObject* fval = PyList_GetItem(ddata,i);
						if(strCHECK(fval)){
							_oFile.writeStr(strGET(fval), i==fldsize-1);
						}
						else if (PyLong_Check(fval)){
							_oFile.writeDbl(PyLong_AsDouble(fval), i==fldsize-1);
						}
						else if (PyFloat_Check(fval)){
							double d=PyFloat_AsDouble(fval);
							if(isnan(d)||isinf(d)){
								_oFile.writeStr("", i==fldsize-1);
							}
							else{
								_oFile.writeDbl(d, i==fldsize-1);
							}
						}
						else if (Py_None == fval){
							_oFile.writeStr("", i==fldsize-1);
						}
						else{
							throw kgError("unsupport data type");
						}
					}
					nowlin++;
				}
			}
		}else{
			throw kgError("not python list");
		}
		_oFile.close();
		msg.append(successEndMsg());
		return 0;

	}catch(kgError& err){
		_oFile.close();
		msg.append(errorEndMsg(err));

	}catch (const exception& e) {
		_oFile.close();
		kgError err(e.what());
		msg.append(errorEndMsg(err));
	}catch(char * er){
		_oFile.close();
		kgError err(er);
		msg.append(errorEndMsg(err));
	}
	//KG_ABI_CATCH
	catch(...){
		_oFile.close();
		kgError err("unknown error" );
		msg.append(errorEndMsg(err));
		throw;
	}
	return 1;

}
/*
static makeVal(format,...){


}*/
// -----------------------------------------------------------------------------
// 実行
// -----------------------------------------------------------------------------
int kgLoad::run(int inum,int *i_p,PyObject* o_p,pthread_mutex_t *mtx,string &msg) 
{

	PyThreadState *savex = NULL;
	try {

		savex  = PyEval_SaveThread();

		// パラメータチェック
		_args.paramcheck("i=,dtype=,-header",kgArgs::COMMON|kgArgs::IODIFF);
		if(inum>1){ 
			for(int i=0; i<inum ;i++){
				if(*(i_p+i)>0){ ::close(*(i_p+i)); }
			}
			throw kgError("no match IO"); 
		}

		kgCSVfld rls;

		// 入出力ファイルオープン
		if(inum==1 && *i_p > 0){ rls.popen(*i_p, _env,_nfn_i); }
		else     { 
			try{
				rls.open(_args.toString("i=",true), _env,_nfn_i); 
			}catch(...){
				for(int i=0; i<inum ;i++){
					if(*(i_p+i)>0){ ::close(*(i_p+i)); }
				}
				throw;
			}
		}


		rls.read_header();

		vector< vector<kgstr_t> > vvs = _args.toStringVecVec("dtype=",':',2,false);
		kgArgFld fFieldx;
		fFieldx.set(vvs, &rls,_fldByNum);
		//0:str
		//1:int
		//2:float
		//3:bool

		vector<int> ptn(rls.fldSize(),0);
		for(vector<kgstr_t>::size_type i=0; i<fFieldx.size(); i++){
			if(fFieldx.attr(i)=="int"){
				ptn[fFieldx.num(i)] = 1;
			}
			else if(fFieldx.attr(i)=="float"){
				ptn[fFieldx.num(i)] = 2;
			}
			else if(fFieldx.attr(i)=="bool"){
				ptn[fFieldx.num(i)] = 3;
			}
		}
		bool addhead = _args.toBool("-header");

		PyEval_RestoreThread(savex);
		savex=NULL;

		if(PyList_Check(o_p)){

			//PyGILState_STATE gstate;
			//pthread_mutex_lock(mtx);
			//{
			//	gstate = PyGILState_Ensure();
			//}
			//pthread_mutex_unlock(mtx);

			if(addhead){ 
				vector<kgstr_t> flds = rls.fldName();
				///PyEval_RestoreThread(savex);
				//pthread_mutex_lock(mtx);
				//{
					//PyGILState_STATE gstate;
					//gstate = PyGILState_Ensure();
					PyObject* hlist = PyList_New(flds.size());
					for(size_t i=0; i < flds.size();i++){
						const char * p = flds[i].c_str();
						PyList_SET_ITEM(hlist,i,PyUnicode_FromStringAndSize(p, strlen(p)));
					}
					PyList_Append(o_p,hlist);
					Py_DECREF(hlist);
					//PyGILState_Release(gstate);
				//}
				//pthread_mutex_unlock(mtx);
				///savex = PyEval_SaveThread();
			}

			savex = PyEval_SaveThread();

			while( EOF != rls.read() ){

				PyEval_RestoreThread(savex);
				savex=NULL;


				PyObject* tlist = PyList_New(rls.fldSize());
					
				for(size_t j=0 ;j<rls.fldSize();j++){

					char * p = rls.getVal(j);

					if(*p=='\0'){
						
						if(ptn[j]==0){
							PyList_SET_ITEM(tlist,j,PyUnicode_FromStringAndSize(p, strlen(p)));
						}
						else{
							Py_INCREF(Py_None);
							PyList_SET_ITEM(tlist,j,Py_None);
						}

					}
					else if(ptn[j]==0){

						PyList_SET_ITEM(tlist,j,PyUnicode_FromStringAndSize(p, strlen(p)));

					}
					else if(ptn[j]==1){

						PyList_SET_ITEM(tlist,j,PyLong_FromLong(atol(p)));

					}
					else if(ptn[j]==2){

						PyList_SET_ITEM(tlist,j,PyFloat_FromDouble(atof(p)));

					}
					else if(ptn[j]==3){
	
						if(strlen(p)==1 && *p=='0'){

							Py_INCREF(Py_False);
							PyList_SET_ITEM(tlist,j,Py_False);
							
						}else{

							Py_INCREF(Py_True);
							PyList_SET_ITEM(tlist,j,Py_True);
							
						}

					}
				}
				PyList_Append(o_p,tlist);
				Py_DECREF(tlist);
				savex = PyEval_SaveThread();
			}
			
			PyEval_RestoreThread(savex);
			savex=NULL;

		}
		else{
			throw kgError("not python list");
		}
		rls.close();
		msg.append(successEndMsg());
		return 0;

	}
	catch(kgError& err){
		if(savex!=NULL){ PyEval_RestoreThread(savex);	}
		msg.append(errorEndMsg(err));

	}catch (const exception& e) {
		if(savex!=NULL){ PyEval_RestoreThread(savex);	}
		kgError err(e.what());
		msg.append(errorEndMsg(err));

	}catch(char * er){
		if(savex!=NULL){ PyEval_RestoreThread(savex);	}
		kgError err(er);
		msg.append(errorEndMsg(err));

	}
	// KG_ABI_CATCH 
	catch(...){
		if(savex!=NULL){ PyEval_RestoreThread(savex);	}
		kgError err("unknown error" );
		msg.append(errorEndMsg(err));
		throw;
	}
	return 1;
}
