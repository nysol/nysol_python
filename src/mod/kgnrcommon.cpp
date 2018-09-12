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
// kgnrcommon.h 参照ファイルによる数値範囲マッチング行選択クラス
// =============================================================================
#include <sstream>
#include <vector>
#include <kgnrcommon.h>
#include <kgError.h>
#include <kgRange.h>
#include <kgConfig.h>

using namespace std;
using namespace kglib;
using namespace kgmod;

// -----------------------------------------------------------------------------
// コンストラクタ(モジュール名，バージョン登録)
// -----------------------------------------------------------------------------
const char * kgNrcommon::_ipara[] = {"i","m",""};
const char * kgNrcommon::_opara[] = {"o","u",""};

kgNrcommon::kgNrcommon(void)
{
	_name    = "kgnrcommon";
	_version = "###VERSION###";

	_paralist = "i=,o=,m=,k=,K=,rf=,R=,u=,-r,-q";
	_paraflg = kgArgs::COMMON|kgArgs::IODIFF|kgArgs::NULL_KEY;

	#include <help/en/kgnrcommonHelp.h>
	_titleL = _title;
	_docL   = _doc;
	#ifdef JPN_FORMAT
		#include <help/jp/kgnrcommonHelp.h>
	#endif

}
// -----------------------------------------------------------------------------
// パラメータセット＆入出力ファイルオープン
// -----------------------------------------------------------------------------
void kgNrcommon::setArgsMain(void)
{
	_iFile.read_header();
	_mFile.read_header();

	// k= 項目引数のセット
	vector<kgstr_t> vs_k = _args.toStringVector("k=",false);

	// K= 項目引数のセット
	// K=の指定がなければk=の値をセットする
	vector<kgstr_t> vs_K = _args.toStringVector("K=",false);
	if(vs_K.empty()){ vs_K=vs_k; }

	// k=とK=の数があっている化チェック
	if(vs_k.size()!=vs_K.size()){
		ostringstream ss;
		ss << "not match keyfield size k:" << _kField.size() << " K:" << _KField.size() ;
		throw kgError(ss.str());
	}

	// v= 項目引数のセット(指定するのは１項目)
	vector< vector<kgstr_t> >  vvs_rs = _args.toStringVecVec("rf=","%:",2,true);
	// R= 項目引数のセット(指定するのは２項目)
	vector< vector<kgstr_t> >  vvs_r = _args.toStringVecVec("R=","%:",2,true);

	bool seqflg = _args.toBool("-q");
	if(_nfn_i) { seqflg = true; }

	if(!seqflg && !vs_k.empty()){ 
		vector<kgCSVfld*> csv_p;  
		vector< vector<kgstr_t> > fld_ary;  
		csv_p.push_back(&_iFile);
		csv_p.push_back(&_mFile);
		fld_ary.push_back(vs_k);
		fld_ary.push_back(vs_K);
		sortingRun(csv_p,fld_ary);
	}

	_kField.set(vs_k, &_iFile,_fldByNum);
	_KField.set(vs_K, &_mFile,_fldByNum);
	_rField.set(vvs_rs, &_iFile,_fldByNum);
	_RField.set(vvs_r, &_mFile,_fldByNum);

	//比較タイプセット(nがあるとtrueをセット(数字ソートになる))
	if( _rField.attr(0).find("n") == kgstr_t::npos){
		_cmp_type = false;
	}else{
		_cmp_type = true;		
	}

	//範囲タイプセット(equalを含むときは場合ture)
	if( _RField.attr(0).find("n") == kgstr_t::npos){
		_range1_type = true;
	}else{
		_range1_type = false;		
	}
	if( _RField.attr(1).find("e") == kgstr_t::npos){
		_range2_type = false;
	}else{
		_range2_type = true;		
	}


	if(_rField.size()!=1){
		ostringstream ss;
		ss << "r= has 1 item : r= size" << _rField.size() ;
		throw kgError(ss.str());
	}
	if(_RField.size()!=2){
		ostringstream ss;
		ss << "R= has 2 item : R= size:" << vvs_r.size() ;
		throw kgError(ss.str());
	}

}

