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
// kgwindow.h 移動窓作成クラス
// =============================================================================
#include <cstdio>
#include <sstream>
#include <vector>
#include <kgwindow.h>
#include <kgError.h>
#include <kgMethod.h>
#include <kgConfig.h>

using namespace std;
using namespace kglib;
using namespace kgmod;

// -----------------------------------------------------------------------------
// コンストラクタ(モジュール名，バージョン登録)
// -----------------------------------------------------------------------------
const char * kgWindow::_ipara[] = {"i",""};
const char * kgWindow::_opara[] = {"o",""};

kgWindow::kgWindow(void)
{
	_name    = "kgwindow";
	_version = "###VERSION###";
	_paralist = "wk=,i=,o=,k=,t=,-n,-r,-q";
	_paraflg = kgArgs::COMMON|kgArgs::IODIFF|kgArgs::NULL_KEY;

	#include <help/en/kgwindowHelp.h>
	_titleL = _title;
	_docL   = _doc;
	#ifdef JPN_FORMAT
		#include <help/jp/kgwindowHelp.h>
	#endif
	_d_stock_ap =NULL;
	_stock_cnt=0;
}
// -----------------------------------------------------------------------------
// 入出力ファイルオープン
// -----------------------------------------------------------------------------
void kgWindow::setArgsMain(void)
{
	_iFile.read_header();

	// num=スライド数
	kgstr_t s = _args.toString("t=",true);
	_interval = atoi(s.c_str());
	if(_interval<=0)		{ _interval = 1; }	

	// k= 項目引数のセット
	vector<kgstr_t> vs = _args.toStringVector("k=",false);

	// wk= 項目引数のセット
	vector< vector<kgstr_t> > vswk = _args.toStringVecVec("wk=",':',2,true);
	vector<kgstr_t> sortvec = vswk[0];
	for(size_t i=0;i<vswk[0].size();i++){
		vector<kgstr_t> spvec = splitToken(vswk[0].at(i),'%');
		vswk[0].at(i) = spvec[0];
	}

	// -n NULL出力フラグ
	_nulout = _args.toBool("-n");

	// -r reverseフラグ
	_reverse = _args.toBool("-r");

	bool seqflg = _args.toBool("-q");
	if(_nfn_i) { seqflg = true; }

	if(!seqflg){ 
		vector<kgstr_t> vsk	= vs;
		vsk.insert(vsk.end(),sortvec.begin(),sortvec.end());
		sortingRun(&_iFile,vsk);
	}

	_kField.set(vs,  &_iFile,_fldByNum);
	_wkField.set(vswk,  &_iFile,_fldByNum);

}
// -----------------------------------------------------------------------------
// 入出力ファイルオープン
// -----------------------------------------------------------------------------
void kgWindow::setArgs(void)
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
void kgWindow::setArgs(int inum,int *i_p,int onum ,int *o_p)
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
// 出力
// s_pos:開始行,e_pos:終了行,nkpos:nk行
// -----------------------------------------------------------------------------
void kgWindow::output(int s_pos,int e_pos, int nkpos) 
{
	int tpos = s_pos;
	do {
		for(unsigned int i=0;i<_wkField.size();i++){
			*( _o_stock_ap.get() + i) = _d_stock_ap[nkpos*_iFile.fldSize()+_wkField.num(i)];
		}
		for(unsigned int i=0;i<_iFile.fldSize();i++){
			*( _o_stock_ap.get() + _wkField.size() + i) = _d_stock_ap[tpos*_iFile.fldSize()+i];
		}
		_oFile.writeFld(_iFile.fldSize()+_wkField.size(),_o_stock_ap.get());	

	} while( (tpos=pos_inc(tpos)) != e_pos );
}
// -----------------------------------------------------------------------------
// 実行
// -----------------------------------------------------------------------------
int kgWindow::runMain(void)
{

	// 入力ファイルにkey項目番号をセットする．
	_iFile.setKey(_kField.getNum());

	// 項目名出力
	if( !_oFile.noFldName( ) ){
		vector<kgstr_t> outfld;
		for(string::size_type i=0; i<_wkField.size(); i++){
			kgstr_t oName;
			oName=_wkField.attr(i);                    
			if( oName.empty()) oName=_wkField.name(i);
			oName.append(_iFile.sortParaStr(_wkField.num(i)));
			outfld.push_back( oName );
		}
		for(size_t i=0; i<_iFile.fldSize(); i++){
			if(_kField.flg(i)==-1){ outfld.push_back( _iFile.fldName(i)); }
			else{ outfld.push_back(_iFile.fldName(i,true)); }
		}
		_oFile.writeFldNameCHK(outfld);
	}

	// データストック領域確保:項目数*(num)分
	_stock_cnt = _iFile.fldSize()*(_interval);
	try{
		_d_stock_ap = new char*[_stock_cnt];
	} catch(...) {
		_d_stock_ap=NULL;
		throw kgError("memory allocation error ");
	}	
	for(unsigned int i=0;i<_stock_cnt;i++){
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


	// 既存OUTOUT領域確保:f=項目数分+_wkFildサイズ
	_o_stock_ap.set( new char*[_iFile.fldSize()+_wkField.size()] );

	// データ格納位置セット
	int pos=0;
	bool full=false;

	// データ集計＆出力
	while(_iFile.read()!=EOF){

		/*一行目読み込み時は何もしない*/
		if(( _iFile.status() & kgCSV::Begin )){continue;}

		// oldデータをセット
		for(unsigned int i=0;i<_iFile.fldSize();i++){
			strcpy(_d_stock_ap[pos*_iFile.fldSize()+i],_iFile.getOldVal(i));
		}
		pos = pos_inc(pos);

		if(pos==0){ full=true; }

		//出力領域セット＆出力
		if(full){
			int nkpos=pos;
			if(!_reverse){ nkpos=pos_dec(pos);}
			int tpos = pos;
			output(tpos,pos,nkpos);
		}
		else if(_nulout&&!_reverse){
			int nkpos=pos_dec(pos);
			int tpos =0;
			output(tpos,pos,nkpos);
		}

		// キーブレイク
		if( _iFile.keybreak() ){
			if(_nulout&&_reverse){
				int tpos = pos_inc(pos);
				if(!full){tpos=0;}
				for(;tpos!=pos;tpos=pos_inc(tpos)){
					output(tpos,pos,tpos);
				}
			}
			/*ENDなら終了*/
			if((_iFile.status() & kgCSV::End )) break;
			pos=0;
			full=false;
		}
	}

	//ASSERT keynull_CHECK
	if(_assertNullKEY) { _existNullKEY = _iFile.keynull(); }

	// 終了処理
	th_cancel();
	_iFile.close();
	_oFile.close();

	return 0;

}
// -----------------------------------------------------------------------------
// 実行 
// -----------------------------------------------------------------------------
int kgWindow::run(void)
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
    ((kgWindow*)arg)->runErrEnd();
}

int kgWindow::run(int inum,int *i_p,int onum, int* o_p,string &msg)
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
		sts=0;

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

