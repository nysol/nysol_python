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
// kgvdelim.cpp ベクトル区切り文字の変更クラス
// =============================================================================
#include <cstdio>
#include <sstream>
#include <vector>
#include <kgsplit.h>
#include <kgError.h>
#include <kgMethod.h>
#include <kgConfig.h>

using namespace std;
using namespace kglib;
using namespace kgmod;

// -----------------------------------------------------------------------------
// コンストラクタ(モジュール名，バージョン登録)
// -----------------------------------------------------------------------------
const char * kgSplit::_ipara[] = {"i",""};
const char * kgSplit::_opara[] = {"o",""};


kgSplit::kgSplit(void)
{
	_name    = "kgsplit";
	_version = "###VERSION###";
	_paralist = "i=,o=,f=,delim=,a=,-r";
	_paraflg = kgArgs::COMMON|kgArgs::IODIFF|kgArgs::NULL_IN|kgArgs::NULL_OUT;

	#include <help/en/kgsplitHelp.h>
	_titleL = _title;
	_docL   = _doc;
	#ifdef JPN_FORMAT
		#include <help/jp/kgsplitHelp.h>
	#endif

}
// -----------------------------------------------------------------------------
// パラメータセット＆入出力ファイルオープン
// -----------------------------------------------------------------------------
void kgSplit::setArgsMain(void)
{
	_iFile.read_header();

	// f= 項目引数のセット
	vector<kgstr_t>  vs = _args.toStringVector("f=",true);
	if( vs.size()!=1){
		ostringstream ss;
		ss << "f= must take just one item : f= size:" << vs.size() ;
		throw kgError(ss.str());
	}
	_fField.set(vs, &_iFile,_fldByNum); 

	_aStr = _args.toStringVector("a=",true);

	_remove = _args.toBool("-r");
	
	kgstr_t s_d = _args.toString("delim=",false);
	if(s_d.empty()){	
		_delim=' ';
	}else if(s_d.size()!=1){
		ostringstream ss;
		ss << "delim= takes 1 byte charactor (" << s_d << ")";
		throw kgError(ss.str());
	}else{
		_delim=*(s_d.c_str());
	}

}

// -----------------------------------------------------------------------------
// パラメータセット＆入出力ファイルオープン
// -----------------------------------------------------------------------------
void kgSplit::setArgs(void)
{
	// パラメータチェック
	_args.paramcheck(_paralist,_paraflg);

	// 入出力ファイルオープン
	_iFile.open(_args.toString("i=",false),_env,_nfn_i);
	_oFile.open(_args.toString("o=",false),_env,_nfn_o);

	setArgsMain();


}
// -----------------------------------------------------------------------------
// パラメータセット＆入出力ファイルオープン
// -----------------------------------------------------------------------------
void kgSplit::setArgs(int inum,int *i_p,int onum ,int *o_p)
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
int kgSplit::runMain(void)
{

	char dmy[1];
	dmy[0] = '\0';
	
	// 項目名出力
	if (_remove){
		vector<kgstr_t> outFldName;
		for(size_t i=0; i<_iFile.fldSize(); i++){
			if( _fField.flg(i) == -1  ){
				outFldName.push_back(_iFile.fldName(i,true));
			}
		}
		for(vector<kgstr_t>::size_type i=0; i<_aStr.size(); i++){
			outFldName.push_back(_aStr.at(i));
		}
		if(!_nfn_o){ _oFile.writeFldNameCHK(outFldName); }
	}
	else{	
		_oFile.writeFldName(_iFile, _aStr);
	}

	kgAutoPtr2<char*> _o_stock_ap;
	_o_stock_ap.set( new char*[_aStr.size()] );
	char ** opp = _o_stock_ap.get();
	while(EOF != _iFile.read() ){
		strcpy(_outstr,_iFile.getVal(_fField.num(0)));
		if( *_outstr=='\0'){
			if( _assertNullIN ){ _existNullIN  = true;} 
			if( _assertNullOUT){ _existNullOUT = true;}
		}
		vector<char*> eachItem = splitToken(_outstr,_delim);
		for( size_t i=0 ;  i < _aStr.size() ;i++){
			if( i < eachItem.size() ){ * (opp+i) = 	eachItem[i] ;}
			else { 	
				* (opp+i) = dmy;
				if( _assertNullOUT){ _existNullOUT = true;}
			} 
		}
		if (_remove){
			for(size_t i=0; i<_iFile.fldSize(); i++){
				if( _fField.flg(i) == -1  ){ _oFile.writeStr( _iFile.getVal(i), false ); }
			}
			for(size_t i=0; i<_aStr.size()-1; i++){
				_oFile.writeStr( *(opp+i), false );
			}
				_oFile.writeStr( *(opp+_aStr.size()-1), true );
		}
		else{
			_oFile.writeFld(_iFile.getFld(),_iFile.fldSize(),opp,_aStr.size());
		}
	}

	// 終了処理
	_iFile.close();
	_oFile.close();

	return 0;

}
// -----------------------------------------------------------------------------
// 実行 
// -----------------------------------------------------------------------------
int kgSplit::run(void) 
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
    ((kgSplit*)arg)->runErrEnd();
}

int kgSplit::run(int inum,int *i_p,int onum, int* o_p,string &msg)
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

