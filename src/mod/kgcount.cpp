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
// kgcount.cpp 行カウントクラス
// =============================================================================
#include <cstdio>
#include <kgcount.h>
#include <kgError.h>

using namespace std;
using namespace kglib;
using namespace kgmod;

// -----------------------------------------------------------------------------
// コンストラクタ(モジュール名，バージョン登録)
// -----------------------------------------------------------------------------
const char * kgCount::_ipara[] = {"i",""};
const char * kgCount::_opara[] = {"o",""};

kgCount::kgCount(void)
{
	_name    = "kgcount";
	_version = "###VERSION###";

	_paralist = "a=,i=,o=,k=,-q";
	_paraflg = kgArgs::COMMON|kgArgs::IODIFF|kgArgs::NULL_KEY;

	#include <help/en/kgcountHelp.h>
	_titleL = _title;
	_docL   = _doc;
	#ifdef JPN_FORMAT
		#include <help/jp/kgcountHelp.h>
	#endif
}
// -----------------------------------------------------------------------------
// パラメータセット＆入出力ファイルオープン
// -----------------------------------------------------------------------------
void kgCount::setArgsMain(void)
{
	_iFile.read_header();

	// a= 追加文字列
	_addstr = _args.toString("a=",false);
	if(_addstr.empty()&& _nfn_o==false){
		throw kgError("parameter a= is mandatory");
	}

	// k= 項目引数のセット
	vector<kgstr_t> vs = _args.toStringVector("k=",false);
	
	bool seqflg = _args.toBool("-q");
	if(_nfn_i) { seqflg = true; }

	if(!seqflg && !vs.empty()){ sortingRun(&_iFile,vs);}
	
	_kField.set(vs,  &_iFile, _fldByNum);
}

// -----------------------------------------------------------------------------
// パラメータセット＆入出力ファイルオープン
// -----------------------------------------------------------------------------
void kgCount::setArgs(void)
{
	// パラメータチェック
	_args.paramcheck(_paralist,_paraflg);

	// 入出力ファイルオープン
	_iFile.open(_args.toString("i=",false), _env,_nfn_i);
  _oFile.open(_args.toString("o=",false), _env,_nfn_o);

}
// -----------------------------------------------------------------------------
void kgCount::setArgs(int inum,int *i_p,int onum, int* o_p)
{
	int iopencnt = 0;
	int oopencnt = 0;
	try{

		// パラメータチェック
		_args.paramcheck(_paralist,_paraflg);


		if(inum>1 || onum>1){ throw kgError("no match IO");}

		if(inum==1 && *i_p>0){ _iFile.popen(*i_p, _env,_nfn_i); }
		else     { _iFile.open(_args.toString("i=",true), _env,_nfn_i); }
		iopencnt++; 

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
int kgCount::runMain(void)
{
	// 入力ファイルにkey項目番号をセットする．
	_iFile.setKey(_kField.getNum());

	// 項目名の出力
  _oFile.writeFldName(_iFile,_addstr);

	size_t cnt = 0;
	// データ集計＆出力
	while(_iFile.read()!=EOF){		
		//keybreakしたら出力&cnt初期化
		if( _iFile.keybreak() ){
			_oFile.writeFld(_iFile.getOldFld(),_iFile.fldSize(),cnt,false);
			if((_iFile.status() & kgCSV::End )) break;
			cnt=0;				
		}
		cnt++;
	}

	//ASSERT keynull_CHECK
	if(_assertNullKEY) { _existNullKEY = _iFile.keynull(); }

	// 終了処理
	th_cancel();
	_iFile.close();
	_oFile.close();

	return 0;

}

// -----------------------------------------------------------------------------
// 実行 
// -----------------------------------------------------------------------------
int kgCount::run(void) 
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
	}
	KG_ABI_CATCH
	catch(...){

		runErrEnd();
		kgError err("unknown error" );
		errorEnd(err);
	}
	return 1;

}

///* thraad cancel action
static void cleanup_handler(void *arg)
{
    ((kgCount*)arg)->runErrEnd();
}

int kgCount::run(int inum,int *i_p,int onum, int* o_p,string &msg)
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

	}catch(...){

		runErrEnd();
		kgError err("unknown error" );
		msg.append(errorEndMsg(err));

	}
  pthread_cleanup_pop(0);
	return sts;
}

