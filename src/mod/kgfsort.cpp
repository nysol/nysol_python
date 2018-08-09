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
// kgfsort.h 項目ソートクラス
// =============================================================================
#include <cstdio>
#include <sstream>
#include <vector>
#include <kgfsort.h>
#include <kgError.h>
#include <kgMethod.h>
#include <kgConfig.h>

using namespace std;
using namespace kglib;
using namespace kgmod;


namespace kgfsort_local {//////////////////////////////////////////////////

	// 文字列昇順
	inline int cmpStrAsc(unsigned char* a, unsigned char* b)
	{
		while(true){
		       if(*a < *b) return  1;
			else if(*a > *b) return -1;
			if(*a=='\0') break; // *a==*b=='\0'
			a++;b++;
		}
		return 0;
	}
	// 文字列降順
	inline int cmpStrDec(unsigned char* a, unsigned char* b)
	{
		while(true){
		       if(*a < *b) return -1;
			else if(*a > *b) return  1;
			if(*a=='\0') break; // *a==*b=='\0'
			a++;b++;
		}
		return 0;
	}
	// 数値昇順
	inline int cmpNumAsc(char* a, char* b)
	{
		if(*a=='\0'){
			if(*b=='\0') return 0;
			else         return 1;
		}else{
			if(*b=='\0') return -1;
			double fa=atof(a);
			double fb=atof(b);
		       if(fa < fb) return  1;
			else if(fa > fb) return -1;
			else             return  0;
		}
		return 0;
	}
	// 数値降順
	inline int cmpNumDec(char* a, char* b)
	{
		if(*a=='\0'){
			if(*b=='\0') return 0;
			else         return -1;
		}else{
			if(*b=='\0') return 1;
			double fa=atof(a);
			double fb=atof(b);
		       if(fa < fb) return -1;
			else if(fa > fb) return  1;
			else             return  0;
		}
		return 0;
	}
	// 比較クラス
	struct itmComp 
	{
		int type;//0:文字ソート,1:数字ソート,2:文字逆ソート,3:数字逆ソート
		itmComp(void){type=0;}
		itmComp(int x){type = x;}

 		bool operator()(char* v1, char* v2) {
 			int retVal=0;
			switch( type ){
				case 0: // 文字昇順
				retVal=cmpStrAsc((unsigned char*)v1, (unsigned char*)v2);
				break;
			case 1: // 数字昇順
				retVal=cmpNumAsc(v1, v2);
				break;
			case 2: // 文字降順
				retVal=cmpStrDec((unsigned char*)v1, (unsigned char*)v2);
				break;
			case 3: // 数字降順
				retVal=cmpNumDec(v1, v2);
				break;
	 		}
	   	if( retVal>0 ){ return true;}
  	 	else       		{ return false;}
  	 	return false;
  	}

};

} //////////////////////////////////////////////////////////////////////////////
// -----------------------------------------------------------------------------
// コンストラクタ(モジュール名，バージョン登録)
// -----------------------------------------------------------------------------
const char * kgFsort::_ipara[] = {"i",""};
const char * kgFsort::_opara[] = {"o",""};

kgFsort::kgFsort(void)
{
	_name    = "kgfsort";
	_version = "###VERSION###";

	_paralist = "i=,o=,f=,-n,-r";
	_paraflg = kgArgs::COMMON|kgArgs::IODIFF|kgArgs::NULL_IN;

	#include <help/en/kgfsortHelp.h>
	_titleL = _title;
	_docL   = _doc;
	#ifdef JPN_FORMAT
		#include <help/jp/kgfsortHelp.h>
	#endif

}
// -----------------------------------------------------------------------------
// パラメータセット
// -----------------------------------------------------------------------------
void kgFsort::setArgsMain(void)
{
	_iFile.read_header();

	// f= 項目引数のセット
	vector< vector<kgstr_t> > vvs = _args.toStringVecVec("f=",':',2,true);
	_fField.set(vvs, &_iFile, _fldByNum);

	// -r 逆順フラグ
	_reverse = _args.toBool("-r");

	// -n 数値ソートフラグ
	_numsort = _args.toBool("-n");

}

// -----------------------------------------------------------------------------
// 入出力ファイルオープン
// -----------------------------------------------------------------------------
void kgFsort::setArgs(void)
{
	// パラメータチェック
	_args.paramcheck(_paralist,_paraflg);

	// 入出力ファイルオープン
	_iFile.open(_args.toString("i=",false),_env,_nfn_i);
	_oFile.open(_args.toString("o=",false),_env,_nfn_o);

	setArgsMain();

}
// -----------------------------------------------------------------------------
// 入出力ファイルオープン
// -----------------------------------------------------------------------------
void kgFsort::setArgs(int inum,int *i_p,int onum ,int *o_p)
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
//  val : vector<kgVal>バージョン
// -----------------------------------------------------------------------------
void kgFsort::writeFld(char** fld, const vector<int>* flg, vector<char*>& val)
{	
	size_t i;
	int num;
	for(i=0; i<flg->size()-1; i++){ 
		num=flg->at(i);
		if(num == -1) { _oFile.writeStr( *(fld+i)   , false );}
		else          { _oFile.writeStr(val.at(num) , false );}
	}
		num=flg->at(i);
		if(num == -1) { _oFile.writeStr( *(fld+i)   , true );}
		else          { _oFile.writeStr(val.at(num) , true );}
}


// -----------------------------------------------------------------------------
// 実行
// -----------------------------------------------------------------------------
int kgFsort::runMain(void)
{
	// 項目名出力
  _oFile.writeFldName(_fField, true);

	// ソート情報セット
	int flg=0;
	if(_numsort) { flg |= 1;}
	if(_reverse) { flg |= 2;}
	kgfsort_local::itmComp icomp(flg);
	

	while(EOF != _iFile.read() ){
		vector<char*> eachItem;
		for(size_t i=0; i<_fField.size(); i++){
			if(_assertNullIN && *_iFile.getVal(_fField.num(i))=='\0'){  _existNullIN  = true; }
			eachItem.push_back(_iFile.getVal(_fField.num(i)));
		}
		sort(eachItem.begin(),eachItem.end(),icomp);
		writeFld(_iFile.getFld(),_fField.getFlg_p(),eachItem);
	}

	// 終了処理(メッセージ出力,thread pipe終了通知)
	_iFile.close();
	_oFile.close();

	return 0;

}
// -----------------------------------------------------------------------------
// 実行 
// -----------------------------------------------------------------------------
int kgFsort::run(void) 
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
    ((kgFsort*)arg)->runErrEnd();
}

int kgFsort::run(int inum,int *i_p,int onum, int* o_p,string &msg)
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

