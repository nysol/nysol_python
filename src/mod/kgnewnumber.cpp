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
// kgnewnumber.cpp 連続値項目の新規生成クラス
// =============================================================================
#include <sstream>
#include <kgnewnumber.h>
#include <kgError.h>
#include <kgMethod.h>

using namespace std;
using namespace kglib;
using namespace kgmod;

// -----------------------------------------------------------------------------
// コンストラクタ(モジュール名，バージョン登録)
// -----------------------------------------------------------------------------
const char * kgNewnumber::_ipara[] = {""};
const char * kgNewnumber::_opara[] = {"o",""};

kgNewnumber::kgNewnumber(void)
{
	_name    = "kgNewnumber";
	_version = "###VERSION###";

	_paralist = "a=,o=,S=,I=,l=";
	
	
	#include <help/en/kgnewnumberHelp.h>
	_titleL = _title;
	_docL   = _doc;
	#ifdef JPN_FORMAT
		#include <help/jp/kgnewnumberHelp.h>
	#endif
	
}
// -----------------------------------------------------------------------------
// パラメータセット＆入出力ファイルオープン
// -----------------------------------------------------------------------------
void kgNewnumber::setArgsMain(void)
{
	// a= 追加項目名
	_addstr = _args.toString("a=",false);
	if(_addstr.empty()&& _nfn_o==false){
		throw kgError("parameter a= is mandatory");
	}

	// S= 開始番号
	_alpha_flg=false;			
	string str_S = _args.toString("S=",false);
	if(str_S.empty()){ _start=0; }
	else{	
		string digits("-0123456789");
		string alpha("ABCDEFGHIJKLMNOPQRSTUVWXYZ");
		if(str_S.find_first_not_of(digits)==kgstr_t::npos){//数値のみ
			_start=atoi(str_S.c_str());
		}
		else if(str_S.find_first_not_of(alpha)==kgstr_t::npos){//アルファベット
			_alpha_flg=true;			
			_start=0;
			for(string::size_type i=0;i<str_S.size();i++){
				_start = (_start*26)+str_S[i]-'A'+1;
			}					
		}
		else{
			ostringstream ss;
			ss << "S= is digits or alpha" << str_S.c_str();	
			throw kgError(ss.str());
		}
	}

	// I= 間隔
	string str_I = _args.toString("I=",false);
	_interval=atoi(str_I.c_str());
	if(_interval==0){ _interval=1;}
	if(_alpha_flg&&_interval<0){
		ostringstream ss;
		ss << "when S= is alpha, 0 or more can be specified with I=" << str_S.c_str();	
		throw kgError(ss.str());
	}

	kgstr_t s_l =  _args.toString("l=",false);
	if(s_l.empty()){
		_line = 10;
	}else{
		_line = aToSizeT(s_l.c_str()) ; 
	}
	
}

// -----------------------------------------------------------------------------
// パラメータセット＆入出力ファイルオープン
// -----------------------------------------------------------------------------
void kgNewnumber::setArgs(void)
{
	_args.paramcheck(_paralist);

  _oFile.open(_args.toString("o=",false), _env,_nfn_o);

	setArgsMain();
	
}

// -----------------------------------------------------------------------------
// パラメータセット＆入出力ファイルオープン
// -----------------------------------------------------------------------------
void kgNewnumber::setArgs(int inum,int *i_p,int onum ,int *o_p)
{
	int iopencnt = 0;
	int oopencnt = 0;
	try{
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
int kgNewnumber::runMain(void)
{

	// 項目名の出力
	_oFile.writeFldName(_addstr);

	// データ出力
	size_t cnt = _start;
	for(size_t i=0;i<_line;i++){	
		_oFile.writeFld(NULL,0,cnt,_alpha_flg);
		cnt = cnt + _interval; 
	}

	// 終了処理(メッセージ出力,thread pipe終了通知)
	_oFile.close();

	return 0;

}
// -----------------------------------------------------------------------------
// 実行 
// -----------------------------------------------------------------------------
int kgNewnumber::run(void) 
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
    ((kgNewnumber*)arg)->runErrEnd();
}

int kgNewnumber::run(int inum,int *i_p,int onum, int* o_p,string &msg)
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
