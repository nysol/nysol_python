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
// kgpaste.h 参照ファイル項目の行番号マッチング結合クラス
// =============================================================================
#include <cstdio>
#include <kgpaste.h>
#include <kgError.h>
#include <kgConfig.h>

using namespace std;
using namespace kglib;
using namespace kgmod;

// -----------------------------------------------------------------------------
// コンストラクタ(モジュール名，バージョン登録)
// -----------------------------------------------------------------------------
const char * kgPaste::_ipara[] = {"i","m",""};
const char * kgPaste::_opara[] = {"o",""};

kgPaste::kgPaste(void)
{
	_name    = "kgpaste";
	_version = "###VERSION###";

	_paralist = "f=,i=,o=,m=,-n,-N";
	_paraflg = kgArgs::COMMON|kgArgs::IODIFF|kgArgs::NULL_IN|kgArgs::NULL_OUT;

	#include <help/en/kgpasteHelp.h>
	_titleL = _title;
	_docL   = _doc;
	#ifdef JPN_FORMAT
		#include <help/jp/kgpasteHelp.h>
	#endif
	
}
// -----------------------------------------------------------------------------
// パラメータセット＆入出力ファイルオープン
// -----------------------------------------------------------------------------
void kgPaste::setArgsMain(void)
{
	_iFile.read_header();
	_mFile.read_header();

	// f= 項目引数のセット指定が無かった場合はk=の項目以外をセットする
	vector< vector<kgstr_t> >  vvs = _args.toStringVecVec("f=",':',2,false);
	if(vvs[0].empty()){
		for(size_t i=0 ; i< _mFile.fldSize();i++){
			vvs[0].push_back(toString(i));				
			vvs[1].push_back("");				
		}
		_fField.set(vvs, &_mFile, true);
	}else{
		_fField.set(vvs, &_mFile, _fldByNum);
	}
	// -n,-N アウタージョインフラグ
	_i_outer = _args.toBool("-n");
	_m_outer = _args.toBool("-N");

}

// -----------------------------------------------------------------------------
// パラメータセット＆入出力ファイルオープン
// -----------------------------------------------------------------------------
void kgPaste::setArgs(void)
{
	// パラメータチェック
	_args.paramcheck(_paralist,_paraflg);

	// 入出力ファイルオープン
	kgstr_t ifile = _args.toString("i=",false);
	kgstr_t mfile = _args.toString("m=",false);
	if(ifile.empty() && mfile.empty()){
		throw kgError("Either i= or m= must be specified.");
	}
	_iFile.open(ifile,_env,_nfn_i);
	_oFile.open(_args.toString("o=",false),_env,_nfn_o);
	_mFile.open(mfile,_env,_nfn_i);

	setArgsMain();
}
// -----------------------------------------------------------------------------
// パラメータセット＆入出力ファイルオープン
// -----------------------------------------------------------------------------
void kgPaste::setArgs(int inum,int *i_p,int onum, int* o_p)
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
// -----------------------------------------------------------------------------
// 実行
// -----------------------------------------------------------------------------
int kgPaste::runMain(void)
{

	// 項目名出力
	_oFile.writeFldName(_iFile,_fField,true);

	//比較結果用フラグ
	int cmpflg=0;

	//追加項目no
  vector<int> fField =_fField.getNum();

	//入力、参照ファイルの最終チェック(1なら終了)
	bool traEnd=false;
	bool mstEnd=false;

	// データ出力(入力ファイルあるいは参照ファイルどちらか最後まで読み込むまで)
	while(true){
		// 読み込み
		if(_iFile.read() == EOF) traEnd=true;
		if(_mFile.read() == EOF) mstEnd=true;

		// traとmstのキーの比較 (終了しているかどうか)
		     if(traEnd && mstEnd) break;
		else if(traEnd          ) cmpflg= 1;
		else if(          mstEnd) cmpflg=-1;
		else                      cmpflg= 0;

		// tra,mst両者とも書き出す
		if(cmpflg==0){
			for( size_t i=0; i<_iFile.fldSize(); i++){
				_oFile.writeStr(_iFile.getVal(i),false);
			}
			for(vector<int>::size_type i=0; i<fField.size(); i++){
				bool ret=false;
				if(i==fField.size()-1) ret=true;
				if(_assertNullIN && *_mFile.getVal(fField.at(i))=='\0' ){ _existNullIN  = true; }
				_oFile.writeStr(_mFile.getVal(fField.at(i)),ret);
			}
		// mstのみ書き出す
		}else if( _m_outer && cmpflg>0 ){
			if(_assertNullOUT){ _existNullOUT = true;}
			for( size_t i=0; i<_iFile.fldSize(); i++){
				_oFile.writeStr("",false);
			}
			for(vector<int>::size_type i=0; i<fField.size(); i++){
				bool ret=false;
				if(i==fField.size()-1) ret=true;
				_oFile.writeStr(_mFile.getVal(fField.at(i)),ret);
			}
		// traのみ書き出す
		}else if( _i_outer && cmpflg<0 ){
			if(_assertNullOUT){ _existNullOUT = true;}
			for( size_t i=0; i<_iFile.fldSize(); i++){
				_oFile.writeStr(_iFile.getVal(i),false);
			}
			for(vector<int>::size_type i=0; i<fField.size(); i++){
				bool ret=false;
				if(i==fField.size()-1) ret=true;
				_oFile.writeStr("",ret);
			}
		}
	}
	// 終了処理
	_iFile.close();
	_mFile.close();
	_oFile.close();
	return 0;

}
// -----------------------------------------------------------------------------
// 実行 
// -----------------------------------------------------------------------------
int kgPaste::run(void) 
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
    ((kgPaste*)arg)->runErrEnd();
}

int kgPaste::run(int inum,int *i_p,int onum, int* o_p,string &msg)
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
