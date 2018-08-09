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
// kgnewstr.cpp 固定文字列項目の新規作成クラス
// =============================================================================
#include <kgnewstr.h>
#include <kgCSVout.h>
#include <kgError.h>
#include <kgMethod.h>

using namespace std;
using namespace kglib;
using namespace kgmod;

// -----------------------------------------------------------------------------
// コンストラクタ(モジュール名，バージョン登録,パラメータ)
// -----------------------------------------------------------------------------
const char * kgNewstr::_ipara[] = {""};
const char * kgNewstr::_opara[] = {"o",""};

kgNewstr::kgNewstr(void)
{
	_name    = "kgnewstr";
	_version = "###VERSION###";
	_paralist = "v=,o=,a=,l=";
	#include <help/en/kgnewstrHelp.h>
	_titleL = _title;
	_docL   = _doc;
	#ifdef JPN_FORMAT
		#include <help/jp/kgnewstrHelp.h>
	#endif
	
}
// -----------------------------------------------------------------------------
// 入出力ファイルオープン
// -----------------------------------------------------------------------------
void kgNewstr::setArgsMain(void)
{

	// a=,v= 項目引数のセット
	_aField = _args.toStringVector("a=",false);
	if(_aField.empty()&& _nfn_o==false){
		throw kgError("parameter a= is mandatory");
	}
	_vField = _args.toStringVector("v=",true);
	if(_aField.size()!=_vField.size()&& _nfn_o==false){
		throw kgError("item size of parameters a= and v= must be same");
	}

	kgstr_t s_l =  _args.toString("l=",false);
	if(s_l.empty()){
		_line = 10;
	}else{
		_line = aToSizeT(s_l.c_str()) ; 
	}
}

// -----------------------------------------------------------------------------
// 入出力ファイルオープン
// -----------------------------------------------------------------------------
void kgNewstr::setArgs(void)
{
	// パラメータチェック
	_args.paramcheck(_paralist);

	// 出力ファイルオープン
	_oFile.open(_args.toString("o=",false), _env, _nfn_o);

	setArgsMain();

}
// -----------------------------------------------------------------------------
// 入出力ファイルオープン
// -----------------------------------------------------------------------------
void kgNewstr::setArgs(int inum,int *i_p,int onum ,int *o_p)
{
	int iopencnt = 0;
	int oopencnt = 0;
	try{
		// パラメータチェック
		_args.paramcheck(_paralist);

		if(inum>0 || onum>1){ throw kgError("no match IO");}

		if(onum==1 && *o_p>0){ _oFile.popen(*o_p, _env,_nfn_o); }
		else     { _oFile.open(_args.toString("o=",true), _env,_nfn_o);}
		oopencnt++;

		setArgsMain();

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
int kgNewstr::runMain(void)
{

	// 項目名の出力
	_oFile.writeFldName(_aField);

	// データ出力
	vector<kgstr_t>::size_type size=_vField.size();
	for(size_t i=0;i<_line;i++){	
		for(vector<string>::size_type j=0;j<size;j++){	
			if(j==size-1) _oFile.writeStr(_vField.at(j).c_str(),true );
			else          _oFile.writeStr(_vField.at(j).c_str(),false);
		}
	}
	// 終了処理
	_oFile.close();

	return 0;
	
}


// -----------------------------------------------------------------------------
// 実行 
// -----------------------------------------------------------------------------
int kgNewstr::run(void) 
{
	try {

		setArgs();
		int sts = runMain();
		successEnd();
		return sts;

	}catch(kgOPipeBreakError& err){

		runErrEnd();
		successEnd();
		return 0;

	}catch(kgError& err){

		runErrEnd();
		errorEnd(err);
	}catch (const exception& e) {

		runErrEnd();
		kgError err(e.what());
		errorEnd(err);
	}catch(char * er){

		runErrEnd();
		kgError err(er);
		errorEnd(err);
	}catch(...){

		runErrEnd();
		kgError err("unknown error" );
		errorEnd(err);
	}
	return 1;

}
///* thraad cancel action
static void cleanup_handler(void *arg)
{
    ((kgNewstr*)arg)->runErrEnd();
}

int kgNewstr::run(int inum,int *i_p,int onum, int* o_p,string &msg)
{

	int sts=1;
	pthread_cleanup_push(&cleanup_handler, this);	

	try {

		setArgs(inum, i_p, onum,o_p);
		sts = runMain();
		msg.append(successEndMsg());

	}catch(kgOPipeBreakError& err){

		runErrEnd();
		msg.append(successEndMsg());
		sts = 0;

	}catch(kgError& err){

		runErrEnd();
		msg.append(errorEndMsg(err));

	}catch (const exception& e) {

		runErrEnd();
		kgError err(e.what());
		msg.append(errorEndMsg(err));

	}catch(char * er){

		runErrEnd();
		kgError err(er);
		msg.append(errorEndMsg(err));

	}
	KG_ABI_CATCH
	catch(...){

		runErrEnd();
		kgError err("unknown error" );
		msg.append(errorEndMsg(err));

	}
  pthread_cleanup_pop(0);
	return sts;

}

