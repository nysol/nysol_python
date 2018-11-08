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
// kgslide.cpp 前行データ項目の追加クラス
// =============================================================================
#include <cstdio>
#include <sstream>
#include <vector>
#include <kgslide.h>
#include <kgError.h>
#include <kgConfig.h>

using namespace std;
using namespace kglib;
using namespace kgmod;

// -----------------------------------------------------------------------------
// コンストラクタ(モジュール名，バージョン登録)
// -----------------------------------------------------------------------------
const char * kgSlide::_ipara[] = {"i",""};
const char * kgSlide::_opara[] = {"o",""};

kgSlide::kgSlide(void)
{
	_name    = "kgslide";
	_version = "###VERSION###";

	_paralist = "f=,i=,o=,k=,-n,-r,t=,-l,s=,-q";
	_paraflg = kgArgs::ALLPARAM;

	#include <help/en/kgslideHelp.h>
	_titleL = _title;
	_docL   = _doc;
	#ifdef JPN_FORMAT
		#include <help/jp/kgslideHelp.h>
	#endif
	_d_stock_ap = NULL;
	_stock_cnt =0;
}

// -----------------------------------------------------------------------------
// パラメータセット
// -----------------------------------------------------------------------------
void kgSlide::setArgsMain(void){

	_iFile.read_header();
	// t=スライド数
	kgstr_t s = _args.toString("t=",false);
	if(s.empty())	{ _interval = 1; }
	else        	{ _interval = atoi(s.c_str());}
	if(_interval<=0)		{ _interval = 1; }	

	// f= 項目引数のセット
	vector< vector<kgstr_t> > vvs = _args.toStringVecVec("f=",':',2,true);

	// k= 項目引数のセット
	vector<kgstr_t> vs = _args.toStringVector("k=",false);

	// -n 最終行NULL出力フラグ
	_nulout = _args.toBool("-n");

	// -r 逆順出力フラグ
	_reverse = _args.toBool("-r");

	// -l 最終行フラグ
	_last  = _args.toBool("-l");

	bool seqflg = _args.toBool("-q");
	if(_nfn_i) { seqflg = true; }
	vector<kgstr_t> vss = _args.toStringVector("s=",false);
	if(!seqflg&&vss.empty()){
		throw kgError("parameter s= is mandatory without -q , -nfn");
	}
	
	if(!seqflg && (!vs.empty()||!vss.empty())){ 
		vector<kgstr_t> vsk	= vs;
		vsk.insert(vsk.end(),vss.begin(),vss.end());
		sortingRun(&_iFile,vsk);
	}

	_kField.set(vs,  &_iFile,_fldByNum);
	_fField.set(vvs, &_iFile,_fldByNum);

}

void kgSlide::setArgs(void)
{
	// パラメータチェック
	_args.paramcheck(_paralist,_paraflg);

	// 入出力ファイルオープン
	_iFile.open(_args.toString("i=",false), _env,_nfn_i);
	_oFile.open(_args.toString("o=",false), _env,_nfn_o);

	setArgsMain();

}

