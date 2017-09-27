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
#include <kgload.h>
#include <kgError.h>
#include <kgConfig.h>

using namespace std;
using namespace kglib;
using namespace kgmod;

// -----------------------------------------------------------------------------
// コンストラクタ(モジュール名，バージョン登録,パラメータ)
// -----------------------------------------------------------------------------
kgLoad::kgLoad(void)
{
	_name    = "kgload";
	_version = "###VERSION###";

	_paralist = "i=,o=";
	_paraflg = kgArgs::COMMON|kgArgs::IODIFF;

	_titleL = _title = "";
//	_docL   = _doc  = "";
//	#ifdef JPN_FORMAT
//		#include <help/jp/kgtab2csvHelp.h>
//	#endif
	
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
void kgLoad::setArgs(int i_p,int o_p)
{
	// パラメータチェック
	_args.paramcheck("i=,o=",kgArgs::COMMON|kgArgs::IODIFF);

	// 入出力ファイルオープン
	if(i_p>0){
		_iFile.popen(i_p, _env,_nfn_i);
	}
	else{
		// 入出力ファイルオープン
		_iFile.open(_args.toString("i=",false), _env,_nfn_i);
	}
	if(o_p>0){
		_oFile.popen(o_p, _env,_nfn_o);
	}else{
		_oFile.open(_args.toString("o=",false), _env,_nfn_o);
	}
	_iFile.read_header();
	
}

// -----------------------------------------------------------------------------
// 実行
// -----------------------------------------------------------------------------
int kgLoad::run(void) try 
{
	char * data;
	size_t fcnt=0;

	// パラメータセット＆入出力ファイルオープン
	setArgs();

	// headerがあるとき
	if(!_nfn_i){
		vector<string> head;
		if(EOF != _iFile.read()){
			data = _iFile.getRec();
			string hdata = data;
			head = splitToken(hdata,',');
			fcnt = head.size();
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
	errorEnd(err);
	return 1;
}catch (const exception& e) {
	kgError err(e.what());
	errorEnd(err);
	return 1;
}catch(char * er){
	kgError err(er);
	errorEnd(err);
	return 1;
}catch(...){
	kgError err("unknown error" );
	errorEnd(err);
	return 1;
}

// -----------------------------------------------------------------------------
// 実行
// -----------------------------------------------------------------------------
int kgLoad::run(int i_p,int o_p) try 
{
	size_t fcnt=0;

	// パラメータセット＆入出力ファイルオープン
	setArgs(i_p,o_p);

	// headerがあるとき
	if(!_nfn_i){
		vector<string> head;
		if(EOF != _iFile.read()){
			char * data = _iFile.getRec();
			string hdata = data;
			head = splitToken(hdata,',');
			fcnt = head.size();
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
	errorEnd(err);
	return 1;
}catch (const exception& e) {
	kgError err(e.what());
	errorEnd(err);
	return 1;
}catch(char * er){
	kgError err(er);
	errorEnd(err);
	return 1;
}catch(...){
	kgError err("unknown error" );
	errorEnd(err);
	return 1;
}

// -----------------------------------------------------------------------------
// 実行
// -----------------------------------------------------------------------------
int kgLoad::run(PyObject* i_p,int o_p) try 
{
	size_t fcnt=0;

	// パラメータチェック
	_args.paramcheck("o=",kgArgs::COMMON|kgArgs::IODIFF);

	if(o_p>0){
		_oFile.popen(o_p, _env,_nfn_o);
	}else{
		_oFile.open(_args.toString("o=",false), _env,_nfn_o);
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
					headdata.push_back(PyBytes_AsString(PyList_GetItem(head,i)));
				}		
				nowlin++;
			}
			else{
				fldsize = PyList_Size(PyList_GetItem(i_p, nowlin));
			}
			// headerを出力するとき
			if(!_nfn_o){ _oFile.writeFldName(headdata);}
			// 行数を取得してデータ出力
			char ** vals = new char*[fldsize];
			while( nowlin < max ){
				PyObject* ddata = PyList_GetItem(i_p, nowlin);
				if( fldsize != PyList_Size(ddata) ){
					kgError err("unmatch fld size" );	
				}
				for(Py_ssize_t i=0 ; i<fldsize;i++){
					vals[i] = PyBytes_AsString(PyList_GetItem(ddata,i));
				}
				_oFile.writeFld(fldsize,vals);
				nowlin++;
			}
		}
	}else{
	
	}
	_oFile.close();
	successEnd();
	return 0;

}catch(kgError& err){
	errorEnd(err);
	return 1;
}catch (const exception& e) {
	kgError err(e.what());
	errorEnd(err);
	return 1;
}catch(char * er){
	kgError err(er);
	errorEnd(err);
	return 1;
}catch(...){
	kgError err("unknown error" );
	errorEnd(err);
	return 1;
}



