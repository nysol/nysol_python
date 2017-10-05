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
// kgLoad.cpp 行の複製
// =============================================================================
#include <cstdio>
#include <unistd.h>
#include <kgexcmd.h>
#include <kgError.h>
#include <kgConfig.h>

using namespace std;
using namespace kglib;
using namespace kgmod;

static char* strGET(PyObject* data){
#if PY_MAJOR_VERSION >= 3
	return PyUnicode_AsUTF8(data);
#else		
	return PyString_AsString(data);
#endif

}

// -----------------------------------------------------------------------------
// コンストラクタ(モジュール名，バージョン登録,パラメータ)
// -----------------------------------------------------------------------------
kgExcmd::kgExcmd(void)
{
	_name    = "kgexcmd";
	_version = "###VERSION###";

	_paralist = "i=,o=,cmdstr=";
	_paraflg = kgArgs::COMMON|kgArgs::IODIFF;

	_titleL = _title = "";
//	_docL   = _doc  = "";
//	#ifdef JPN_FORMAT
//		#include <help/jp/kgtab2csvHelp.h>
//	#endif
	
}
// -----------------------------------------------------------------------------
// パラメータセット＆入出力ファイルオープン
// -----------------------------------------------------------------------------
void kgExcmd::setArgs(void)
{
	// パラメータチェック
	_args.paramcheck("cmdstr=");

	// 入出力ファイルオープン
	string cv = _args.toString("cmdstr=",true);
	if(cv.front() =='\''&&cv.back()=='\''){
		cv = cv.substr(1,cv.size()-2);
	}
	
	vector<string> cmdstr = kglib::splitToken(cv,' ');
	_cmdars = new const char* [cmdstr.size()+1];

	for(size_t i=0;i<cmdstr.size();i++){
		_cmdars[i]  = cmdstr[i].c_str();
	}
	_cmdars[cmdstr.size()]=NULL;
}

// -----------------------------------------------------------------------------
// パラメータセット＆入出力ファイルオープン
// -----------------------------------------------------------------------------
/*
void kgExcmd::setArgs(int i_p,int o_p)
{
	// パラメータチェック
	_args.paramcheck("i=,o=",kgArgs::COMMON|kgArgs::IODIFF);

	// 入出力ファイルオープン
	if(i_p>0){
		_iFile.popen(i_p, _env,_nfn_i);
	}
	else{
		// 入出力ファイルオープン
		_iFile.open(_args.toString("i=",false), _env,_nfn_i);
	}
	if(o_p>0){
		_oFile.popen(o_p, _env,_nfn_o);
	}else{
		_oFile.open(_args.toString("o=",false), _env,_nfn_o);
	}
	_iFile.read_header();
	
}*/

// -----------------------------------------------------------------------------
// 実行
// -----------------------------------------------------------------------------
int kgExcmd::run(void) try 
{
}catch(kgError& err){
	errorEnd(err);
	return 1;
}catch (const exception& e) {
	kgError err(e.what());
	errorEnd(err);
	return 1;
}catch(char * er){
	kgError err(er);
	errorEnd(err);
	return 1;
}catch(...){
	kgError err("unknown error" );
	errorEnd(err);
	return 1;
}

// -----------------------------------------------------------------------------
// 実行
// -----------------------------------------------------------------------------
int kgExcmd::run(int i_p,int o_p) try 
{

	setArgs();
	pid_t pid;
	if ((pid = fork()) == 0) {	
		if(i_p>0){
			dup2(i_p, 0);
			close(i_p);
		} 
		if(o_p>0){
			dup2(o_p, 1);
			close(o_p);
		} 
		execvp(_cmdars[0],(char*const*)_cmdars);
		throw kgError("run error" );
	
	}
	else if (pid>0){//parent
		int status = 0;
		int ret = waitpid(pid, &status, 0);
		if(i_p>0){ close(i_p);}
		if(o_p>0){ close(o_p);}
		return status;
	}
	else {//err
		throw kgError("fork error" );
	}
	
	return 0;

}catch(kgError& err){
	errorEnd(err);
	return 1;
}catch (const exception& e) {
	kgError err(e.what());
	errorEnd(err);
	return 1;
}catch(char * er){
	kgError err(er);
	errorEnd(err);
	return 1;
}catch(...){
	kgError err("unknown error" );
	errorEnd(err);
	return 1;
}


