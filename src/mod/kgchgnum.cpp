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
// kgchgnum.cpp 数値範囲による置換
// =============================================================================
#include <cstdio>
#include <sstream>
#include <cfloat>
#include <kgchgnum.h>
#include <kgError.h>


using namespace std;
using namespace kglib;
using namespace kgmod;

// -----------------------------------------------------------------------------
// コンストラクタ(モジュール名，バージョン登録)
// -----------------------------------------------------------------------------
const char * kgChgnum::_ipara[] = {"i",""};
const char * kgChgnum::_opara[] = {"o",""};

kgChgnum::kgChgnum(void)
{
	_name    = "kgchgnum";
	_version = "###VERSION###";

	// パラメータチェック
	_paralist = "v=,f=,R=,O=,-F,-A,-r,i=,o=";
	_paraflg = kgArgs::COMMON|kgArgs::IODIFF|kgArgs::NULL_IN|kgArgs::NULL_OUT;

	#include <help/en/kgchgnumHelp.h>
	_titleL = _title;
	_docL   = _doc;
	#ifdef JPN_FORMAT
		#include <help/jp/kgchgnumHelp.h>
	#endif

}
// -----------------------------------------------------------------------------
// 入出力ファイルオープン
// -----------------------------------------------------------------------------
void kgChgnum::setArgsMain(void)
{
	_iFile.read_header();

	// f= 項目引数のセット
	vector< vector<kgstr_t> > vvs = _args.toStringVecVec("f=",':',2,true);
	_fField.set(vvs, &_iFile, _fldByNum);

	// -F,-A,-rフラグ
	_F_flg	= _args.toBool("-F");
	_add		= _args.toBool("-A");
	_range	= _args.toBool("-r");

	//O=の指定
	_OField = _args.toString("O=",false);
	if(_OField.empty())	{ _O_flg=false;}
	else                { _O_flg=true; }
	if(_F_flg && _O_flg){ throw kgError("O= and -F cannot be specified at the same time."); } 


	// R=,v= 項目引数のセット
	vector<kgstr_t> vs =_args.toStringVector("R=",true);
	for(vector<kgstr_t>::size_type i=0; i<vs.size();i++){
		if(vs[i]=="MIN")			{ _RField.push_back(-DBL_MAX); }		
		else if(vs[i]=="MAX")	{ _RField.push_back(DBL_MAX);  }
		else									{ _RField.push_back(atof(vs[i].c_str()));}
	}

	//v=の指定がなければダミーデータをセットするfrom_toの文字列をセットする
	_vField = _args.toStringVector("v=",false);
	if(_vField.empty()){
		kgstr_t f,t;
		for(vector<kgstr_t>::size_type i=0; i<vs.size()-1;i++){
			if(vs[i]=="MIN")		{ f = "";      }
			else 								{ f = vs[i];   }
			if(vs[i+1]=="MAX")	{ t = "";      }
			else 								{ t = vs[i+1]; }
			_vField.push_back(f+"_"+t);
		}
	}
	// v=とR=の数チェック(v=の数はR=の数-1,R=の数は2以上)
	if(_RField.size()>=2 && _vField.size()+1!=_RField.size()){
		ostringstream ss;
		ss << "not match r= v= size " << _RField.size() << " " << _vField.size() ;
		throw kgError(ss.str());
	}
}
// -----------------------------------------------------------------------------
// 入出力ファイルオープン
// -----------------------------------------------------------------------------
void kgChgnum::setArgs(void)
{
	_args.paramcheck(_paralist,_paraflg);

	// 入出力ファイルオープン
	_iFile.open(_args.toString("i=",false), _env, _nfn_i);
	_oFile.open(_args.toString("o=",false), _env, _nfn_o);

	setArgsMain();

}
// -----------------------------------------------------------------------------
void kgChgnum::setArgs(int inum,int *i_p,int onum ,int *o_p)
{
	int iopencnt = 0;
	int oopencnt = 0;
	try{

		_args.paramcheck(_paralist,_paraflg);
	
		if(inum>1 || onum>1){ throw kgError("no match IO");}

		// 入出力ファイルオープン
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
// 範囲チェック
// -----------------------------------------------------------------------------
int kgChgnum::rangecheck(char *str) 
{
	vector<double>::size_type i,match_no=-1;
	if(*str=='\0') return -1;
	double ckval = atof(str);
	
	for(i=0;i<_RField.size();i++){
		if(_range){//より大きい
			if(ckval>_RField[i]) { match_no=i;}
			else								 { break; }
		}
		else{//以上
			if(ckval>=_RField[i])	{ match_no=i;}
			else									{ break; }
		}
	}
	//最終項目で一致していれば範囲外
	if(match_no==_RField.size()-1){ return -1;	}
	return match_no;
}


int kgChgnum::runMain(void) 
{
	// 項目名出力
	if(_add){ _oFile.writeFldName(_iFile,_fField,true);}
	else		{ _oFile.writeFldName(_fField, true);	}

	// 領域の確保 f=項目数分
	kgAutoPtr2<char*> ap2;
	try {
		ap2.set( new char*[_fField.size()] ); 
	} catch(...) {
		throw kgError("memory allocation error ");
	}
	char** constr = ap2.get();

	// データ出力
	while( EOF != _iFile.read() ){
		for(size_t i=0;i<_fField.size();i++){
			char *strtmp = _iFile.getVal(_fField.num(i));
			if(*strtmp=='\0'){ 
				*(constr+i)=const_cast<char*>(""); 
				if(_assertNullIN) { _existNullIN  = true;}
				if(_assertNullOUT){ _existNullOUT = true;}
			}
			else{
				int range_rtn = rangecheck(strtmp);
				if(range_rtn==-1){
					//範囲外  O=:指定文字列,F:項目値,デフォルト:NULL
							 if(_O_flg)	{ *(constr+i)=const_cast<char*>(_OField.c_str());}
					else if(_F_flg)	{ *(constr+i)=strtmp;}
					else						{ 
						*(constr+i)=const_cast<char*>("");
						if(_assertNullOUT){ _existNullOUT = true;}
					}
				}	
				else{
					*(constr+i)= const_cast<char*>( _vField.at(range_rtn).c_str() );
				}
			}
		}
		if(_add) { _oFile.writeFld(_iFile.getFld(),_iFile.fldSize(),constr,_fField.size());}
		else		 { _oFile.writeFld(_iFile.getFld(),_fField.getFlg_p(),constr);}
	}

	// 終了処理
	_iFile.close();
	_oFile.close();

	return 0;

// 例外catcher
}
// -----------------------------------------------------------------------------
// 実行 
// -----------------------------------------------------------------------------
int kgChgnum::run(void)
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
    ((kgChgnum*)arg)->runErrEnd();
}

int kgChgnum::run(int inum,int *i_p,int onum, int* o_p,string &msg)
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

