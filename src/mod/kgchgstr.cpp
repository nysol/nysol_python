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
// kgchgstr.cpp 文字列の置換
// =============================================================================
#include <cstdio>
#include <iostream>
#include <sstream>
#include <kgchgstr.h>
#include <kgError.h>
#include <kgMethod.h>

using namespace std;
using namespace kglib;
using namespace kgmod;

//static function
namespace 
{
	// -----------------------------------------------------------------------------
	// 文字列置換
	// cstrのリストを元にstr文字列を置換してbufにセットする
	// 置換できた場合、返り値が
	// -----------------------------------------------------------------------------
	bool chgstrSub(char* buf, char* str, vector<vector<string> >& cstr)
	{
		bool ret=false;
		string s(str);
 		for(unsigned int i=0; i<cstr.at(0).size(); i++){
			string::size_type pos = s.find( cstr.at(0).at(i) );
			if( pos != string::npos ){
				s.replace( pos, cstr.at(0).at(i).size(), cstr.at(1).at(i) );
				ret=true;
			}
		}
		int len=strlen(s.c_str());
		if(len>=KG_MAX_STR_LEN || len==0){
			buf[0]='\0';
		}else{
			strncpy(buf,s.c_str(),len);
			buf[len]='\0';			
		}
		return ret;
	}
	// -----------------------------------------------------------------------------
	// 文字列置換:置換リストwstringバージョン
	// cstrのリストを元にstr文字列を置換してbufにセットする
	// -----------------------------------------------------------------------------
	bool chgstrSub(char* buf, char* str, vector<vector<wstring> >& cstr)
	{
		bool ret=false;
		wstring s=toWcs(str);
		for(unsigned int i=0; i<cstr.at(0).size(); i++){
			wstring::size_type pos = s.find( cstr.at(0).at(i) );
			if( pos != wstring::npos ){
				s.replace( pos, cstr.at(0).at(i).size(), cstr.at(1).at(i) );
				ret=true;
			}
		}
		string ss=toMbs(s);
		int len=strlen(ss.c_str());
		if(len>=KG_MAX_STR_LEN||len==0){
			buf[0]='\0';
		}else{
			strncpy(buf,ss.c_str(),len);
			buf[len]='\0';			
		}
		return ret;
	}
}
// -----------------------------------------------------------------------------
// コンストラクタ(モジュール名，バージョン登録)
// -----------------------------------------------------------------------------
const char * kgChgstr::_ipara[] = {"i",""};
const char * kgChgstr::_opara[] = {"o",""};

kgChgstr::kgChgstr(void){

	_name    = "kgchgstr";
	_version = "###VERSION###";

	_paralist = "i=,o=,c=,f=,-F,-A,O=,-sub,-W";
	_paraflg = kgArgs::COMMON|kgArgs::IODIFF|kgArgs::NULL_IN|kgArgs::NULL_OUT;

	#include <help/en/kgchgstrHelp.h>
	_titleL = _title;
	_docL   = _doc;
	#ifdef JPN_FORMAT
		#include <help/jp/kgchgstrHelp.h>
	#endif

}
// -----------------------------------------------------------------------------
// パラメータセット
// -----------------------------------------------------------------------------
void kgChgstr::setArgsMain(void)
{
	_iFile.read_header();

	// f= 項目引数のセット
	vector< vector<kgstr_t> > vvs_f = _args.toStringVecVec("f=",':',2,true);
	_fField.set(vvs_f, &_iFile, _fldByNum);

	// フラグセット
	_add_flg		= _args.toBool("-A");
	_F_flg			= _args.toBool("-F");
	_substr     = _args.toBool("-sub");
	_widechr    = _args.toBool("-W");

	//O=条件外文字列指定があればセット
	_elsestr = _args.toString("O=",false);
	if(_elsestr.empty()) { _estrflg=false; }
	else								 { _estrflg=true; }
	if(_F_flg && _estrflg){ throw kgError("O= and -F cannot be specified at the same time." ); } 

	// c= 項目引数のセット
	vector< vector<kgstr_t> > vvs_c = _args.toStringVecVec("c=",':',2,true);

	// 部分マッチの場合はc=をベクトルにセットする
	// -wが指定されたときはwstringに変換
	if(_substr){	
		if(_widechr)	{ _cFieldSubw.resize(2); }
		else					{ _cFieldSub.resize(2);  }
	}
	for(vector<kgstr_t>::size_type i=0;i<vvs_c[0].size();i++){
		if(_substr){
			if(_widechr){
				_cFieldSubw.at(0).push_back(toWcs(vvs_c.at(0).at(i)));
				_cFieldSubw.at(1).push_back(toWcs(vvs_c.at(1).at(i)));
			}
			else{
				_cFieldSub.at(0).push_back(vvs_c.at(0).at(i));
				_cFieldSub.at(1).push_back(vvs_c.at(1).at(i));
			}
		}else{
			_cField[vvs_c[0].at(i)] = vvs_c[1].at(i);
		}
	}
}