// -----------------------------------------------------------------------------
// 入出力ファイルオープン
// -----------------------------------------------------------------------------
void kgSlide::setArgs(int inum,int *i_p,int onum ,int *o_p)
{
	int iopencnt = 0;
	int oopencnt = 0;
	try{
		// パラメータチェック
		_args.paramcheck(_paralist,_paraflg);

		if(inum>1 || onum>1){
			throw kgError("no match IO");
		}

		// 入出力ファイルオープン
		if(inum==1 && *i_p > 0){ _iFile.popen(*i_p, _env,_nfn_i); }
		else     { _iFile.open(_args.toString("i=",true), _env,_nfn_i); }
		iopencnt++;
	
		if(onum==1 && *o_p > 0){ _oFile.popen(*o_p, _env,_nfn_o); }
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
// 出力
// -----------------------------------------------------------------------------
void kgSlide::output(int s_pos,int e_pos) 
{
	//スライド項目数セット
	int outfld = _interval;
	if(_last) { outfld = 1; } 

	// 既存データ
	for(unsigned int i=0;i<_iFile.fldSize();i++){
		*( _o_stock_ap.get() + i) = _d_stock_ap[s_pos*_iFile.fldSize()+i];
	}

	// 追加データ
	for(unsigned int i=0;i<_fField.size();i++){
		int fnum = _fField.num(i);
		int cnt =0;
		if(_last){
			if(s_pos == e_pos){
				*( _onew_stock_ap.get()+i) = _d_stock_ap[pos_proceed(s_pos,!_reverse)*_iFile.fldSize()+fnum] ;			
				if(_assertNullIN && **(_onew_stock_ap.get()+i)=='\0' ) { _existNullIN  = true;}
			}
			else{
				*( _onew_stock_ap.get()+i) = const_cast<char*>("");
				if(_assertNullOUT){ _existNullOUT = true;}
			}
		}
		else{
			for(int tpos=pos_proceed(s_pos,_reverse); tpos!=e_pos; tpos=pos_proceed(tpos,_reverse),cnt++){
				*( _onew_stock_ap.get() + i*_interval + cnt ) = _d_stock_ap[tpos*_iFile.fldSize()+fnum];
				if(_assertNullIN && **( _onew_stock_ap.get() + i*_interval + cnt ) =='\0' ) { _existNullIN  = true;}
			}
			for(;cnt<_interval;cnt++){
				*( _onew_stock_ap.get() + i*_interval + cnt ) = const_cast<char*>("");
				if(_assertNullOUT){ _existNullOUT = true;}
			}
		}
	}
	_oFile.writeFld(_o_stock_ap.get(),_iFile.fldSize(),_onew_stock_ap.get(),outfld*_fField.size());	

}

// -----------------------------------------------------------------------------
// runMain
// -----------------------------------------------------------------------------
int kgSlide::runMain()
{
	// 入力ファイルにkey項目番号をセットする．
	_iFile.setKey(_kField.getNum());

	//field項目数セット
	vector<int> fField =_fField.getNum();

	// 項目名の展開
	vector<kgstr_t> outfld;
	for(unsigned int i=0; i<_fField.size();i++){

		if(_last){
			ostringstream os;
			if(!_fField.attr(i).empty()){ os << _fField.attr(i) ;}
			else 												{ os << _fField.name(i) << _interval;}
			outfld.push_back(os.str());
		}
		else{
			kgstr_t oName = _fField.attr(i);
			if( oName.empty()) oName=_fField.name(i);
			for(int j=0;j<_interval;j++){
				ostringstream os;
				if(_interval==1&&!_fField.attr(i).empty()){ os << oName ;}
				else 																			{ os << oName << j+1;}
				outfld.push_back(os.str());
			}
		}
	}

	_oFile.writeFldName(_iFile,outfld);

	// データストック領域確保:項目数*(num+1)分
	_stock_cnt = _iFile.fldSize()*(_interval+1);
	try{
		_d_stock_ap = new char*[_iFile.fldSize()*(_interval+1)];
	} catch(...) {
		_d_stock_ap=NULL;
		throw kgError("memory allocation error ");
	}
	
	for(unsigned int i=0;i<_iFile.fldSize()*(_interval+1);i++){
		try {
			_d_stock_ap[i] = new char[KG_MAX_STR_LEN];
		} catch(...) {
			for(size_t j=0;j<i;j++){
				delete [] _d_stock_ap[j];
			}
			delete [] _d_stock_ap;
			_d_stock_ap = NULL;
			throw kgError("memory allocation error ");
		}
	}	

	// 既存OUTOUT領域確保:f=項目数分
	_o_stock_ap.set( new char*[_iFile.fldSize()] );
	// 追加OUTOUT領域確保:f=項目数*num分
	_onew_stock_ap.set( new char*[_fField.size()*_interval] );

	// データ格納位置セット
	int pos=0;
	bool full=false;

	// データ集計＆出力
	while(_iFile.read()!=EOF){

		//一行目読み込み時は何もしない
		if(( _iFile.status() & kgCSV::Begin )){continue;}

		// oldデータをセット
		for(unsigned int i=0;i<_iFile.fldSize();i++){
			strcpy(_d_stock_ap[(pos*_iFile.fldSize()+i)],_iFile.getOldVal(i));
		}

		pos = pos_proceed(pos);
		if(pos==0){ full=true; }

		//出力領域セット＆出力
		if(full){

			if(_reverse){
				int tpos = pos_proceed(pos,_reverse);
				output(tpos,tpos);
			}
			else{ output(pos,pos);}

		}
		else if(_nulout&&_reverse){

			int spos=pos_proceed(pos,_reverse);
			int epos =pos_proceed(0,_reverse);;
			output(spos,epos);

		}

		// キーブレイク
		if( _iFile.keybreak() ){
			if(_nulout&&!_reverse){

				int spos = pos_proceed(pos);
				int epos = pos;
				if(!full){spos=0;}				
				for(;spos!=epos;spos=pos_proceed(spos)){
					output(spos,epos);
				}
			}

			// ENDなら終了
			if((_iFile.status() & kgCSV::End )) break;

			pos=0;
			full=false;
		}
	}
	//ASSERT keynull_CHECK
	if(_assertNullKEY) { _existNullKEY = _iFile.keynull(); }

	// 終了処理(メッセージ出力,thread pipe終了通知)
	th_cancel();
	_iFile.close();
	_oFile.close();

	return 0;

}
// -----------------------------------------------------------------------------
// 実行
// -----------------------------------------------------------------------------
int kgSlide::run(void) 
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
    ((kgSlide*)arg)->runErrEnd();
}

int kgSlide::run(int inum,int *i_p,int onum, int* o_p,string &msg)
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


