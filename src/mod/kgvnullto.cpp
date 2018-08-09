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
// kgvcat.cpp ベクトルサイズの計算クラス
// =============================================================================
#include <cstdio>
#include <sstream>
#include <string>
#include <vector>
#include <list>
#include <set>
#include <csignal>
#include <kgvnullto.h>
#include <kgError.h>
#include <kgMethod.h>
#include <kgConfig.h>

using namespace std;
using namespace kglib;
using namespace kgmod;

// -----------------------------------------------------------------------------
// コンストラクタ(モジュール名，バージョン登録)
// -----------------------------------------------------------------------------
const char * kgVnullto::_ipara[] = {"i",""};
const char * kgVnullto::_opara[] = {"o",""};

kgVnullto::kgVnullto(void)
{
	_name    = "kgvnullto";
	_version = "###VERSION###";

	_paralist = "i=,o=,vf=,delim=,O=,v=,-A,-p";
	_paraflg = kgArgs::COMMON|kgArgs::IODIFF|kgArgs::NULL_IN|kgArgs::NULL_OUT;

	#include <help/en/kgvnulltoHelp.h>
	_titleL = _title;
	_docL   = _doc;
	#ifdef JPN_FORMAT
		#include <help/jp/kgvnulltoHelp.h>
	#endif
	
}

// -----------------------------------------------------------------------------
// 入出力ファイルオープン
// -----------------------------------------------------------------------------
void kgVnullto::setArgsMain(void)
{
	_iFile.read_header();

	// vf= 項目引数のセット
	vector< vector<kgstr_t> >  vs = _args.toStringVecVec("vf=",':',2,true);
	_vfField.set(vs, &_iFile,_fldByNum);

	// delim指定
	kgstr_t s_d = _args.toString("delim=",false);
	if(s_d.empty()){	
		_delim=' ';
	}else if(s_d.size()!=1){
		ostringstream ss;
		ss << "delim= takes 1 byte charactor (" <<  s_d << ")";
		throw kgError(ss.str());
	}else{
		_delim=*(s_d.c_str());
	}	
	// -A（追加）,-p(前行文字列での置換) フラグセット
	_add_flg 		= _args.toBool("-A");
	_prv_flg 		= _args.toBool("-p");

	// v= 置換文字列
	_vField = _args.toString("v=",false);
	if(_vField.size()==0&&!_prv_flg){
		throw kgError("parameter v= or -p is mandatory");
	}

	// O= NULL値以外の置換文字列
	_OField = _args.toString("O=",false);
	_exp_nul = !_OField.empty();

}

// -----------------------------------------------------------------------------
// 入出力ファイルオープン
// -----------------------------------------------------------------------------
void kgVnullto::setArgs(void)
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
void kgVnullto::setArgs(int inum,int *i_p,int onum, int* o_p)
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

void kgVnullto::output_n(char *str,bool eol)
{
	char buf[KG_MAX_STR_LEN];
	char delimstr[2];
	delimstr[0] =_delim;
	delimstr[1] ='\0';
	*buf = '\0';
	bool d_out=false;
	
	vector<char*> eachItem = splitToken( str ,_delim);
	char *prv;
	int len=0;
	if(eachItem.size()>0){ prv =eachItem[0];}

	for(vector<char*>::iterator j=eachItem.begin(); j!=eachItem.end(); j++){

		char *p = *j;
		if(*p=='\0'){ 
			if(_prv_flg) { p = const_cast<char*>(prv); }
			else				 { p = const_cast<char*>(_vField.c_str()); }
		}
		else if(_exp_nul){
			p = const_cast<char*>(_OField.c_str());
		}
		if(_prv_flg && **j!='\0'){ prv = *j ;}


		if(d_out) { len += (1+strlen(p));}
		else			{ len += strlen(p);}
		
		if(len>=KG_MAX_STR_LEN){ 
			throw kgError("field length exceeded KG_MAX_STR_LEN");
		}
			
		if(d_out) { strcat(buf,delimstr); }
		strcat(buf,p);
		d_out = true;
	}
	if(_assertNullOUT && *buf=='\0'){ _existNullOUT = true;}
	_oFile.writeStr(buf,eol);	
} 

// -----------------------------------------------------------------------------
// 実行
// -----------------------------------------------------------------------------
int kgVnullto::runMain(void)
{

	//出力項目名出力 追加 or 置換
	if(_add_flg) { _oFile.writeFldName(_iFile,_vfField,true);}
	else				 { _oFile.writeFldName(_vfField, true);}
	int outsize = _iFile.fldSize();
	if(_add_flg) { outsize += _vfField.size(); }	

	while(EOF != _iFile.read() ){
		int outcnt=0;
		for(size_t i=0; i<_iFile.fldSize(); i++){
			char* str=_iFile.getVal(i);
			outcnt++;
			if(_add_flg||_vfField.flg(i)==-1){
				_oFile.writeStr(str,outcnt==outsize);
			}
			else{
				if(_assertNullIN && *str=='\0' ) { _existNullIN  = true;}
				output_n(str,outcnt==outsize);
			}
		}
		if(_add_flg){
			for(kgstr_t::size_type i=0 ; i< _vfField.size() ;i++){
				outcnt++;
				if(_assertNullIN && *_iFile.getVal(_vfField.num(i))=='\0') { _existNullIN  = true;}
				output_n(_iFile.getVal(_vfField.num(i)),outcnt==outsize);
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
int kgVnullto::run(void)
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
    ((kgVnullto*)arg)->runErrEnd();
}

int kgVnullto::run(int inum,int *i_p,int onum, int* o_p,string &msg)
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