// -----------------------------------------------------------------------------
// パラメータセット＆入出力ファイルオープン
// -----------------------------------------------------------------------------
void kgNrcommon::setArgs(void)
{
	// パラメータチェック
	_args.paramcheck("i=,o=,m=,k=,K=,r=,R=,u=,-r,-q",kgArgs::COMMON|kgArgs::IODIFF|kgArgs::NULL_KEY);
	// -r 条件反転
	_reverse = _args.toBool("-r");

	// 入出力ファイルオープン
	kgstr_t ifile = _args.toString("i=",false);
	kgstr_t mfile = _args.toString("m=",false);
	if(ifile.empty() && mfile.empty()){
		throw kgError("Either i= or m= must be specified.");
	}
	_iFile.open(ifile,_env,_nfn_i);
	_mFile.open(mfile,_env,_nfn_i);
	_oFile.open(_args.toString("o=",false),_env,_nfn_o);
	kgstr_t ufile = _args.toString("u=",false);
	if(ufile.empty()){ _elsefile=false; }
	else {
		_elsefile=true;
		_uFile.open(ufile,_env,_nfn_o);
	}
	setArgsMain();
}
// -----------------------------------------------------------------------------
// パラメータセット＆入出力ファイルオープン
// -----------------------------------------------------------------------------
void kgNrcommon::setArgs(int inum,int *i_p,int onum, int* o_p)
{
	int iopencnt = 0;
	int oopencnt = 0;
	try{
		// パラメータチェック
		_args.paramcheck(_paralist,_paraflg);
		// -r 条件反転
		_reverse = _args.toBool("-r");

		if(inum>2 || onum>2){ throw kgError("no match IO");}

		// 入出力ファイルオープン
		kgstr_t ifile = _args.toString("i=",false);
		kgstr_t mfile = _args.toString("m=",false);

		int i_p_t = -1;
		int m_p_t = -1;
		if(inum>0){ i_p_t = *i_p;     }
		if(inum>1){ m_p_t = *(i_p+1); }


		if((ifile.empty()&&i_p_t<=0) && ( mfile.empty()&&m_p_t<=0)){
			throw kgError("Either i= or m= must be specified.");
		}
		if(i_p_t>0){ _iFile.popen(i_p_t, _env,_nfn_i); }
		else if( ifile.empty()){ 
			throw kgError("i= is necessary");
		}
		else       { _iFile.open(ifile, _env,_nfn_i);}	
		iopencnt++;

		if(m_p_t>0){ _mFile.popen(m_p_t, _env,_nfn_i); }
		else if( mfile.empty()){ 
			throw kgError("m= is necessary");
		}
		else       { _mFile.open(mfile, _env,_nfn_i);}
		iopencnt++;

		int o_p_t = -1;
		int u_p_t = -1;
		if(onum>0){ o_p_t = *o_p;     }
		if(onum>1){ u_p_t = *(o_p+1); }
		kgstr_t ufile = _args.toString("u=",false);
		kgstr_t ofile = _args.toString("o=",false);

		if(o_p_t==-1 && ofile.empty()){
			if(u_p_t!=-1 || !ufile.empty()){
				_reverse = !_reverse;
				kgstr_t swptmp;
				swptmp = ufile ; ufile = ofile;  ofile = swptmp;
				int swptmpi;
				swptmpi = u_p_t ; u_p_t = o_p_t ; o_p_t = swptmpi;
			}
		}

		if(o_p_t>0){ _oFile.popen(o_p_t, _env,_nfn_o);}
		else if( ofile.empty()){ 
			throw kgError("o= is necessary");
		}
		else       { _oFile.open(ofile, _env,_nfn_o);}
		oopencnt++;

		if(u_p_t>0){ 
			_elsefile=true;
			_oFile.popen(u_p_t, _env,_nfn_o);
		}
		else if(ufile.empty()){
			_elsefile=false; 
		}
		else{
			_elsefile=true;
			_uFile.open(ufile,_env,_nfn_o);
		}		
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
// rangeと比較してマッチすれば出力する(キーが一致した場合に実行)
// -----------------------------------------------------------------------------
void kgNrcommon::range_match(void)
{
	// 参照ファイルからrange一覧を作成
	kgRange range;
	range.set(_cmp_type,_range1_type,_range2_type);
	while(1){
		range.set_table(_mFile.getNewVal(_RField.num(0)),_mFile.getNewVal(_RField.num(1)),NULL,0);	
		_mFile.read();
		if( _mFile.keybreak() ){break;}
	}

	// rangeからマッチするものを探して出力
	// (-rが指定されていた場合、出力するものを逆転させる)
	while(1){
		if(*(_iFile.getNewVal(_rField.num(0)))=='\0'){//NULLの時はアンマッチ
			if(_reverse){
				_oFile.writeFld(_iFile.fldSize(),_iFile.getNewFld());
			}
			else if(_elsefile==true){
				_uFile.writeFld(_iFile.fldSize(),_iFile.getNewFld());
			}
		}
		else{
			bool match = range.serch(_iFile.getNewVal(_rField.num(0)));
			if(match!=_reverse){
				_oFile.writeFld(_iFile.fldSize(),_iFile.getNewFld());				
			}else if(_elsefile==true){
				_uFile.writeFld(_iFile.fldSize(),_iFile.getNewFld());
			}
		}
		_iFile.read();
		if( _iFile.keybreak() ){break;}
	}	
}
// -----------------------------------------------------------------------------
// 実行
// -----------------------------------------------------------------------------
int kgNrcommon::runMain(void)
{
	// 入力、参照ファイルにkey項目番号をセットする．
	_iFile.setKey(_kField.getNum());
	_mFile.setKey(_KField.getNum());

	// 項目名出力
	_oFile.writeFldName(_iFile);
	if(_elsefile){ _uFile.writeFldName(_iFile); }

	// keyサイズとデータセット
	int ksize = _kField.size();
	vector<int> kField =_kField.getNum();
	vector<int> KField =_KField.getNum();

	//比較結果用フラグ
	int cmpflg=0;

	//一行目読み込み
	_iFile.read();
	_mFile.read();

	// k=,K=でマッチするところを検索し.その後v=,r=で比較マッチをする
  // k,Kはソートされていることが前提
	while( 1 ){
		if((_iFile.status() & kgCSV::End )||
				(_mFile.status() & kgCSV::End )){ break;}
		for(int i=0;i<ksize;i++){
			cmpflg = strcmp( _iFile.getNewVal(kField[i]), _mFile.getNewVal(KField[i]) );
			if(cmpflg!=0){break;}
			//両方共NULLならアンマッチとする
			if( *(_iFile.getNewVal(kField[i]))=='\0' &&
				*(_mFile.getNewVal(KField[i]))=='\0')
			{
				cmpflg=1;
				break;
			}
		}
		// キー一致したので範囲検索へ
		if(cmpflg==0){ range_match(); }
		//参照ファイルの方が小さい(参照の次の行を読み込む)
		else if(cmpflg>0){
			while(1){
				_mFile.read();
				if( _mFile.keybreak() ){break;}
			}
		}
		//参照ファイルの方が大きくなった(u=が指定されていればno_matchとして書き出す)*/
		else{
			if(_reverse){
				_oFile.writeFld(_iFile.fldSize(),_iFile.getNewFld());
			}
			else if(_elsefile==true){
				_uFile.writeFld(_iFile.fldSize(),_iFile.getNewFld());
			}
			//次の行読み込み
			_iFile.read();
		}
	}
	//入力ファイルのデータが残っていた場合のデータ出力
	if(!(_iFile.status() & kgCSV::End )){
		if(_reverse){
			while(1){
				_oFile.writeFld(_iFile.fldSize(),_iFile.getNewFld());
				_iFile.read();
 				if(_iFile.status() & kgCSV::End){ break;} 
			}
		}
		else if(_elsefile==true){
			while(1){
				_uFile.writeFld(_iFile.fldSize(),_iFile.getNewFld());
				_iFile.read();
 				if(_iFile.status() & kgCSV::End){ break;} 
			}
		}
	}
	//ソートスレッドを終了させて、終了確認
	//for(size_t i=0 ;i<_th_st.size();i++){ pthread_cancel(_th_st[i]->native_handle());	}
	//for(size_t i=0 ;i<_th_st.size();i++){ pthread_join(_th_st[i]->native_handle(),NULL);}

	if(_assertNullKEY) { _existNullKEY = _iFile.keynull()||_mFile.keynull(); }
	// 終了処理
	th_cancel();
	_iFile.close();
	_mFile.close();
	_oFile.close();
	if(_elsefile){ _uFile.close(); }

	return 0;

}
// -----------------------------------------------------------------------------
// 実行 
// -----------------------------------------------------------------------------
int kgNrcommon::run(void) 
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
    ((kgNrcommon*)arg)->runErrEnd();
}

int kgNrcommon::run(int inum,int *i_p,int onum, int* o_p,string &msg)
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


