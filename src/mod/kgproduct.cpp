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
// kgproduct.h 参照ファイル項目の直積結合クラス
// =============================================================================
#include <cstdio>
#include <vector>
#include <kgproduct.h>
#include <kgError.h>
#include <kgMethod.h>
#include <kgTempfile.h>

using namespace std;
using namespace kglib;
using namespace kgmod;

// -----------------------------------------------------------------------------
// コンストラクタ(モジュール名，バージョン登録)
// -----------------------------------------------------------------------------
const char * kgProduct::_ipara[] = {"i","m",""};
const char * kgProduct::_opara[] = {"o",""};

kgProduct::kgProduct(void)
{
	_name    = "kgproduct";
	_version = "###VERSION###";

	_paralist = "f=,i=,o=,m=,bufcount=";
	_paraflg = kgArgs::COMMON|kgArgs::IODIFF|kgArgs::NULL_IN;

	#include <help/en/kgproductHelp.h>
	_titleL = _title;
	_docL   = _doc;
	#ifdef JPN_FORMAT
		#include <help/jp/kgproductHelp.h>
	#endif

}
// -----------------------------------------------------------------------------
// 入出力ファイルオープン
// -----------------------------------------------------------------------------
void kgProduct::setArgsMain(void)
{

	kgstr_t s=_args.toString("bufcount=",false);
	int bcnt = 10;
	if(!s.empty()){ 
		bcnt = atoi(s.c_str());
		if(bcnt<=0){ bcnt=1;}
	}
	_mFile.setbufsize(bcnt);
	_iFile.read_header();
	_mFile.read_header();

	// f= 項目引数のセット
	// 項目引数の指定が無かった場合は全項目をセットする
	vector< vector<kgstr_t> > vvs = _args.toStringVecVec("f=",':',2,false);
	if(vvs[0].empty()){
		for(size_t i=0 ; i< _mFile.fldSize();i++){
			vvs[0].push_back(toString(i));				
			vvs[1].push_back("");				
		}
		_fField.set(vvs, &_mFile,true);		
	}
	else{
		_fField.set(vvs, &_mFile,_fldByNum);
	}

}

// -----------------------------------------------------------------------------
// 入出力ファイルオープン
// -----------------------------------------------------------------------------
void kgProduct::setArgs(void)
{
	// パラメータチェック
	_args.paramcheck(_paralist,_paraflg);

	// 入出力ファイルオープン&バッファ設定
	kgstr_t ifile = _args.toString("i=",false);
	kgstr_t mfile = _args.toString("m=",false);
	_oFile.open(_args.toString("o=",false),_env,_nfn_o);
	if(ifile.empty() && mfile.empty()){
		throw kgError("Either i= or m= must be specified." );
	}
	_iFile.open(ifile,_env,_nfn_i);
	_mFile.open(mfile,_env,_nfn_i);
	setArgsMain();

}

// -----------------------------------------------------------------------------
// 入出力ファイルオープン
// -----------------------------------------------------------------------------
void kgProduct::setArgs(int inum,int *i_p,int onum, int* o_p)
{
	int iopencnt = 0;
	int oopencnt = 0;
	try{
		// パラメータチェック
		_args.paramcheck(_paralist,_paraflg);

		if(inum>2 && onum>1){ throw kgError("no match IO"); }

		// 入出力ファイルオープン
		kgstr_t ifile = _args.toString("i=",false);
		kgstr_t mfile = _args.toString("m=",false);

		int i_p_t = -1;
		int m_p_t = -1;
		if(inum>0){ i_p_t = *i_p;     }
		if(inum>1){ m_p_t = *(i_p+1); }

		if((ifile.empty()&&i_p_t<=0) && ( mfile.empty()&&m_p_t<=0)){
			throw kgError("Either i= or m= must be specified.");
		}

		// 入出力ファイルオープン
		if(i_p_t>0){ _iFile.popen(i_p_t, _env,_nfn_i); }
		else if( ifile.empty()){ 
			throw kgError("i= is necessary");
		}
		else       { _iFile.open(ifile, _env,_nfn_i);}
		iopencnt++;

		if(m_p_t>0){ _mFile.popen(m_p_t, _env,_nfn_i); }
		else if( mfile.empty()){ 
			throw kgError("m= is necessary");
		}
		else       { _mFile.open(mfile, _env,_nfn_i);}
		iopencnt++;

		if(onum == 1 && *o_p > 0){ _oFile.popen(*o_p, _env,_nfn_o);}
		else{ _oFile.open(_args.toString("o=",true), _env,_nfn_o);}
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

int kgProduct::runMain(void){

	//参照ファイルの出力項目セット
	vector<int> fField =_fField.getNum();

	// 項目名出力
	_oFile.writeFldName(_iFile,_fField,true);
	//参照ファイルのデータをセット

	if ( _mFile.blkset() != EOF){
		// データ出力(入力ファイルに対して参照ファイルの全データをマッチして出力)
		// 参照ファイルは入力ファイルを読み込むたびに位置を先頭行へセットしなおす
		while( EOF != _iFile.read() ){
			while(_mFile.blkread() != EOF){
				if(_assertNullIN && _mFile.isNull(fField) ){ _existNullIN  = true; }
				_oFile.writeFld(_iFile.getFld(),_iFile.fldSize(),_mFile.getBlkFld(),&fField);				
			}
			_mFile.seekBlkTop();
		}
	}
	
	// 終了処理(メッセージ出力,thread pipe終了通知)
	_iFile.close();
	_mFile.close();
	_oFile.close();

	return 0;

}
// -----------------------------------------------------------------------------
// 実行 
// -----------------------------------------------------------------------------
int kgProduct::run(void) 
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
    ((kgProduct*)arg)->runErrEnd();
}

int kgProduct::run(int inum,int *i_p,int onum, int* o_p,string &msg)
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
