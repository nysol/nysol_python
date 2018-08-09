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
// Keybreak.cpp キーブレーク情報付加クラス
// =============================================================================
#include <cstdio>
#include <vector>
#include <kgConfig.h>
#include <kgkeybreak.h>
#include <kgError.h>
#include <kgVal.h>

using namespace std;
using namespace kglib;
using namespace kgmod;

// -----------------------------------------------------------------------------
// コンストラクタ(モジュール名，バージョン登録)
// -----------------------------------------------------------------------------
const char * kgKeybreak::_ipara[] = {"i",""};
const char * kgKeybreak::_opara[] = {"o",""};

kgKeybreak::kgKeybreak(void)
{
	_name    = "kgkeybreak";
	_version = "###VERSION###";

	_paralist = "a=,i=,o=,k=,s=,-q";
	_paraflg = kgArgs::COMMON|kgArgs::IODIFF|kgArgs::NULL_KEY|kgArgs::NULL_OUT;

	#include <help/en/kgkeybreakHelp.h>
	_titleL = _title;
	_docL   = _doc;
	#ifdef JPN_FORMAT
		#include <help/jp/kgkeybreakHelp.h>
	#endif

}
// -----------------------------------------------------------------------------
// 入出力ファイルオープン
// -----------------------------------------------------------------------------
void kgKeybreak::setArgsMain(void)
{
	_iFile.read_header();
	_oFile.setPrecision(_precision);

	// k= 項目引数のセット
	vector<kgstr_t> vs = _args.toStringVector("k=",true);

	// a=は２項目足りないときは自動的保管され多い場合は無視される
	_addstr.resize(2);
	_addstr[0]="top";
	_addstr[1]="bot";
	vector<kgstr_t> vs_a = _args.toStringVector("a=",false);
	size_t alim = vs_a.size();
	if ( alim > 2 ) { alim=2; }
	for(size_t i=0 ; i< alim ;i++){
		if(vs_a[i].empty()){ continue; }
		_addstr[i]=vs_a[i];
	}

	bool seqflg = _args.toBool("-q");
	if(_nfn_i) { seqflg = true; }
	vector<kgstr_t> vss = _args.toStringVector("s=",false);

	if( !seqflg ){
		vector<kgstr_t> vsk	= vs;
		vsk.insert(vsk.end(),vss.begin(),vss.end());
		sortingRun(&_iFile,vsk);
	}

	_kField.set(vs,  &_iFile,_fldByNum);

}

// -----------------------------------------------------------------------------
// 入出力ファイルオープン
// -----------------------------------------------------------------------------
void kgKeybreak::setArgs(void)
{
	// パラメータチェック
	_args.paramcheck("a=,i=,o=,k=,s=,-q",kgArgs::COMMON|kgArgs::IODIFF|kgArgs::NULL_KEY|kgArgs::NULL_OUT);

	// 入出力ファイルオープン
	_iFile.open(_args.toString("i=",false), _env,_nfn_i);
  _oFile.open(_args.toString("o=",false), _env,_nfn_o);

	setArgsMain();

}

// -----------------------------------------------------------------------------
// 入出力ファイルオープン
// -----------------------------------------------------------------------------
void kgKeybreak::setArgs(int inum,int *i_p,int onum, int* o_p)
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
int kgKeybreak::runMain(void)
{
	// 入力ファイルにkey項目番号をセットする．
	_iFile.setKey(_kField.getNum());

	// 項目名の出力
  _oFile.writeFldName(_iFile, _addstr);

	// break flg
	bool top = true;
	bool bot  = false;
	bool kb    = true;
	vector<kgstr_t> output(2);

	while(_iFile.read()!=EOF){
	
		if(kb){ top = true;}
		else	{ top = false;}

		if( _iFile.keybreak() )	{ 
			bot = true;
			kb   = true;
		}
		else{
			bot = false;
			kb   = false;
		}

		if(_iFile.begin()){
			kb   = true;
			continue;
		}
		// アウトプット
		if(top) { output[0]="1";}
		else		{ output[0]="";}
		if(bot) { output[1]="1";}
		else		{ output[1]="";}
		if(_assertNullOUT && output[0].size()==0 && output[1].size()==0){ _existNullOUT = true;}

		_oFile.writeFld(_iFile.getOldFld(),_iFile.fldSize(),&output);
	}

	//ASSERT keynull_CHECK
	if(_assertNullKEY) { _existNullKEY = _iFile.keynull(); }

	// 終了処理(メッセージ出力,thread pipe終了通知)
	th_cancel();
	_iFile.close();
	_oFile.close();

	return 0;

}
// -----------------------------------------------------------------------------
// 実行 
// -----------------------------------------------------------------------------
int kgKeybreak::run(void) 
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
    ((kgKeybreak*)arg)->runErrEnd();
}

int kgKeybreak::run(int inum,int *i_p,int onum, int* o_p,string &msg)
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

