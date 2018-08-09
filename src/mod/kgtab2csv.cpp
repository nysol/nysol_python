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
// kgduprec.cpp 行の複製
// =============================================================================
#include <cstdio>
#include <kgtab2csv.h>
#include <kgError.h>
#include <kgConfig.h>

using namespace std;
using namespace kglib;
using namespace kgmod;

// -----------------------------------------------------------------------------
// コンストラクタ(モジュール名，バージョン登録,パラメータ)
// -----------------------------------------------------------------------------
const char * kgTab2csv::_ipara[] = {"i",""};
const char * kgTab2csv::_opara[] = {"o",""};

kgTab2csv::kgTab2csv(void)
{
	_name    = "kgtab2csv";
	_version = "###VERSION###";

	_paralist = "i=,o=,-r,d=";
	_paraflg = kgArgs::COMMON|kgArgs::IODIFF;

	#include <help/en/kgtab2csvHelp.h>
	_titleL = _title;
	_docL   = _doc;
	#ifdef JPN_FORMAT
		#include <help/jp/kgtab2csvHelp.h>
	#endif
	
}

// -----------------------------------------------------------------------------
// パラメータセット
// -----------------------------------------------------------------------------
void kgTab2csv::setArgsMain(void)
{
	_iFile.read_header();

	// -r 出力反転フラグ
	_remove = _args.toBool("-r");

	kgstr_t s_d = _args.toString("d=",false);
	if(s_d.empty()){	
		_delim='\t';
	}else if(s_d.size()!=1){
		ostringstream ss;
		ss << "delim= takes 1 byte charactor (" << s_d << ")";
		throw kgError(ss.str());
	}else{
		_delim=*(s_d.c_str());
	}

}
// -----------------------------------------------------------------------------
// パラメータチェック＆入出力ファイルオープン
// -----------------------------------------------------------------------------
void kgTab2csv::setArgs(void)
{
	// パラメータチェック
	_args.paramcheck("i=,o=,-r,d=",kgArgs::COMMON|kgArgs::IODIFF);

	// 入出力ファイルオープン
	_iFile.open(_args.toString("i=",false), _env, _nfn_i);
	_oFile.open(_args.toString("o=",false), _env, _nfn_o);

	setArgsMain();

}

void kgTab2csv::setArgs(int inum,int *i_p,int onum ,int *o_p)
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
int kgTab2csv::runMain(void)
{
	char * data;
	size_t fcnt=0;

	// headerがあるとき
	if(!_nfn_i){
		vector<string> head;
		if(EOF != _iFile.read()){
			char * data = _iFile.getRec();
			if (_remove){
				char *p = data;
				while( *p ){ p++;}
				if(p!=data && *(p-1)=='\r'){
					*(p-1)='\0';
				}
			}
			string hdata = data;
			head = splitToken(hdata,_delim);
			fcnt = head.size();
		}
		// headerを出力するとき
		if(!_nfn_o){ _oFile.writeFldName(head);}
	}
	// 行数を取得してデータ出力
	while( EOF != _iFile.read() ){
		data = _iFile.getRec();
		vector<char*> d_split = splitToken(data,_delim);
		if(fcnt==0){ fcnt = d_split.size();}
		if ( fcnt != d_split.size()){
			ostringstream ss;
			ss << "unmatch field size: ( " << fcnt << "," << d_split.size();
			throw kgError(ss.str());
		}
		if (_remove){
			char *p = d_split.back();
			while( *p ){ p++;}
			if(p!=d_split.back() && *(p-1)=='\r'){
				*(p-1)='\0';
			}
		}
		for(size_t i=0; i<fcnt; i++){
			_oFile.writeStr( d_split[i], i == fcnt-1);
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
int kgTab2csv::run(void)
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
    ((kgTab2csv*)arg)->runErrEnd();
}

int kgTab2csv::run(int inum,int *i_p,int onum, int* o_p,string &msg)
{
	int sts = 1;
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

