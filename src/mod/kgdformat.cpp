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
// kgdformat.h 日付時間フォーマットによる文字列置換
// =============================================================================
#include <cstdio>
#include <sstream>
#include <kgdformat.h>
#include <kgError.h>
#include <kgMethod.h>

using namespace std;
using namespace kglib;
using namespace kgmod;


kgstr_t kgTimeparser::extraction(char * cstr , bool err )
{
	kgTimeFormat tf;
	char* cpos=cstr;

	int limit=strlen(cstr);
	if(limit==0){ return "";}
	for(vector<char>::size_type i=0;i<pattrn.size();i++){
		int l =  length[i];
		char* bpos=_buf+start[i];
		if(l!=0 && strncmp(bpos,cpos,l)){
			if(*bpos==' ' && ( (cpos!=cstr) && *(cpos-1)==' ') ){ //先頭がspaceの場合は特殊処理
				cpos--;
				i--;
				continue;
			}
			return "";	
		}
		cpos+=l;
		bool setflg=true;
		if(pattrn[i]=='Y'){
			int len = numlength(cpos,4);
			setflg = tf.set(cpos,len,kgTimeFormat::YEAR);
			cpos+=len;
		}
		else if(pattrn[i]=='y'){
			int len = numlength(cpos,2);
			setflg = tf.set(cpos,len,kgTimeFormat::YEAR);
			cpos+=len;
		}
		else if(pattrn[i]=='m'){
			int len = numlength(cpos,2);
			setflg = tf.set(cpos,len,kgTimeFormat::MON);
			cpos+=len;
		}
		else if(pattrn[i]=='d'){
			int len = numlength(cpos,2);
			setflg = tf.set(cpos,len,kgTimeFormat::DAY);
			cpos+=len;
		}
		else if(pattrn[i]=='H'){
			int len = numlength(cpos,2);
			setflg = tf.set(cpos,len,kgTimeFormat::HOUR);
			cpos+=len;
		}
		else if(pattrn[i]=='M'){
			int len = numlength(cpos,2);
			setflg = tf.set(cpos,len,kgTimeFormat::MIN);
			cpos+=len;
		}
		else if(pattrn[i]=='S'){
			int len = numlength(cpos,2);
			setflg = tf.set(cpos,len,kgTimeFormat::SEC);
			cpos+=len;
		}
		else if(pattrn[i]=='s'){
			int len = numlength(cpos,2);
			setflg = tf.set(cpos,len,kgTimeFormat::SEC);
			cpos+=len;
			if(setflg && *cpos=='.'){
				cpos++;
				int len = numlength(cpos,6);
				setflg = tf.set(cpos,len,kgTimeFormat::USEC);
				cpos+=len;
			}
		}
		if (!setflg){ return "";}
	}
	if(*cpos!='\0'){ return "";}
	return tf.str(_outpat);
}
void kgTimeparser::compile(const char *str)
{
	strcpy(_buf,str);
	bool aFlg=false; 
	char * p = _buf;
	char * s = _buf;
	_outpat =0;
	while(*p){
		if(!aFlg){
			if(*p=='%'){ aFlg=true;}
		}
		else{
			switch(*p){
				case '%':
					aFlg=false;
					break;
				case 'Y':
					_outpat |= kgTimeFormat::YEAR;
					break;
				case 'y':
					_outpat |= kgTimeFormat::YEAR;
					break;
				case 'm':
					_outpat |= kgTimeFormat::MON;
					break;
				case 'd':
					_outpat |= kgTimeFormat::DAY;
					break;
				case 'H':
					_outpat |= kgTimeFormat::HOUR;
					break;
				case 'M':
					_outpat |= kgTimeFormat::MIN;
					break;
				case 'S':
					_outpat |= kgTimeFormat::SEC;
					break;
				case 's':
					_outpat |= kgTimeFormat::SEC;
					break;
				default:
					ostringstream oss;
					oss << "format Error : " << _buf;
					throw kgError(oss.str());	
			}
			pattrn.push_back(*p);
			start.push_back(s-_buf);
			length.push_back(p-s-1);
			s=p+1;
			aFlg=false;
		}
		p++;
	}
	if( p-s != 0){
		start.push_back(s-_buf);
		length.push_back(p-s);
		pattrn.push_back('\0');
	}
}

// -----------------------------------------------------------------------------
// コンストラクタ(モジュール名，バージョン登録,パラメータ)
// -----------------------------------------------------------------------------
const char * kgDformat::_ipara[] = {"i",""};
const char * kgDformat::_opara[] = {"o",""};


