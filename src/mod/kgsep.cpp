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
// kgsep.cpp 行の複数ファイルへの分割
// =============================================================================
#include <cstdio>
#include <string>
#include <vector>
#include <kgsep.h>
#include <kgCSVutils.h>
#include <kgError.h>
#include <boost/filesystem.hpp>

using namespace boost;
using namespace std;
using namespace kglib;
using namespace kgmod;

// -----------------------------------------------------------------------------
// コンストラクタ(モジュール名，バージョン登録)
// -----------------------------------------------------------------------------
const char * kgSep::_ipara[] = {"i",""};
const char * kgSep::_opara[] = {""};

kgSep::kgSep(void)
{
	_name    = "kgsep";
	_version = "###VERSION###";
	_paralist = "d=,i=,-p,-q,f=,s=";
	_paraflg = kgArgs::COMMON|kgArgs::NULL_IN;

	#include <help/en/kgsepHelp.h>
	_titleL = _title;
	_docL   = _doc;
	#ifdef JPN_FORMAT
		#include <help/jp/kgsepHelp.h>
	#endif

}
// -----------------------------------------------------------------------------
// パラメータセット＆入出力ファイルオープン
// -----------------------------------------------------------------------------
void kgSep::setArgsMain(void)
{
	_iFile.read_header();

	// d= 項目のセット
	kgstr_t str = _args.toString("d=",true);
	_dField_str = evalFileFldName(str,_iFile,_dField,_fldByNum);

	vector<kgstr_t> vf = _args.toStringVector("f=",false);

	// s= 項目引数のセット
	vector< vector<kgstr_t> > vs_s = _args.toStringVecVec("s=","%",2,false);

	bool seqflg = _args.toBool("-q");
	if(_nfn_i) { seqflg = true; }
	if(!seqflg){ 
		//sortingRun(&_iFile,_dField);
		vector<kgstr_t> vsk;
		for(size_t i=0;i<_dField.size();i++){
			vsk.push_back( _iFile.fldName(_dField[i]) );
		}
		vector<kgstr_t> vs_ss = _args.toStringVector("s=",false);
		vsk.insert(vsk.end(),vs_ss.begin(),vs_ss.end());
		sortingRun(&_iFile,vsk);
		
	}

	_fField.set(vf, &_iFile,_fldByNum);
	_sField.set(vs_s, &_iFile,_fldByNum);

	// -p
	_mkdir_flg = _args.toBool("-p");
	
}

void kgSep::setArgs(int inum,int *i_p,int onum ,int *o_p)
{

	int iopencnt = 0;
	int oopencnt = 0;
	try{
		_args.paramcheck(_paralist,_paraflg);
		if(inum>1 || onum!=0){ throw kgError("no match IO");}

		// 入出力ファイルオープン
		if(inum==1 && *i_p>0){ _iFile.popen(*i_p, _env,_nfn_i); }
		else     { _iFile.open(_args.toString("i=",true), _env,_nfn_i); }
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
// パラメータチェック＆入出力ファイルオープン
// -----------------------------------------------------------------------------
void kgSep::setArgs(void)
{
	_args.paramcheck(_paralist,_paraflg);

	_iFile.open(_args.toString("i=",false), _env,_nfn_i);

	setArgsMain();

}

void kgSep::writeFldName()
{
	if( _oFile.noFldName( ) ) return;

	if (_fField.size()==0){
		if(_sField.size()!=0){
			_oFile.writeFldName(_sField,kgstr_t("%"));
		}
		else{		
			_oFile.writeFldName(_iFile);
		}
	}
	else{
		if(_sField.size()!=0){
			vector<kgstr_t> outfld;
			for(size_t i=0; i<_fField.size();i++){
				kgstr_t oName;
				oName = _iFile.fldName(_fField.num(i));
				oName.append(_iFile.sortParaStr(_fField.num(i),_dField.size()));
				outfld.push_back(oName);
			}
			_oFile.writeFldName(outfld);

		}
		else{
			vector<kgstr_t> outfld;
			for(size_t i=0; i<_fField.size();i++){
				kgstr_t oName;
				oName = _iFile.fldName(_fField.num(i));
				oName.append(_iFile.sortParaStr(_fField.num(i)));
				outfld.push_back(oName);
			}
			_oFile.writeFldName(outfld);

		}	
	}
} 


// -----------------------------------------------------------------------------
// 実行
// -----------------------------------------------------------------------------
int kgSep::runMain(void) 
{
	// 入力ファイルにkey項目番号をセットする．
	_iFile.setKey(_dField);

	// データ出力
	while(_iFile.read()!=EOF){
		//keybreak or最初はファイル名を生成
		if( _iFile.keybreak() || _iFile.begin() ){
			if( _iFile.keybreak() ) { _oFile.close();}
			if((_iFile.status() & kgCSV::End )) break;
			/*ファイル名生成*/
			kgstr_t fstr;
			vector<kgstr_t>::size_type i=0;
			for(vector<int>::size_type j=0;j<_dField.size();j++,i++){
				if(_assertNullIN && *_iFile.getNewVal(_dField[j])=='\0') { _existNullIN  = true;}
				fstr = fstr+ _dField_str[i] + _iFile.getNewVal(_dField[j]);			
			}
			fstr = fstr+_dField_str[i];
			if(_mkdir_flg){
				filesystem::path filename=filesystem::path(fstr);
				filesystem::path dirname =filename.branch_path();
				create_directories(dirname);
			}
			// ファイルオープン
			_oFile.open(fstr, _env,_nfn_o);
			writeFldName();

		}
		//通常処理
			if (_fField.size()==0){
				_oFile.writeFld(_iFile.fldSize(),_iFile.getNewFld());
			}else{
				vector<int>	vt = _fField.getNum();
				_oFile.writeFld(_iFile.getNewFld(),&vt);
			}
	}
	// 終了処理
	th_cancel();
	_oFile.close();
	_iFile.close();
	return 0;

}




int kgSep::run(void) 
{
	try {
		// パラメータセット＆入出力ファイルオープン
		setArgs();
		int sts = runMain();
		successEnd();
		return sts;

	}catch(kgOPipeBreakError& err){

		runErrEnd();
		successEnd();
		return 0;

	}
	catch(kgError& err){

		runErrEnd();
		errorEnd(err);

	}catch (const std::exception& e) {

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
    ((kgSep*)arg)->runErrEnd();
}


int kgSep::run(int inum,int *i_p,int onum, int* o_p,string &msg)
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

	}catch (const std::exception& e) {

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

