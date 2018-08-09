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
// kgtonull.h NULL値の置換クラス
// =============================================================================
#include <cstdio>
#include <string>
#include <cstring>
#include <vector>
#include <kgtonull.h>
#include <kgError.h>
#include <kgConfig.h>

using namespace std;
using namespace kglib;
using namespace kgmod;

// -----------------------------------------------------------------------------
//	文字列比較(当てはまればtrue)vstrにある文字列のどれかに一致すればtrue
// -----------------------------------------------------------------------------
namespace 
{
	// ワイド文字による部分文字列比較
	bool strCompSub(char* str,vector<wstring>& vstr){
		for(vector<wstring>::size_type i=0;i<vstr.size();i++){
			wstring ws=toWcs(str);
			if(ws.find(vstr[i])!=wstring::npos) {return true;}
		}
		return false;
	}
	// 通常の部分文字列比較
	bool strCompSub(char* str,vector<string>& vstr){
		for(vector<string>::size_type i=0;i<vstr.size();i++){
			if(NULL!=strstr(str,vstr[i].c_str())) {return true;}
		}
		return false;
	}
	// 完全マッチ比較
	bool strComp(char* str,vector<string>& vstr){
		for(vector<string>::size_type i=0;i<vstr.size();i++){
			if(!strcmp(str,vstr[i].c_str())) {return true;}
		}
		return false;
	}
}

// -----------------------------------------------------------------------------
// コンストラクタ(モジュール名，バージョン登録)
// -----------------------------------------------------------------------------
const char * kgTonull::_ipara[] = {"i",""};
const char * kgTonull::_opara[] = {"o",""};

kgTonull::kgTonull(void)
{
	_name    = "kgtonull";
	_version = "###VERSION###";

	_paralist = "i=,o=,f=,v=,-sub,-W";
	_paraflg = kgArgs::COMMON|kgArgs::IODIFF|kgArgs::NULL_IN;

	#include <help/en/kgtonullHelp.h>
	_titleL = _title;
	_docL   = _doc;
	#ifdef JPN_FORMAT
		#include <help/jp/kgtonullHelp.h>
	#endif

}
// -----------------------------------------------------------------------------
// パラメータセット＆入出力ファイルオープン
// -----------------------------------------------------------------------------
void kgTonull::setArgsMain(void)
{
	_iFile.read_header();

	// f= 項目引数のセット
	vector<kgstr_t>  vs_f = _args.toStringVector("f=",true);
	_fField.set(vs_f, &_iFile,_fldByNum);

	// -sub 部分マッチフラグ
	_substr     = _args.toBool("-sub");
	_widechr    = _args.toBool("-W");

	// v= 項目引数のセット
	_vField = _args.toStringVector("v=",true);
	for(vector<kgstr_t>::size_type i=0;i<_vField.size();i++){
		if(_substr && _widechr){
			_vFieldw.push_back(toWcs(_vField[i]));
		}
	}

}

// -----------------------------------------------------------------------------
// パラメータセット＆入出力ファイルオープン
// -----------------------------------------------------------------------------
void kgTonull::setArgs(void)
{
	// パラメータチェック
	_args.paramcheck(_paralist,_paraflg);
	// ファイルオープン
	_iFile.open(_args.toString("i=",false),_env,_nfn_i);
	_oFile.open(_args.toString("o=",false),_env,_nfn_o);

	setArgsMain();

}
// -----------------------------------------------------------------------------
// パラメータセット＆入出力ファイルオープン
// -----------------------------------------------------------------------------
void kgTonull::setArgs(int inum,int *i_p,int onum ,int *o_p)
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
int kgTonull::runMain(void)
{

	// 項目名出力
	_oFile.writeFldName(_iFile);

	while( EOF != _iFile.read() ){
		const vector<int>* flg=_fField.getFlg_p();
		for(vector<int>::size_type i=0; i<flg->size(); i++){

			bool eol= (i==flg->size()-1);	// 改行出力フラグ
			int   num = flg->at(i);       // f=で指定されたかどうか
			char* str = _iFile.getVal(i); // 項目の値

			// f=で指定された項目以外の場合はそのまま出力
			if(num == -1){
				_oFile.writeStr(str, eol);
			}else{
				// f=で指定された項目の場合
				if(_assertNullIN && *str=='\0') { _existNullIN  = true;}
				if(_substr){ // 部分マッチの比較
					if(_widechr){
						if( strCompSub(str, _vFieldw) ) _oFile.writeStr("" ,eol);
						else                            _oFile.writeStr(str,eol);
					}else{
						if( strCompSub(str, _vField ) ) _oFile.writeStr("" ,eol);
						else                            _oFile.writeStr(str,eol);
					}
				}else{// 完全一致の比較
					if( strComp(str, _vField) ) _oFile.writeStr("" ,eol);
					else                        _oFile.writeStr(str,eol);
				}
			}
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
int kgTonull::run(void) 
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
    ((kgTonull*)arg)->runErrEnd();
}


int kgTonull::run(int inum,int *i_p,int onum, int* o_p,string &msg)
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

