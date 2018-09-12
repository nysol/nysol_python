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
// kgselstr.cpp 文字列による行選択クラス
// =============================================================================
#include <cstdio>
#include <string>
#include <cstring>
#include <vector>
#include <list>
#include <kgselstr.h>
#include <kgError.h>
#include <kgMethod.h>
#include <kgConfig.h>

using namespace std;
using namespace kglib;
using namespace kgmod;

namespace //////////////////////////////////////////////////////////////////////////////////
{
	// -----------------------------------------------------------------------------
	// 比較関数
	// -----------------------------------------------------------------------------
	// ワイド文字による部分文字列比較
	bool strCompSub(char* str,vector<wstring>& vstr)
	{
		wstring ws=toWcs(str);
		for(vector<wstring>::size_type i=0;i<vstr.size();i++){
			if(ws.find(vstr[i])!=wstring::npos) {return true;}
		}
		return false;
	}
	// 通常の部分文字列比較
	bool strCompSub(char* str,vector<string>& vstr)
	{
		for(vector<string>::size_type i=0;i<vstr.size();i++){
			if(NULL!=strstr(str,vstr[i].c_str())) {return true;}
		}
		return false;
	}
	// ワイド文字による前方マッチ比較
	bool strCompHead(char* str,vector<wstring>& vstr)
	{
		wstring ws=toWcs(str);
		for(vector<string>::size_type i=0;i<vstr.size();i++){
			if(ws.compare(0,vstr[i].length(),vstr[i])==0) {return true;}
		}
		return false;
	}
	// 前方マッチ比較
	bool strCompHead(char* str,vector<string>& vstr)
	{
		wstring ws=toWcs(str);
		for(vector<string>::size_type i=0;i<vstr.size();i++){
			if(!strncmp(str,vstr[i].c_str(),strlen(vstr[i].c_str()))) {return true;}
		}
		return false;
	}
	// ワイド文字による後方マッチ比較
	bool strCompTail(char* str,vector<wstring>& vstr)
	{
		wstring ws=toWcs(str);
		size_t len=ws.length();
		for(vector<string>::size_type i=0;i<vstr.size();i++){
			size_t len_v=vstr[i].length();
			if(len < len_v){ continue; }
			if(ws.compare(len-len_v,len_v,vstr[i])==0) {return true;}
		}
		return false;
	}
	// 後方マッチ比較
	bool strCompTail(char* str,vector<string>& vstr)
	{
		size_t len = strlen(str);
		for(vector<string>::size_type i=0;i<vstr.size();i++){
			size_t len_v= strlen(vstr[i].c_str());
			if(len < len_v){ continue; }
			if(!strcmp(str+len-len_v,vstr[i].c_str())) {return true;}
		}
		return false;
	}
	// 完全マッチ比較
	bool strComp(char* str,vector<string>& vstr)
	{
		for(vector<string>::size_type i=0;i<vstr.size();i++){
			if(!strcmp(str,vstr[i].c_str())) {return true;}
		}
		return false;
	}
}//////////////////////////////////////////////////////////////////////////////////


// -----------------------------------------------------------------------------
// コンストラクタ(モジュール名，バージョン登録)
// -----------------------------------------------------------------------------
const char * kgSelstr::_ipara[] = {"i",""};
const char * kgSelstr::_opara[] = {"o","u",""};

