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
// kgfldname.cpp 項目名の変更
// =============================================================================
#include <cstdio>
#include <kgfldname.h>
#include <kgError.h>
#include <kgConfig.h>

using namespace std;
using namespace kglib;
using namespace kgmod;

// -----------------------------------------------------------------------------
// コンストラクタ(モジュール名，バージョン登録)
// -----------------------------------------------------------------------------
const char * kgFldname::_ipara[] = {"i",""};
const char * kgFldname::_opara[] = {"o",""};

kgFldname::kgFldname(void)
{
	_name    = "kgfldname";
	_version = "###VERSION###";

	_paralist = "i=,o=,f=,n=,-nfni,-q";
	_paraflg = kgArgs::COMMON|kgArgs::IODIFF;

	#include <help/en/kgfldnameHelp.h>
	_titleL = _title;
	_docL   = _doc;
	#ifdef JPN_FORMAT
		#include <help/jp/kgfldnameHelp.h>
	#endif
	
}
// -----------------------------------------------------------------------------
// パラメータセット
// -----------------------------------------------------------------------------
void kgFldname::setArgsMain(void)
{

	_iFile.read_header();

	_rmsinfo = _args.toBool("-q");

	// 変更項目名セット f= or n=
	if(! _nfn_o){
		vector< vector<kgstr_t> > vvs = _args.toStringVecVec("f=",':',2,false);
		vector<kgstr_t> vssn = _args.toStringVector("n=",false);
		_fField.set(vvs, &_iFile, _fldByNum);
		
		if(_fField.size()==0 &&  vssn.empty() && _nfn_i){
			throw kgError("`-nfni' cannot be specified with `f='");		
		}
		if(!vssn.empty() && vssn.size() != _iFile.fldSize() ){
			if (!_nfn_i || !_iFile.end() ){
				throw kgError("the number of fields is different on `n=' and input data.");
			}
		}
		if(!_fField.size() && vssn.empty()&& !_rmsinfo){
			throw kgError("`f=' or `n=' or `-nfno' or '-q' must be specified.");
		}
		//ヘッダ情報作成
		if(vssn.size()>0){//n=
			if(_rmsinfo){ newFldName_ = vssn;}
			else { 
				for(size_t i=0;i<vssn.size();i++){
					newFldName_.push_back(vssn.at(i));
					newFldName_.at(i).append(_iFile.sortParaStr(i));
				}
			}
		}
		else{//f=
			kgstr_t oName;
			for(size_t i=0; i<_iFile.fldSize() ; i++){
				int flg = _fField.flg(i);
				if( flg==-1 ){ oName = _iFile.fldName(i,!_rmsinfo);}
				else{
					oName = _fField.attr(flg);
					if( oName.empty()){ oName=_iFile.fldName(i,!_rmsinfo);}
					else{
						if(!_rmsinfo){ oName.append(_iFile.sortParaStr(i));}
					}
				}
				newFldName_.push_back( oName );
			}
		}
	}

}
// -----------------------------------------------------------------------------
// パラメータセット＆入出力ファイルオープン
// -----------------------------------------------------------------------------
void kgFldname::setArgs(void)
{
	// パラメータチェック
	_args.paramcheck(_paralist,_paraflg);

	// -nfniを指定した場合、-xも指定されていることにする
	bool nfniflg = _args.toBool("-nfni");
	if(nfniflg){ 
		_nfn_i =nfniflg;
		_fldByNum = true;
	}

	// 入出力ファイルオープン
	_iFile.open(_args.toString("i=",false), _env,_nfn_i);
  _oFile.open(_args.toString("o=",false), _env,_nfn_o);

	setArgsMain();

}
// -----------------------------------------------------------------------------
// パラメータセット＆入出力ファイルオープン
// -----------------------------------------------------------------------------
void kgFldname::setArgs(int inum,int *i_p,int onum ,int *o_p)
{
	int iopencnt = 0;
	int oopencnt = 0;
	try{

		_args.paramcheck(_paralist,_paraflg);

		// -nfniを指定した場合、-xも指定されていることにする
		bool nfniflg = _args.toBool("-nfni");
		if(nfniflg){ 
			_nfn_i =nfniflg;
			_fldByNum = true;
		}

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
int kgFldname::runMain(void)
{

	//項目名出力
 	_oFile.writeFldName(newFldName_);

	// データ出力
	while(EOF != _iFile.read()){
  	_oFile.writeRec(_iFile.getRec());
	}

	// 終了処理
	_iFile.close();
	_oFile.close();

	return 0;

}
// -----------------------------------------------------------------------------
// 実行 
// -----------------------------------------------------------------------------
int kgFldname::run(void) 
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
    ((kgFldname*)arg)->runErrEnd();
}

int kgFldname::run(int inum,int *i_p,int onum, int* o_p,string &msg)
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
