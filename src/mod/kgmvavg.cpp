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
// kgmvavg.h 移動平均クラス
// =============================================================================
#include <cstdio>
#include <iostream>
#include <sstream>
#include <string>
#include <kgmvavg.h>
#include <kgError.h>
#include <kgConfig.h>

using namespace std;
using namespace kglib;
using namespace kgmod;
// -----------------------------------------------------------------------------
// コンストラクタ(モジュール名，バージョン登録)
// -----------------------------------------------------------------------------
const char * kgMvavg::_ipara[] = {"i",""};
const char * kgMvavg::_opara[] = {"o",""};

kgMvavg::kgMvavg(void)
{
	_name    = "kgmvavg";
	_version = "###VERSION###";
	_paralist = "f=,i=,o=,k=,t=,-w,-exp,alpha=,skip=,s=,-n,-q";
	_paraflg = kgArgs::ALLPARAM;

	#include <help/en/kgmvavgHelp.h>
	_titleL = _title;
	_docL   = _doc;
	#ifdef JPN_FORMAT
		#include <help/jp/kgmvavgHelp.h>
	#endif

}

// -----------------------------------------------------------------------------
// 入出力ファイルオープン
// -----------------------------------------------------------------------------
void kgMvavg::setArgsMain(void)
{
	_iFile.read_header();
	_oFile.setPrecision(_precision);

	// f= 項目引数のセット
	vector< vector<kgstr_t> > vvs = _args.toStringVecVec("f=",':',2,true,true);

	// k= 項目引数のセット
	vector<kgstr_t> vs = _args.toStringVector("k=",false);

	// -weighted ,-exp の値をセット
	_weighted = _args.toBool("-w");
	_exp = _args.toBool("-exp");
	if(_weighted && _exp){
		throw kgError("-w and -exp are exclusive options");
	}
	_simple = (!_weighted && !_exp) ;

	// t= alpha=  の値をセット
	kgstr_t strT=_args.toString("t=",false);
	string strA=_args.toString("alpha=",false);

	if(strT.empty()&&(_simple || _weighted)){
		throw kgError("t= must be specified");
	}
	if(strA.empty()&&strT.empty()){
		throw kgError("t= or alpha= must be specified with -exp");
	}
	if(!strA.empty()&&!_exp){
		throw kgError("alpha= can be specified only with -exp");
	}
	if(_exp){
		if(!strA.empty())	{ _alpha = atof(strA.c_str());}
		else							{ _alpha = 2.0/(atof(strT.c_str())+1); }
		_term=1;
	}
	else{
		_term = atoi(strT.c_str());
	}
	// skip= の値をセット
	string strS=_args.toString("skip=",false);
	if(strS.empty()){
		if(_exp){ _skip = 0; }
		else		{ _skip = _term - 1; }
	}else{
		_skip = atoi(strS.c_str());
	}

	// -n オプションのセット
	_null=_args.toBool("-n");

	bool seqflg = _args.toBool("-q");
	if(_nfn_i) { seqflg = true; }

	vector<kgstr_t> vss = _args.toStringVector("s=",false);
	if(!seqflg&&vss.empty()){
		throw kgError("parameter s= is mandatory without -q,-nfn");
	}

	if(!seqflg && (!vs.empty()||!vss.empty())){ 
		vector<kgstr_t> vsk	= vs;
		vsk.insert(vsk.end(),vss.begin(),vss.end());
		sortingRun(&_iFile,vsk);
	}
	
	_kField.set(vs,  &_iFile,_fldByNum);
	_fField.set(vvs, &_iFile,_fldByNum);

}
// -----------------------------------------------------------------------------
// 入出力ファイルオープン
// -----------------------------------------------------------------------------
void kgMvavg::setArgs(int inum,int *i_p,int onum ,int *o_p)
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
// パラメータチェック 入出力ファイルオープン
// -----------------------------------------------------------------------------
void kgMvavg::setArgs(void)
{
	_args.paramcheck(_paralist,_paraflg);

	// 入出力ファイルオープン
	_iFile.open(_args.toString("i=",false), _env,_nfn_i);
  _oFile.open(_args.toString("o=",false), _env,_nfn_o);

	setArgsMain();

}
// -----------------------------------------------------------------------------
// 移動平均
// -----------------------------------------------------------------------------
void kgMvavg::simpleMA()
{
	vector<double> sumV(_fField.size() ,0);
	size_t pointer=0;
	//bool   out    = false;
	size_t recNo=0;

	// データ集計＆出力
	while(_iFile.read()!=EOF){
		recNo++;
		//keybreakしたら出力
		if( _iFile.keybreak() ){
			for(size_t i=0; i<_val.size(); i++){
				_val.at(i)=0;
				_nullv.at(i)=true;
			}
			pointer=0;
			//out    = false;
			recNo=1;
			for(std::size_t i=0; i<_fField.size(); i++){
				sumV.at(i) = 0;
				_cnt.at(i) = 0;
			}
			//ENDなら終了
			if((_iFile.status() & kgCSV::End )) break;
		}


		// データ更新
		size_t cell=pointer*_fField.size();
		for(vector<kgstr_t>::size_type i=0; i<_fField.size(); i++){
			if(!_nullv.at(cell+i)){
				sumV.at(i) -= _val.at(cell+i);
				_cnt.at(i)--;
			}
			char* str=_iFile.getNewVal(_fField.num(i));
			if(*str!='\0') { 
				_val.at(cell+i)=atof(str);
				_nullv.at(cell+i)=false;
				sumV.at(i)+=_val.at(cell+i);
				_cnt.at(i)++;
			}
			else{
				if(_assertNullIN) { _existNullIN  = true;}
				_val.at(cell+i)=0; 
				_nullv.at(cell+i)=true;
			}
		}

		//--------------------------------------------
		// 結果出力
		// -------------------------------------------
		if(recNo>_skip){ 
			for(vector<kgstr_t>::size_type i=0; i<_fField.size(); i++){
				if(_null){ 
					if(_cnt.at(i)==_term){ _avg.at(i).r( sumV.at(i)/_cnt.at(i) );}
					else                 { 
						_avg.at(i).null(true);
						if(_assertNullOUT){ _existNullOUT = true;}
					}
				}else{
					if(_cnt.at(i) > 0){ _avg.at(i).r( sumV.at(i)/_cnt.at(i) );}
					else{
						_avg.at(i).null(true);
						if(_assertNullOUT){ _existNullOUT = true;}
					}
				}
			}
			_oFile.writeFld(_iFile.getNewFld(),_fField.getFlg_p(),_avg);
		}


		//--------------------------------------------
		// FIFOキューのポインタを更新
		// FIFOキューのポインタの循環判定処理
		// 指定期間(t=)到達判定
		pointer++;
		if(pointer >= _term){ pointer = 0; }

	}
}
// ================================================================================
// 線形加重移動平均
// ================================================================================
void kgMvavg::weightedMA()
{
	vector<double> sumV(_fField.size() ,0);
	vector<double> numV(_fField.size() ,0);

	// _valをFIFOキューと見立てたときのデキュー
	size_t pointer=0;
	//bool   filled = false;
	//bool   out    = false;

	size_t recNo=0;

	// データ集計＆出力
	while(_iFile.read()!=EOF){
		recNo++;
		//keybreakしたら出力
		if( _iFile.keybreak() ){
			for(size_t i=0; i<_val.size(); i++){
				_val.at(i)=0;
				_nullv.at(i)=true;
			}
			pointer=0;
			//filled = false;
			//out    = false;
			recNo=1;
			for(std::size_t i=0; i<_fField.size(); i++){
				sumV.at(i) = 0;
				numV.at(i) = 0;
				_cnt.at(i) = 0;
			}
			//ENDなら終了
			if((_iFile.status() & kgCSV::End )) break;
		}
		// 更新位置セット
		size_t cell=pointer*_fField.size();
		
		for(vector<kgstr_t>::size_type i=0; i<_fField.size(); i++){
			if(!_nullv.at(cell+i)){
				numV.at(i) -= sumV.at(i);
				sumV.at(i) -= _val.at(cell+i);
				_cnt.at(i)--;
			}
			char* str=_iFile.getNewVal(_fField.num(i));
			if(*str!='\0'){
				_val.at(cell+i)=atof(str);
				_nullv.at(cell+i)=false;
				_cnt.at(i)++;
			}else{
				if(_assertNullIN) { _existNullIN  = true;}
				_val.at(cell+i)=0;
				_nullv.at(cell+i)=true;
			}
			numV.at(i) = numV.at(i) + _val.at(cell+i)*_cnt.at(i);
			sumV.at(i) += _val.at(cell+i);
		}

		//--------------------------------------------
		// FIFOキューのポインタを更新
		//--------------------------------------------
		pointer++;

		//--------------------------------------------
		// FIFOキューのポインタの循環判定処理
		// 指定期間(t=)到達判定
		//--------------------------------------------
		if(pointer >= _term){ pointer = 0; }

		// -------------------------------------------
		// 出力処理
		//--------------------------------------------
		// 結果出力フラグの更新
		//--------------------------------------------
		if(recNo>_skip){ 
			for(std::size_t i=0; i<_fField.size(); i++){

				if(_null){ 
					if(_cnt.at(i)==_term){ _avg.at(i).r( numV.at(i)/((_cnt.at(i)+1)*_cnt.at(i)/2));}
					else{ 
						if(_assertNullOUT){ _existNullOUT = true;}
						_avg.at(i).null(true);
					}
				}else{
					if(_cnt.at(i) > 0){ _avg.at(i).r( numV.at(i)/((_cnt.at(i)+1)*_cnt.at(i)/2));}
					else{ 
						if(_assertNullOUT){ _existNullOUT = true;}
						_avg.at(i).null(true);
					}
				}
			}
			_oFile.writeFld(_iFile.getNewFld(),_fField.getFlg_p(),_avg);
		}
	}

}
// ================================================================================
// 指数平滑移動平均
// ================================================================================
// -----------------------------------------------------------------------------
// 指数平滑移動平均出力
// -----------------------------------------------------------------------------
void kgMvavg::expMA()
{
	// 初期化
	for(size_t i=0; i<_avg.size(); i++){ _avg.at(i).type('N');}
	size_t recNo  =0;

	// データ集計＆出力
	while(_iFile.read()!=EOF){
		recNo++;

		//keybreakしたら出力
		if( _iFile.keybreak() ){
			for(size_t i=0; i< _val.size(); i++){
				_val.at(i)=0;
				_nullv.at(i)=true;
				_cnt.at(i) = 0;
				_avg.at(i).type('N');
			}
			recNo  =1;
			//ENDなら終了
			if((_iFile.status() & kgCSV::End )) break;
		}
		// ------------------------------------------
		// データの値セット
		// ------------------------------------------
		for(vector<kgstr_t>::size_type i=0; i<_fField.size(); i++){
			char* str=_iFile.getNewVal(_fField.num(i));
			if(*str!='\0'){
				_val.at(i)=atof(str);
				_nullv.at(i)=false;
				_cnt.at(i)++;
			}else{
				if(_assertNullIN) { _existNullIN  = true;}
				_val.at(i)=0;
				_nullv.at(i)=true;
			}
		}
		// -----------------------------------------
		// avgの更新
		// -----------------------------------------
		for(vector<kgstr_t>::size_type i=0; i<_fField.size(); i++){

			if(_null && _cnt.at(i)!=recNo){ 
				_avg.at(i).null(true); 
				if(_assertNullOUT ){ _existNullOUT = true;}
			}
			else if(!_nullv.at(i)){
				if (_avg.at(i).null()){ 
					_avg.at(i).r(_val.at(i));
				}
				else{
					_avg.at(i).r( _alpha * _val.at(i) + (1.0 - _alpha) * _avg.at(i).r());
				}
			}
		}
		//--------------------------------------------
		// 出力処理
		//--------------------------------------------
		if(recNo>_skip) {
			_oFile.writeFld(_iFile.getNewFld(),_fField.getFlg_p(),_avg);
		}

	}
}

// -----------------------------------------------------------------------------
// 実行
// -----------------------------------------------------------------------------
int kgMvavg::runMain(void)
{

	// 入力ファイルにkey項目番号をセットする．
	_iFile.setKey(_kField.getNum());

	// 項目名の出力
  _oFile.writeFldName(_fField, true);

	// 集計用変数領域確保＆初期化
	_val.resize(_fField.size()*_term ,0);     //データ領域
	_nullv.resize(_fField.size()*_term ,true);//NULLデータ領域
	_cnt.resize(_fField.size() ,0);
	_avg.resize(_fField.size() ,kgVal('N'));
	if(_simple)				 { simpleMA();}
	else if(_weighted) { weightedMA();}
	else							 { expMA();}

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
int kgMvavg::run(void) 
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
    ((kgMvavg*)arg)->runErrEnd();
}


int kgMvavg::run(int inum,int *i_p,int onum, int* o_p,string &msg)
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