kgSelstr::kgSelstr(void)
{
	_name    = "kgselstr";
	_version = "###VERSION###";
	_paralist = "i=,o=,u=,k=,f=,v=,-F,-R,-r,-sub,-W,-head,-tail,bufcount=,-q";
	_paraflg = kgArgs::COMMON|kgArgs::IODIFF|kgArgs::NULL_KEY|kgArgs::NULL_IN;
	
	#include <help/en/kgselstrHelp.h>
	_titleL = _title;
	_docL   = _doc;
	#ifdef JPN_FORMAT
		#include <help/jp/kgselstrHelp.h>
	#endif
	
}
// -----------------------------------------------------------------------------
// パラメータセット＆入出力ファイルオープン
// -----------------------------------------------------------------------------
void kgSelstr::setArgsMain(void)
{

	kgstr_t s=_args.toString("bufcount=",false);
	int bcnt = 10;
	if(!s.empty()){ 
		bcnt = atoi(s.c_str());
		if(bcnt<=0){ bcnt=1;}
	}
	_iFile.setbufsize(bcnt);
	_iFile.read_header();

	// k= 項目引数のセット
	vector<kgstr_t> vs_k = _args.toStringVector("k=",false);

	// f= 項目引数のセット
	vector<kgstr_t>  vs_f = _args.toStringVector("f=",true);

	//-w -sub -head -tail マッチフラグ
	_widechr   = _args.toBool("-W");
	bool s_b	 = _args.toBool("-sub");
	bool t_b	 = _args.toBool("-tail");
	bool h_b	 = _args.toBool("-head");

	// -r 条件反転,-Rキー単位sel、-Fselの条件
	_keysel_flg = _args.toBool("-R");
	_sel_flg    = _args.toBool("-F");

	if( (s_b&&t_b) || (t_b&&h_b) || (h_b&&s_b) ){
		throw kgError("choose one from -sub, -tail and -head.");
	}

	//マッチタイプ設定
	_macth_type = 0;
	if(h_b){ _macth_type=1; }
	if(t_b){ _macth_type=2; }
	if(s_b){ _macth_type=3; }

	// v= 項目引数のセット
	_vField = _args.toStringVector("v=",true);
	for(vector<kgstr_t>::size_type i=0;i<_vField.size();i++){
		if(_macth_type >0 && _widechr){
			_vFieldw.push_back(toWcs(_vField[i]));
		}
	}

	bool seqflg = _args.toBool("-q");

	if(!seqflg && !vs_k.empty()){ sortingRun(&_iFile,vs_k);}

	_kField.set(vs_k, &_iFile,_fldByNum);
	_fField.set(vs_f, &_iFile,_fldByNum);

}

// -----------------------------------------------------------------------------
// パラメータセット＆入出力ファイルオープン
// -----------------------------------------------------------------------------
void kgSelstr::setArgs(void)
{
	// パラメータチェック
	_args.paramcheck(_paralist,_paraflg);
	_reverse    = _args.toBool("-r");

	// 入出力ファイルオープン
	_iFile.open(_args.toString("i=",false),_env,_nfn_i);
	_oFile.open(_args.toString("o=",false),_env,_nfn_o);
	kgstr_t ufile = _args.toString("u=",false);
	if(ufile.empty()){
		_elsefile=false;
	}
	else {
		_elsefile=true;
		_uFile.open(ufile,_env,_nfn_o);
	}		
	setArgsMain();
}