kgDformat::kgDformat(void)
{
	_name    = "kgdformat";
	_version = "###VERSION###";

	_paralist = "f=,i=,o=,c=,-A";
	_paraflg = kgArgs::COMMON|kgArgs::IODIFF|kgArgs::NULL_IN|kgArgs::NULL_OUT;

	#include <help/en/kgdformatHelp.h>
	_titleL = _title;
	_docL   = _doc;
	#ifdef JPN_FORMAT
		#include <help/jp/kgdformatHelp.h>
	#endif

}
// -----------------------------------------------------------------------------
// 入出力ファイルオープン
// -----------------------------------------------------------------------------
void kgDformat::setArgsMain(void)
{
	_iFile.read_header();

	// -A:追加フラグ
	_add 			= _args.toBool("-A");

	// f= 項目引数のセット
	vector< vector<kgstr_t> > vvs = _args.toStringVecVec("f=",':',2,true);
	_fField.set(vvs, &_iFile,_fldByNum);

	// c= 正規表現のセット
	_regstr = _args.toString("c=",true);

 	//フォーマット
 	_parser.compile(_regstr.c_str());
}
// -----------------------------------------------------------------------------
// 入出力ファイルオープン
// -----------------------------------------------------------------------------
void kgDformat::setArgs(void)
{
	// パラメータチェック
	_args.paramcheck(_paralist,_paraflg);

	// 入出力ファイルオープン
	_iFile.open(_args.toString("i=",false), _env, _nfn_i);
	_oFile.open(_args.toString("o=",false), _env, _nfn_o,_rp);

	setArgsMain();

}
// -----------------------------------------------------------------------------
// 入出力ファイルオープン
// -----------------------------------------------------------------------------
void kgDformat::setArgs(int inum,int *i_p,int onum, int* o_p)
{
	int iopencnt = 0;
	int oopencnt = 0;
	try{
		// パラメータチェック
		_args.paramcheck(_paralist,_paraflg);

		if(inum>1 || onum>1){ throw kgError("no match IO");}

		if(inum==1 && *i_p>0){ _iFile.popen(*i_p, _env,_nfn_i);  }
		else     { _iFile.open(_args.toString("i=",true), _env,_nfn_i); }
		iopencnt++;

		if(onum==1 && *o_p>0){ _oFile.popen(*o_p, _env,_nfn_o,_rp); }
		else     { _oFile.open(_args.toString("o=",true), _env,_nfn_o,_rp);}
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
// 出力（置換用）
// -----------------------------------------------------------------------------
void kgDformat::writeFld(char** fld, const vector<int>* flg, const vector<string>* val)
{
	unsigned int i;
	int num;
	for(i=0; i<flg->size()-1; i++){ 
		num=flg->at(i);               
		if(num == -1) _oFile.writeStr( *(fld+i)   					, false );
		else          _oFile.writeStr( val->at(num).c_str()	, false );
	}
		num=flg->at(i);
		if(num == -1) _oFile.writeStr( fld[i]      					, true  );
		else          _oFile.writeStr( val->at(num).c_str()	, true  );
}
// -----------------------------------------------------------------------------
// 実行
// -----------------------------------------------------------------------------
int kgDformat::runMain(void)
{
	//出力項目名出力 追加 or 置き換
	if(_add) { _oFile.writeFldName(_iFile,_fField,true); }
	else		 { _oFile.writeFldName(_fField, true);}

	vector<string> val;
	// データ出力
	while( EOF != _iFile.read() ){
		// 変換
		for(vector<kgstr_t>::size_type i=0; i<_fField.size(); i++){
			if(_assertNullIN && *_iFile.getVal(_fField.num(i))=='\0'){  _existNullIN = true; }
			string tval =	_parser.extraction( _iFile.getVal(_fField.num(i)) );
			if(_assertNullOUT && tval.size()==0 ){  _existNullOUT = true; }
			val.push_back( tval	);
		}
		// 出力　追加 or 置換 
		if(_add) { _oFile.writeFld(_iFile.getFld(),_iFile.fldSize(),&val); }
		else		 { writeFld(_iFile.getFld(),_fField.getFlg_p(),&val);}

		val.clear();
	}

	// 終了処理
	_iFile.close();
	_oFile.close();

	return 0;

}
// -----------------------------------------------------------------------------
// 実行 
// -----------------------------------------------------------------------------
int kgDformat::run(void) 
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
    ((kgDformat*)arg)->runErrEnd();
}

int kgDformat::run(int inum,int *i_p,int onum, int* o_p,string &msg)
{
	int sts=1;
	//pthread_cleanup_push(&cleanup_handler, this);	

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
  //pthread_cleanup_pop(0);
	return sts;
}
