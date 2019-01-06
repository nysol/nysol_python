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
// kgcut.cpp 項目の選択クラス
// =============================================================================
#include <cstdio>
#include <vector>
#include <kg2cat.h>
#include <kgError.h>
#include <kgConfig.h>

using namespace std;
using namespace kglib;
using namespace kgmod;
// -----------------------------------------------------------------------------
// コンストラクタ(モジュール名，バージョン登録,パラメータ)
// -----------------------------------------------------------------------------
const char * kg2Cat::_ipara[] = {"i",""};
const char * kg2Cat::_opara[] = {"o",""};

kg2Cat::kg2Cat(void)
{
	_name    = "kg2cat";
	_version = "###VERSION###";
	_paralist = "f=,i=,o=,-nfni";
	_paraflg = kgArgs::COMMON|kgArgs::NULL_IN;
	#include <help/en/kgcutHelp.h>
	_titleL = _title;
	_docL   = _doc;
	#ifdef JPN_FORMAT
		#include <help/jp/kgcutHelp.h>
	#endif
	_first = true;
	_iCnt = 0;
	_itotal = 0;
	_iFiles = NULL;
}


void kg2Cat::setArgsMain(void){

	// f= 項目引数のセット
	vector< vector<kgstr_t> > vvs = _args.toStringVecVec("f=",':',2,false);
//	_fField.set(vvs, &_iFile, _fldByNum);


}

// -----------------------------------------------------------------------------
// 入出力ファイルオープン
// -----------------------------------------------------------------------------
void kg2Cat::setArgs(void)
{
	return ;
}

// -----------------------------------------------------------------------------
void kg2Cat::setArgs(int inum,int *i_p,int onum ,int *o_p)
{
	int iopencnt = 0;
	int oopencnt = 0;
	try {
		// パラメータチェック
		_args.paramcheck(_paralist,_paraflg);

		if(onum>1){ 
			throw kgError("no match IO"); 
		}

		// -nfniを指定した場合、-xも指定されていることにする
		bool nfniflg = _args.toBool("-nfni");
		if(nfniflg){ 
			_nfn_i =nfniflg; 
			_fldByNum = true;
		}
	
		//in
		vector<kgstr_t> isf = _args.toStringVector("i=",false);
		/*
		vector<kgstr_t> pisf = _args.toStringVector("i=",false);
		vector<kgstr_t> isf;
		if(!isf.empty()){
			isf = kgFilesearch(pisf,true,false);
		}
		*/
		_itotal = isf.size() + inum ;
		if(_itotal==0){// maxサイズの制限も入れる
				throw kgError("no match IO");
		}
		if(_itotal>32){// maxサイズの制限も入れる
				throw kgError("unsuport IO size");
		}
		_iFiles = new kgCSVfld*[_itotal];
	
		for(int i=0; i<inum ;i++){
			_iFiles[i] = new kgCSVfld;
			_iFiles[i]->popen(*(i_p+i), _env,_nfn_i);
			iopencnt++;
		}
		for(size_t i=0; i<isf.size() ;i++){
			_iFiles[inum+i] = new kgCSVfld;
			_iFiles[inum+i]->open(isf[i], _env,_nfn_i);
		}

		//out
		if(onum==1 && *o_p > 0){ 
			_oFile.popen(*o_p, _env,_nfn_o); 
			oopencnt++;
		}
		else{
			_oFile.open(_args.toString("o=",true), _env,_nfn_o);
		}
		

		setArgsMain();

	}catch(...){
		for(int i=iopencnt; i<inum ;i++){
			if(*(i_p+i)>0) ::close(*(i_p+i));
		}
		for(int i=oopencnt; i<onum ;i++){
			if(*(o_p+i)>0) ::close(*(o_p+i));
		}
		throw;
	}


}

void kg2Cat::readwrite(size_t pos){
		
	_iFiles[pos]->read_header();

	if(_first){
		_first =false;
		_fldname1 = _iFiles[pos]->fldName();
		_oFile.writeFldName(_fldname1);
	}

	vector<int> outFldNo;
	for(size_t i=0; i<_fldname1.size(); i++){
		int no=_iFiles[pos]->fldNum(_fldname1.at(i),true);
		if(no==-1){
			ostringstream ss;
			ss << "field name [" << _fldname1.at(i)
				<< "] not found on file ["   << _iFiles[pos]->fileName() << "]";
			throw kgError(ss.str());
		}
		outFldNo.push_back(no);
	}


	size_t icnt=0;
	//size_t fsize = _iFiles[pos]->fldSize();
	// 出力実行
	while( EOF != _iFiles[pos]->read() ){
		_oFile.writeFld(_iFiles[pos]->getFld(),&outFldNo);
		icnt++;
	}

	_iFiles[pos]->close();
	_iCnt += icnt;

}
// -----------------------------------------------------------------------------
// runMain
// -----------------------------------------------------------------------------
int kg2Cat::runMain()
{
	for(size_t i=0 ; i<_itotal;i++){
		readwrite(i);
	}
	_oFile.close();

	for(size_t i=0 ; i<_itotal ;i++){
		delete _iFiles[i];
	}
	delete _iFiles;
	_iFiles =NULL;
	_itotal=0;
	return 0;

// 例外catcher
}

// -----------------------------------------------------------------------------
// 実行
// -----------------------------------------------------------------------------
int kg2Cat::run(void) 
{
	return 0;
}

///* thraad cancel action
static void cleanup_handler(void *arg)
{
    ((kg2Cat*)arg)->runErrEnd();
}


int kg2Cat::run(int inum,int *i_p,int onum, int* o_p,string &msg)
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