// -----------------------------------------------------------------------------
// パラメータセット＆入出力ファイルオープン
// -----------------------------------------------------------------------------
void kgSelstr::setArgs(int inum,int *i_p,int onum, int* o_p)
{
	int iopencnt = 0;
	int oopencnt = 0;
	try{
		// パラメータチェック
		_args.paramcheck(_paralist,_paraflg);
		_reverse    = _args.toBool("-r");

		if(inum>1 || onum>2){ throw kgError("no match IO");}

		// 入出力ファイルオープン
		if(inum==1 && *i_p>0){ _iFile.popen(*i_p, _env,_nfn_i); }
		else     { _iFile.open(_args.toString("i=",true), _env,_nfn_i); }
		iopencnt++;

		// 出力チェック
		kgstr_t okwd = "o=";
		kgstr_t ukwd = "u=";
		kgstr_t ofile0	= _args.toString(okwd,false);
		kgstr_t ufile0 = _args.toString(ukwd,false);
		int o_no = -1;
		int u_no = -1;
		if(onum>0){ o_no = *o_p;}
		if(onum>1){ u_no = *(o_p+1);}
		if(o_no==-1 && ofile0.empty()){
			if(u_no!=-1 || !ufile0.empty()){
				_reverse = !_reverse;
				kgstr_t swptmp;
				swptmp = ukwd ; ukwd = okwd;  okwd = swptmp;
				int swptmpi;
				swptmpi = u_no ; u_no = o_no ; o_no = swptmpi;
			}
		}

		if(o_no>0){ _oFile.popen(o_no, _env,_nfn_o); }
		else      { _oFile.open(_args.toString(okwd,true), _env,_nfn_o);}
		oopencnt++;

		kgstr_t ufile = _args.toString(ukwd,false);

		if(u_no>0){ 
			_uFile.popen(u_no, _env,_nfn_o); 
			_elsefile=true;
			oopencnt++;
		}
		else if(ufile.empty()){
			_elsefile=false;
		}
		else{
			_uFile.open(ufile,_env,_nfn_o);
			_elsefile=true;
		}
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
//	文字列比較(当てはまればtrue)vstrにある文字列のどれかに一致すればtrue
// -----------------------------------------------------------------------------
bool kgSelstr::str_comp(char* str)
{

	switch(_macth_type){
		case 0:
			if(strComp(str,_vField)) {return true;}
			break;
		case 1:
			if(_widechr){ if(strCompHead(str,_vFieldw)) {return true;} }
			else        { if(strCompHead(str,_vField )) {return true;} }
			break;
		case 2:
			if(_widechr){ if(strCompTail(str,_vFieldw)) {return true;} }
			else        { if(strCompTail(str,_vField )) {return true;} }
			break;
		case 3:
			if(_widechr){ if(strCompSub(str,_vFieldw)) {return true;} }
			else        { if(strCompSub(str,_vField )) {return true;} }
			break;
	}
	return false;
}
// -----------------------------------------------------------------------------
// 条件による	文字列チェックcheck(当てはまればtrue)
// -----------------------------------------------------------------------------
bool kgSelstr::str_check(vector<int>& cnt,char** str)
{
	bool rtn=false;
	for(vector<int>::size_type i=0;i<cnt.size();i++){
		char *tmpchar = *(str+cnt[i]);
		if(_assertNullIN && *tmpchar=='\0') { _existNullIN  = true;}
		
		if(_reverse){//条件反転
			if(_sel_flg){//すべて不一致
				if(str_comp(tmpchar)){ rtn=false; continue;}
				else{rtn = true; break;}				
			}
			else{/*どれかが不一致*/
				if(str_comp(tmpchar)){ rtn=false;break;}
				else{ rtn = true;continue;}
			}
		}
		else{
			if(_sel_flg){//すべて一致
				if(str_comp(tmpchar)){rtn=true;continue;}
				else{rtn = false;break;}
			}
			else{/*どれか一致*/
				if(str_comp(tmpchar)){rtn=true;break;}
				else{rtn = false;continue;}				
			}
		}
	}
	return rtn;
}
// -----------------------------------------------------------------------------
// 実行
// -----------------------------------------------------------------------------
int kgSelstr::runMain(void)
{

	// 入力ファイルにkey項目番号をセットする．
	_iFile.setKey(_kField.getNum());

	// 項目名出力
	_oFile.writeFldName(_iFile);
	if(_elsefile){ _uFile.writeFldName(_iFile); }

	vector<int> fieldlst= _fField.getNum();

	// キー単位処理をするかで処理を変える
	if(_kField.size()){

		// 条件反転がセットされている場合、初期値をfalse
		// 逆にセットされていない場合、初期値をtrue
		list<char**> stock_data;
		while(_iFile.blkset()!=EOF){
			bool finalSel=false;		
			while(_iFile.blkread() != EOF){
       bool lineSel=str_check(fieldlst,_iFile.getBlkFld());
				if(_reverse){
					if(_keysel_flg){
           if(!lineSel){finalSel=false; continue;}
           else       {finalSel=true; break   ;}
					}
					else{
           if(!lineSel){finalSel=false; break   ;}
           else       {finalSel=true; continue;}
					}
				}
				else{ 
					/*and条件*/
					if(_keysel_flg){
						if(lineSel){finalSel=true; continue;}
						else       {finalSel=false; break   ;}
					}
					/*or条件*/
					else{
						if(lineSel){finalSel=true; break   ;}
						else       {finalSel=false; continue;}
					}
				}
			}
			_iFile.seekBlkTop();
			if(finalSel){
				while(_iFile.blkread() != EOF){
					_oFile.writeFld(_iFile.fldSize(),_iFile.getBlkFld());
				}
			}
			else{
				if(_elsefile){
					while(_iFile.blkread() != EOF){
						_uFile.writeFld(_iFile.fldSize(),_iFile.getBlkFld());
					}
				}
			}
		}
	}
	else{//行単位処理
		while(_iFile.read()!=EOF){			
			if(_iFile.status() & kgCSV::End)break;
			if(str_check(fieldlst,_iFile.getNewFld())){
				_oFile.writeFld(_iFile.fldSize(),_iFile.getNewFld());
			}
			else{
				if(_elsefile){
					_uFile.writeFld(_iFile.fldSize(),_iFile.getNewFld());
				}
			}
		}
	}

	//ASSERT keynull_CHECK
	if(_assertNullKEY) { _existNullKEY = _iFile.keynull(); }

	th_cancel();
	_iFile.close();
	_oFile.close();
	if(_elsefile){ _uFile.close();}

	return 0;

}

// -----------------------------------------------------------------------------
// 実行 
// -----------------------------------------------------------------------------
int kgSelstr::run(void) 
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
    ((kgSelstr*)arg)->runErrEnd();
}

int kgSelstr::run(int inum,int *i_p,int onum, int* o_p,string &msg)
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

