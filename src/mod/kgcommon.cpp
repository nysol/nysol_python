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
// kgcommon.cpp 参照ファイルによる行選択クラス
// =============================================================================
#include <cstdio>
#include <sstream>
#include <kgcommon.h>
#include <kgError.h>
#include <kgMethod.h>
#include <kgConfig.h>

using namespace std;
using namespace kglib;
using namespace kgmod;

// -----------------------------------------------------------------------------
// コンストラクタ(モジュール名，バージョン登録)
// -----------------------------------------------------------------------------
const char * kgCommon::_ipara[] = {"i","m",""};
const char * kgCommon::_opara[] = {"o","u",""};

kgCommon::kgCommon(void)
{
	_name    = "kgcommon";
	_version = "###VERSION###";
	_paralist = "i=,o=,m=,k=,K=,u=,-r,-q";
	_paraflg = kgArgs::COMMON|kgArgs::IODIFF|kgArgs::NULL_KEY;

	#include <help/en/kgcommonHelp.h>
	_titleL = _title;
	_docL   = _doc;
	#ifdef JPN_FORMAT
		#include <help/jp/kgcommonHelp.h>
	#endif

}

// -----------------------------------------------------------------------------
// パラメータセット
// -----------------------------------------------------------------------------
void kgCommon::setArgsMain(void)
{
	_iFile.read_header();
	_mFile.read_header();

	// k= 項目引数のセット
	vector<kgstr_t> vs_k = _args.toStringVector("k=",true);

	// K= 項目引数のセット
	// K=の指定がなければk=の値をセットする
	vector<kgstr_t> vs_K = _args.toStringVector("K=",false);
	if(vs_K.empty()){ vs_K =vs_k;}

	// k=とK=の数があっている化チェック
	if(vs_K.size()!=vs_k.size()){
		ostringstream ss;
		ss << "not match keyfield size k:" << _kField.size() << " K:" << _KField.size() ;
		throw kgError(ss.str());
	}
	bool seqflg = _args.toBool("-q");
	if(_nfn_i) { seqflg = true; }

	vector<kgCSVfld*> csv_p;  
	vector< vector<kgstr_t> > fld_ary;  
	csv_p.push_back(&_iFile);
	csv_p.push_back(&_mFile);
	fld_ary.push_back(vs_k);
	fld_ary.push_back(vs_K);
	if ( !seqflg ) { sortingRun(csv_p,fld_ary);}

	_kField.set(vs_k, &_iFile, _fldByNum);
	_KField.set(vs_K, &_mFile, _fldByNum);	
	
}
// -----------------------------------------------------------------------------
// 入出力ファイルオープン
// -----------------------------------------------------------------------------
void kgCommon::setArgs(void)
{
	// パラメータチェック
	_args.paramcheck(_paralist,_paraflg);

	// -r 条件反転
	_reverse = _args.toBool("-r");

	// 入出力ファイルオープン
	kgstr_t ifile = _args.toString("i=",false);
	kgstr_t mfile = _args.toString("m=",false);
	kgstr_t ufile = _args.toString("u=",false);
	if(ifile.empty() && mfile.empty()){
		throw kgError("Either i= or m= must be specified.");
	}
	_iFile.open(ifile,_env,_nfn_i);
	_mFile.open(mfile,_env,_nfn_i);
	_oFile.open(_args.toString("o=",false),_env,_nfn_o);
	if(ufile.empty()){ _elsefile=false; }
	else {
		_elsefile=true;
		_uFile.open(ufile,_env,_nfn_o);
	}		
	setArgsMain();
	
}
void kgCommon::setArgs(int inum,int *i_p,int onum ,int *o_p)
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

		if(m_p_t>0){ _mFile.popen(m_p_t, _env,_nfn_i);  }
		else if( mfile.empty()){ 
			ostringstream ss;
			throw kgError("m= is necessary");
		}
		else       { _mFile.open(mfile, _env,_nfn_i);}
		iopencnt++;


		int o_no = -1;
		int u_no = -1;
		if(onum>0){ o_no = *o_p;     }
		if(onum>1){ u_no = *(o_p+1); }
		// 出力チェック
		kgstr_t ofile = _args.toString("o=",false);
		kgstr_t ufile = _args.toString("u=",false);


		if(o_no==-1 && ofile.empty()){
			if(u_no!=-1 || !ufile.empty()){
				_reverse = !_reverse;
				kgstr_t swptmp;
				swptmp = ufile ; ufile = ofile;  ofile = swptmp;
				int swptmpi;
				swptmpi = u_no ; u_no = o_no ; o_no = swptmpi;
			}
		}

		if(o_no>0){ _oFile.popen(o_no, _env,_nfn_o); }
		else if( ofile.empty()){ 
			throw kgError("o= is necessary");
		}
		else       { _oFile.open(ofile, _env,_nfn_o);}
		oopencnt++;

		if(u_no>0){ 
			_elsefile=true;
			_oFile.popen(u_no, _env,_nfn_o);
			oopencnt++;
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
// 実行
// -----------------------------------------------------------------------------
int kgCommon::runMain(void) 
{
	// 項目名出力
	_oFile.writeFldName(_iFile);
	if(_elsefile){ _uFile.writeFldName(_iFile); }

	// keyサイズとデータセット
	int ksize = _kField.size();
	vector<int> kField =_kField.getNum();
	vector<int> KField =_KField.getNum();

	//比較結果用フラグ
	int cmpflg=0;
	bool mstEnd=false;
	bool begin=true;

	// データ出力(入力ファイルあるいは参照ファイルどちらか最後まで読み込むまで)
	while(true){
		// traの読み込み
		if(cmpflg<=0){ if(_iFile.read() == EOF) break; }
		// mstの読み込み
		if(cmpflg>0 || begin){
			if(_mFile.read() == EOF){ mstEnd=true; }
			begin=false;
		}
		// traとmstのキーの比較 (tra - mstの演算結果)
		if(mstEnd){
			cmpflg=-1;
		}else{
			cmpflg=0;
			for(int i=0;i<ksize;i++){
				if(_assertNullKEY) { 
					if( *(_iFile.getVal(kField[i]))=='\0' || *(_mFile.getVal(KField[i]))=='\0'){
						_existNullKEY = true;
					}
				}
				cmpflg = strcmp( _iFile.getVal(kField[i]), _mFile.getVal(KField[i]) );
				if(cmpflg!=0) break;
				//両方共NULLならアンマッチとする
				if( *(_iFile.getVal(kField[i]))=='\0' && *(_mFile.getVal(KField[i]))=='\0'){
					cmpflg=1;
					break;
				}
			}
		}
		// 一致
		if( (cmpflg==0 && !_reverse) || (cmpflg<0 && _reverse) ){
			_oFile.writeFld(_iFile.fldSize(),_iFile.getFld());
		}
		if(_elsefile){
			if( (cmpflg==0 && _reverse) || (cmpflg<0 && !_reverse) ){
				_uFile.writeFld(_iFile.fldSize(),_iFile.getFld());
			}
		}
	}
	//ソートスレッドを終了させて、終了確認
	//for(size_t i=0 ;i<_th_st.size();i++){ pthread_cancel(_th_st[i]->native_handle());	}
	//for(size_t i=0 ;i<_th_st.size();i++){ pthread_join(_th_st[i]->native_handle(),NULL);}
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
int kgCommon::run(void) 
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
    ((kgCommon*)arg)->runErrEnd();
}

int kgCommon::run(int inum,int *i_p,int onum, int* o_p,string &msg)
{
	int sts =1;
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

