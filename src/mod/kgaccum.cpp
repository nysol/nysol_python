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
// kgaccum.cpp 累積計算クラス
// =============================================================================
#include <cstdio>
#include <sstream>
#include <vector>
#include <kgaccum.h>
#include <kgVal.h>
#include <kgError.h>
#include <kgConfig.h>

using namespace std;
using namespace kglib;
using namespace kgmod;

// -----------------------------------------------------------------------------
// コンストラクタ(モジュール名，バージョン登録)
// -----------------------------------------------------------------------------
const char * kgAccum::_ipara[] = {"i",""};
const char * kgAccum::_opara[] = {"o",""};

kgAccum::kgAccum(void)
{
	_name    = "kgaccum";
	_version = "###VERSION###";
	_paralist = "f=,i=,o=,k=,s=,-q";
	_paraflg = kgArgs::ALLPARAM;

	#include <help/en/kgaccumHelp.h>
	_titleL = _title;
	_docL   = _doc;
	#ifdef JPN_FORMAT
		#include <help/jp/kgaccumHelp.h>
	#endif

}
// -----------------------------------------------------------------------------
// パラメータセット
// -----------------------------------------------------------------------------
void kgAccum::setArgsMain(void)
{
	_iFile.read_header();
	// f= 項目引数のセット
	vector< vector<kgstr_t> > vvs = _args.toStringVecVec("f=",':',2,true);
	// k= 項目引数のセット
	vector<kgstr_t> vs = _args.toStringVector("k=",false);

	vector<kgstr_t> vss = _args.toStringVector("s=",false);
	bool seqflg = _args.toBool("-q");
	if(_nfn_i) { seqflg = true; }

	if(!seqflg&&vss.empty()){
		throw kgError("parameter s= is mandatory without -q,-nfn");
	}

	if(!seqflg && (!vs.empty()||!vss.empty()) ){ 
		vector<kgstr_t> vsk	= vs;
		vsk.insert(vsk.end(),vss.begin(),vss.end());
		sortingRun(&_iFile,vsk);
	}

  _oFile.setPrecision(_precision);

	_fField.set(vvs, &_iFile, _fldByNum);
	_kField.set(vs,  &_iFile, _fldByNum);

}
// -----------------------------------------------------------------------------
// パラメータチェック＆入出力ファイルオープン
// -----------------------------------------------------------------------------
void kgAccum::setArgs(void)
{
	_args.paramcheck(_paralist,_paraflg);

	_iFile.open(_args.toString("i=",false), _env,_nfn_i);
	_oFile.open(_args.toString("o=",false), _env,_nfn_o);
	setArgsMain();

}
void kgAccum::setArgs(int inum,int *i_p,int onum ,int *o_p)
{

	int iopencnt = 0;
	int oopencnt = 0;
	try{
		_args.paramcheck(_paralist,_paraflg);

		if(inum>1 || onum>1){ throw kgError("no match IO");}

		// 入出力ファイルオープン
		if(inum==1 && *i_p>0){ _iFile.popen(*i_p, _env,_nfn_i);}
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
int kgAccum::runMain()
{

	// 入力ファイルにkey項目番号をセットする．
	_iFile.setKey(_kField.getNum());

	// 項目名の出力
  _oFile.writeFldName(_iFile,_fField, true);

	// 集計用変数領域確保＆初期化
	vector<kgVal> val(_fField.size() ,kgVal('N'));
	for(unsigned int i=0; i<val.size(); i++){ val[i].r(0);}

	// データ集計＆出力
	while(_iFile.read()!=EOF){

		// keybreakしたらデータ初期化
		if( _iFile.keybreak() ){
			if((_iFile.status() & kgCSV::End )) break;
			for(unsigned int i=0; i<val.size(); i++){ val[i].r(0);}
		}
		
		// 通常処理:元データ＋累計出力 
		// :NULLのときはNULLを出力する。
		_oFile.writeFld(_iFile.fldSize(),_iFile.getNewFld(),false);
		for(vector<kgstr_t>::size_type i=0; i<_fField.size(); i++){
			char * strtmp =_iFile.getNewVal(_fField.num(i));
			if( *strtmp=='\0'){
				if(i<_fField.size()-1) _oFile.writeDlm();
				else                   _oFile.writeEol();			
				if(_assertNullIN) { _existNullIN  = true;}
				if(_assertNullOUT){ _existNullOUT = true;}
			}
			else{
				kgA2Fadd(strtmp,val[i]);
				if(i<_fField.size()-1) _oFile.writeVal(val[i],false);
				else                   _oFile.writeVal(val[i],true);			
			}
		}
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
int kgAccum::run(void) 
{
	try{
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
    ((kgAccum*)arg)->runErrEnd();
}

int kgAccum::run(int inum,int *i_p,int onum, int* o_p,string &msg)
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