// -----------------------------------------------------------------------------
// パラメータチェック&入出力ファイルオープン
// -----------------------------------------------------------------------------
void kgChgstr::setArgs(void)
{
	// 
	_args.paramcheck(_paralist,_paraflg);

	// 入出力ファイルオープン
	_iFile.open(_args.toString("i=",false),_env,_nfn_i);
	_oFile.open(_args.toString("o=",false),_env,_nfn_o);

	setArgsMain();
}
// -----------------------------------------------------------------------------
void kgChgstr::setArgs(int inum,int *i_p,int onum ,int *o_p)
{
	int iopencnt = 0;
	int oopencnt = 0;
	try{

		_args.paramcheck(_paralist,_paraflg);

		if(inum>1 || onum>1){ throw kgError("no match IO");}

		if(inum==1 && *i_p>0){ _iFile.popen(*i_p, _env,_nfn_i);}
		else     { _iFile.open(_args.toString("i=",true), _env,_nfn_i); }
		iopencnt++;

		if(onum==1 && *o_p>0){ _oFile.popen(*o_p, _env,_nfn_o); }
		else     { _oFile.open(_args.toString("o=",true), _env,_nfn_o);}
		iopencnt++;

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
int kgChgstr::runMain(void)
{
	// 項目名出力
	if(_add_flg)	{ _oFile.writeFldName(_iFile,_fField,true);}
	else					{ _oFile.writeFldName(_fField, true);}

	// 領域の確保 f=項目数分
	kgAutoPtr2<char*> ap1;
	try {
		ap1.set( new char*[_fField.size()] ); // 行数×csv項目数
	} catch(...) {
		throw kgError("memory allocation error ");
	}
	char** constr = ap1.get();

	// 部分文字列置換の場合の置換後文字列領域の確保

	char **buf = NULL;
	size_t fcnt =_fField.size();


	if(_substr){
		try{
			buf = new char*[_fField.size()];
		} catch(...) {
			buf=NULL;
			throw kgError("memory allocation error ");
		}	

		for(unsigned int i=0;i<fcnt;i++){
			try {
				buf[i] = new char[KG_MAX_STR_LEN];
			} catch(...) {
				for(size_t j=0;j<i;j++){
					delete [] buf[j];
				}
				delete [] buf;
				buf = NULL;
				throw kgError("memory allocation error ");
			}
		}
	}

	try{

		// データ出力
		while(EOF != _iFile.read() ){
			for(size_t  i=0;i<_fField.size();i++){
				bool match=false;	
				if(*( _iFile.getVal(_fField.num(i)) ) == '\0'){ 
					*(constr+i)=const_cast<char*>("");
					if(_assertNullIN) { _existNullIN  = true;}
					if(_assertNullOUT){ _existNullOUT = true;}
				}
				else{
					if(_substr){
						if(_widechr){
							//match=chgstrSub(buf.at(i),_iFile.getVal(_fField.num(i)),_cFieldSubw);
							match=chgstrSub(buf[i],_iFile.getVal(_fField.num(i)),_cFieldSubw);
						}else{
							//match=chgstrSub(buf.at(i),_iFile.getVal(_fField.num(i)),_cFieldSub );
							match=chgstrSub(buf[i],_iFile.getVal(_fField.num(i)),_cFieldSub );
						}
						*(constr+i)=buf[i];
					}else{
						map<string,string>::iterator k
							= _cField.find(_iFile.getVal(_fField.num(i)));
						//条件に一致するかチェック
						if( k != _cField.end() ){//一致した場合
							*(constr+i)=const_cast<char*>((k->second).c_str());
							match=true;
						}
					}
					//一致しなかった場合	 O=:指定文字列,F:項目値,デフォルト:NULL
					if(!match){
						if   (_estrflg) { *(constr+i) = const_cast<char*>(_elsestr.c_str());}
						else if(_F_flg) { *(constr+i) = _iFile.getVal(_fField.num(i));}
						else            { 
							*(constr+i) = const_cast<char*>("");
							if(_assertNullOUT){ _existNullOUT = true;}
						}
					}
				}
			}
			if(_add_flg)	{ _oFile.writeFld(_iFile.getFld(),_iFile.fldSize(),constr,_fField.size());}
			else					{ _oFile.writeFld(_iFile.getFld(),_fField.getFlg_p(),constr);		}		
		}

		_iFile.close();
		_oFile.close();

	}catch(kgError& err){
		if(buf!=NULL){
			for(size_t i=0;i<fcnt;i++){
				delete [] buf[i];
			}
			delete [] buf;
		}
		throw err;
	}
	catch(...) {
		if(buf!=NULL){
			for(size_t i=0;i<fcnt;i++){
				delete [] buf[i];
			}
			delete [] buf;
		}
		throw kgError("memory alloc error ");
	}

	if(buf!=NULL){
		for(size_t i=0;i<fcnt;i++){
			delete [] buf[i];
		}
		delete [] buf;
	}

	return 0;

}
// -----------------------------------------------------------------------------
// 実行 
// -----------------------------------------------------------------------------
int kgChgstr::run(void) 
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
    ((kgChgstr*)arg)->runErrEnd();
}

int kgChgstr::run(int inum,int *i_p,int onum, int* o_p,string &msg)
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

