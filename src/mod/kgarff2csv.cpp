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
// ============================================================================
// kgarff2csv.cpp : wekaのデータフォーマット→CSV変換
// ============================================================================
#include <sstream>
#include <vector>
#include <kgarff2csv.h>
#include <kgError.h>
#include <kgMethod.h>
using namespace std;
using namespace kglib;
using namespace kgmod;

	// arff formatからデータ抽出 
	// @attribute行の２項目目が項目名なのでその部分setする
	void kgArff2csv::arff2data(char* recdata,char* setdata)
	{
		bool s_demi=false,e_demi=false;
		char *p=recdata,*q=setdata;
		while(*p){
			if(*p==' '||*p=='\t'){
				p++;
				if(s_demi==false){
					s_demi=true;
					continue;
				}
				else{
					if(e_demi==false)	{ continue; }
					else							{ break; }
				}
			}
			if(s_demi==true){ e_demi=true;}
			if(s_demi==true&&e_demi==true){ *q++=*p; }
			p++;
		}
		*q='\0';
	}
	// 文字列の比較（小文字アルファベットに変換してから）
	bool kgArff2csv::sameStr(const char* rec, const char* str)
	{
		if(*rec=='\0' || *str=='\0'){ return false; }
		while(*rec!='\0' && *str!='\0'){
			if(tolower(*rec)!=tolower(*str)) return false;
			rec++;str++;
		}
		return true;
	}

// -----------------------------------------------------------------------------
// コンストラクタ(モジュール名，バージョン登録,パラメータ)
// -----------------------------------------------------------------------------
const char * kgArff2csv::_ipara[] = {"i",""};
const char * kgArff2csv::_opara[] = {"o",""};

kgArff2csv::kgArff2csv(void)
{
	_name    = "kgarff2csv";
	_version = "###VERSION###";

	_paralist = "i=,o=";
	_paraflg = kgArgs::COMMON|kgArgs::NULL_OUT;

	#include <help/en/kgarff2csvHelp.h>
	_titleL = _title;
	_docL   = _doc;
	#ifdef JPN_FORMAT
		#include <help/jp/kgarff2csvHelp.h>
	#endif
}
// -----------------------------------------------------------------------------
// パラメータセット＆入出力ファイルオープン
// 入力ファイル ヘッダ無パターンで読み込む
// -----------------------------------------------------------------------------
void kgArff2csv::setArgs(void)
{
	_args.paramcheck(_paralist,_paraflg);

	_iFile.open(_args.toString("i=",false), _env, true);
	_oFile.open(_args.toString("o=",false), _env, _nfn_o);
	_iFile.read_header();
}

void kgArff2csv::setArgs(int inum,int *i_p,int onum ,int *o_p)
{
	int iopencnt = 0;
	int oopencnt = 0;
	try {

		_args.paramcheck(_paralist,_paraflg);

		if(inum>1 || onum>1){ throw kgError("no match IO");}

		// 入出力ファイルオープン
		if(inum==1 && *i_p>0){ _iFile.popen(*i_p, _env, true);}
		else     { _iFile.open(_args.toString("i=",true), _env, true); }
		iopencnt++;

		if(onum==1 && *o_p>0){ _oFile.popen(*o_p, _env,_nfn_o); }
		else     { _oFile.open(_args.toString("o=",true), _env,_nfn_o);}
		oopencnt++;

		_iFile.read_header();

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
// arffは@ATTRIBUTEが項目名行(上から順番にcsvの左から順番の項目になる)で、
// @DATA行以降がデータ(csv)になるので@DATA行以降csvとして出力していく(DQのチェックも行う)
// -----------------------------------------------------------------------------
int kgArff2csv::runMain()
{

	bool dataflg=false;
	size_t fldcnt=0;
	char** sepstr=0;

	kgAutoPtr2<char> buf_ap;
	kgAutoPtr2<char*> ap;
	vector<kgstr_t> outfld;
	buf_ap.set( new char[KG_MaxRecLen] );
	char *strtmp = buf_ap.get();
	char *recstr;

	// arff -> csv変換処理
	while( EOF != _iFile.read() ){
		recstr =	_iFile.getRec();
		//コメント行はskip
		if(*recstr=='%'){continue; }
		if(dataflg){//データ行出力
			strcpy(strtmp,recstr);
			sepFldToken(sepstr,fldcnt,strtmp);
			if(_assertNullOUT){ 
				for(size_t i=0 ; i<fldcnt ; i++){
					if( *sepstr[i] =='\0' ){ _existNullOUT = true; break;}
				}
			}
			_oFile.writeFld(fldcnt,sepstr);

		}
		else{
			char *p =strtmp;
			if(sameStr(recstr,"@attribute")){
				arff2data(recstr,p);
				fldcnt++;
				outfld.push_back(p);
			}
			if(sameStr(recstr,"@data")){
				if( !_oFile.noFldName( ) ){
					_oFile.writeFldNameCHK(outfld);
				}
				dataflg=true;
				try {
					ap.set( new char*[fldcnt] );
				}catch(...){
					throw kgError("memory allocation error");
				}
				sepstr=ap.get();
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
int kgArff2csv::run(void)
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
    ((kgArff2csv*)arg)->runErrEnd();
}

int kgArff2csv::run(int inum,int *i_p,int onum, int* o_p,string &msg)
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
