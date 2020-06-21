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
// kghashavg.cpp 行集計(合計：ソーティング不要)
// =============================================================================
#include <cstdio>
#include <vector>
#include <kghashavg.h>
#include <kgFldHash.h>
#include <kgConfig.h>

using namespace std;
using namespace kglib;
using namespace kgmod;

// -----------------------------------------------------------------------------
// コンストラクタ(モジュール名，バージョン登録)
// -----------------------------------------------------------------------------
const char * kgHashavg::_ipara[] = {"i",""};
const char * kgHashavg::_opara[] = {"o",""};


kgHashavg::kgHashavg(void)
{
	_name    = "kghashavg";
	_version = "###VERSION###";

	_paralist = "f=,i=,o=,k=,hs=,-n";
	_paraflg = kgArgs::ALLPARAM;

	#include <help/en/kghashavgHelp.h>
	_titleL = _title;
	_docL   = _doc;
	#ifdef JPN_FORMAT
		#include <help/jp/kghashavgHelp.h>
	#endif

}
// -----------------------------------------------------------------------------
// パラメータセット＆入出力ファイルオープン
// -----------------------------------------------------------------------------
void kgHashavg::setArgsMain(void)
{
	_iFile.read_header();
	_oFile.setPrecision(_precision);

	// f= 項目引数のセット
	vector< vector<kgstr_t> > vvs = _args.toStringVecVec("f=",':',2,true);
	_fField.set(vvs, &_iFile,_fldByNum);

	// k= 項目引数のセット
	vector<kgstr_t> vs = _args.toStringVector("k=",false);
	_kField.set(vs,  &_iFile,_fldByNum);

	// -n オプションのセット
	_null=_args.toBool("-n");

	// hv= ハッシュ値のセット
	kgstr_t s = _args.toString("hs=",false);
	if(!s.empty()){
		_hashSize=atoi(s.c_str());
		if(_hashSize<=100)     _hashSize=101;
		if(_hashSize>=2000000) _hashSize=1999999;
	}else{
		_hashSize=199999;
	}	
}
// -----------------------------------------------------------------------------
// パラメータセット＆入出力ファイルオープン
// -----------------------------------------------------------------------------
void kgHashavg::setArgs(void)
{
	// パラメータチェック
	_args.paramcheck(_paralist,_paraflg);

	// 入出力ファイルオープン
	_iFile.open(_args.toString("i=",false), _env,_nfn_i);
	_oFile.open(_args.toString("o=",false), _env,_nfn_o,_rp);

	setArgsMain();

}
// -----------------------------------------------------------------------------
// パラメータセット＆入出力ファイルオープン
// -----------------------------------------------------------------------------
void kgHashavg::setArgs(int inum,int *i_p,int onum ,int *o_p)
{
	int iopencnt = 0;
	int oopencnt = 0;
	try{
		_args.paramcheck(_paralist,_paraflg);

		if(inum>1 || onum>1){ throw kgError("no match IO");}

		if(inum==1 && *i_p>0){ _iFile.popen(*i_p, _env,_nfn_i); }
		else     { _iFile.open(_args.toString("i=",true), _env,_nfn_i); }
		iopencnt++; 

		if(onum==1 && *o_p>0){ _oFile.popen(*o_p, _env,_nfn_o,_rp);  }
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
// 実行
// -----------------------------------------------------------------------------
int kgHashavg::runMain(void)
{

	// 項目名の出力
  _oFile.writeFldName(_fField, true,false);

	// ハッシュセット
	kgFldHash hash(_hashSize, &_iFile, &_kField, &_fField);
	while( EOF != _iFile.read() ){
		kgFldHashNode* hn = hash.insert(const_cast<const char**>(_iFile.getFld()));
		for(int i=0; i<hash.fldSize() ; i++){
			char* str=_iFile.getVal(_fField.num(i));
			if(*str != '\0'){
				hn->cnt(i,1);
				hn->sum(i,atof(str));
			}else{
				if(_assertNullIN) { _existNullIN  = true;}
				hn->nul(i,true);
			}
		}
	}

	//データ出力
	for(int i=0;i<hash.hashVal();i++){
		kgFldHashNode* node=hash.node(i);
		if(node==NULL){ continue;}
		while(node!=NULL){
			const vector<int>* flg=_fField.getFlg_p();
			for(std::size_t j=0; j<flg->size(); j++){ // csvの項目数で回す
				bool eol=false;
				if(j==flg->size()-1) eol=true;
				int num=flg->at(j);                  // 対応するval位置
				if(num == -1) _oFile.writeStr( node->idx(j)  ,eol );
				else{
					if(node->cnt(num)!=0 && (!_null || !node->nul(num)) ){
						_oFile.writeDbl( node->sum(num)/node->cnt(num),eol );
					}else{
						if(_assertNullOUT){ _existNullOUT = true;}
						_oFile.writeStr( ""            ,eol );
					}
				}
			}
			node=node->next();
		}
	}
	//ASSERT keynull_CHECK
	if(_assertNullKEY) { _existNullKEY =hash.keynull(); }
	// 終了処理
	_iFile.close();
	_oFile.close();

	return 0;

}

// -----------------------------------------------------------------------------
// 実行 
// -----------------------------------------------------------------------------
int kgHashavg::run(void) 
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
    ((kgHashavg*)arg)->runErrEnd();
}

int kgHashavg::run(int inum,int *i_p,int onum, int* o_p,string &msg)
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
