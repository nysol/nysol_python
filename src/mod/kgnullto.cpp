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
// kgnullto.cpp NULL値の置換クラス
// =============================================================================
#include <cstdio>
#include <sstream>
#include <string>
#include <vector>
#include <kgnullto.h>
#include <kgError.h>
#include <kgMethod.h>
#include <kgConfig.h>

using namespace std;
using namespace kglib;
using namespace kgmod;

// -----------------------------------------------------------------------------
// コンストラクタ(モジュール名，バージョン登録,パラメータ)
// -----------------------------------------------------------------------------
const char * kgNullto::_ipara[] = {"i",""};
const char * kgNullto::_opara[] = {"o",""};

kgNullto::kgNullto(void)
{
	_name    = "kgnullto";
	_version = "###VERSION###";

	_paralist = "k=,s=,f=,i=,o=,O=,v=,-A,-p";
	_paraflg = kgArgs::COMMON|kgArgs::IODIFF;

	#include <help/en/kgnulltoHelp.h>
	_titleL = _title;
	_docL   = _doc;
	#ifdef JPN_FORMAT
		#include <help/jp/kgnulltoHelp.h>
	#endif


}
// -----------------------------------------------------------------------------
// パラメータセット＆入出力ファイルオープン
// -----------------------------------------------------------------------------
void kgNullto::setArgsMain(void)
{
	_iFile.read_header();

	// k= 項目引数のセット
	vector<kgstr_t> vs = _args.toStringVector("k=",false);

	// -A（追加）,-p(前行文字列での置換) フラグセット
	_add_flg 		= _args.toBool("-A");
	_prv_flg 		= _args.toBool("-p");

	// f= 項目引数のセット
	vector< vector<kgstr_t> > vvs = _args.toStringVecVec("f=",':',2,true);
	_fField.set(vvs, &_iFile,_fldByNum);

	// v= 置換文字列
	_vField = _args.toString("v=",false);

	// O= NULL値以外の置換文字列
	_OField = _args.toString("O=",false);
	_exp_nul = !_OField.empty();

	if(_prv_flg && !_vField.empty()){ throw kgError("-p cannot be specified with v=");}
	if(!_prv_flg && _vField.empty()){ throw kgError("-p or v= must be specified"); }

	vector<kgstr_t> vss = _args.toStringVector("s=",false);
	bool seqflg = _args.toBool("-q");
	if(_nfn_i) { seqflg = true; }

	if(!seqflg && (!vs.empty()||!vss.empty())){ 
		vector<kgstr_t> vsk	= vs;
		vsk.insert(vsk.end(),vss.begin(),vss.end());
		sortingRun(&_iFile,vsk);
	}
	_kField.set(vs,  &_iFile, _fldByNum);

}

// -----------------------------------------------------------------------------
// パラメータセット＆入出力ファイルオープン
// -----------------------------------------------------------------------------
void kgNullto::setArgs(void)
{
	// パラメータチェック
	_args.paramcheck(_paralist,_paraflg);

	// 入出力ファイルオープン
	_iFile.open(_args.toString("i=",false), _env, _nfn_i);
	_oFile.open(_args.toString("o=",false), _env, _nfn_o);

	setArgsMain();

}

// -----------------------------------------------------------------------------
// パラメータセット＆入出力ファイルオープン
// -----------------------------------------------------------------------------
void kgNullto::setArgs(int inum,int *i_p,int onum, int* o_p)
{
	int iopencnt = 0;
	int oopencnt = 0;
	try{
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
int kgNullto::runMain(void)
{
	// 入力ファイルにkey項目番号をセットする．
	_iFile.setKey(_kField.getNum());

	//出力項目名出力 追加 or 置換
	if(_add_flg) { _oFile.writeFldName(_iFile,_fField,true);}
	else				 { _oFile.writeFldName(_fField, true);}

	//	fieldの項目番号をセット
	vector<int> field_lst = _fField.getNum();

	// 前行保存用配列(-p用)
	vector<string> prvRec(_fField.size());

	// OUT領域確保:f=項目数分
	kgAutoPtr2<char*> o_stock_ap;
	try {
		o_stock_ap.set( new char*[_fField.size()] ); 
	} catch(...) {
		throw kgError("memory allocation error ");
	}
	char** stock = o_stock_ap.get();

	while( EOF != _iFile.read() ){

		if( _iFile.keybreak() ){
			if((_iFile.status() & kgCSV::End )) break;
			if(_prv_flg){
				for(size_t i=0 ; i<_fField.size();i++){			
					 prvRec.at(i)="";
				}
			}
		}
		for(size_t i=0 ; i<_fField.size();i++){
			char* val=_iFile.getNewVal(_fField.num(i));
			if(*val=='\0'){
				if(_prv_flg){ *(stock+i) = const_cast<char*>(prvRec.at(i).c_str());}
				else				{ *(stock+i) = const_cast<char*>(_vField.c_str());}
			}else{
				if(_exp_nul){ *(stock+i) = const_cast<char*>(_OField.c_str());}
				else				{ *(stock+i) = val;}
				//前回値としてセット
				if(_prv_flg){ prvRec.at(i)=val;}	
			}
		}
		if(_add_flg){
			_oFile.writeFld(_iFile.getNewFld(),_iFile.fldSize(),stock,_fField.size());		
		}else{
			_oFile.writeFld(_iFile.getNewFld(),_fField.getFlg_p(),stock);
		}
	}

	// 終了処理
	th_cancel();
	_iFile.close();
	_oFile.close();

	return 0;

}
// -----------------------------------------------------------------------------
// 実行 
// -----------------------------------------------------------------------------
int kgNullto::run(void) 
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
    ((kgNullto*)arg)->runErrEnd();
}

int kgNullto::run(int inum,int *i_p,int onum, int* o_p,string &msg)
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


	return 1;
}
