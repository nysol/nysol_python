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
#include <fcntl.h>
#include <kgexcmd.h>
#include <kgError.h>
#include <kgConfig.h>

using namespace std;
using namespace kglib;
using namespace kgmod;
// -----------------------------------------------------------------------------
// コンストラクタ(モジュール名，バージョン登録,パラメータ)
// -----------------------------------------------------------------------------
const char * kgExcmd::_ipara[] = {"i",""};
const char * kgExcmd::_opara[] = {"o",""};

kgExcmd::kgExcmd(void)
{
	_name    = "kgexcmd";
	_version = "###VERSION###";

	_paralist = "i=,o=,cmdstr=";
	_paraflg = kgArgs::COMMON|kgArgs::IODIFF;

	_titleL = _title = "";
	
}
// -----------------------------------------------------------------------------
// パラメータセット＆入出力ファイルオープン
// -----------------------------------------------------------------------------
void kgExcmd::setArgs(void)
{
	// パラメータチェック
	_args.paramcheck("cmdstr=");

	// 入出力ファイルオープン
	_cmdstr = _args.toString("cmdstr=",true);
	if(_cmdstr.size()!=0&&_cmdstr[0] =='\''&&_cmdstr[_cmdstr.size()-1]=='\''){
		_cmdstr = _cmdstr.substr(1,_cmdstr.size()-2);
	}
	_cmdstrv = kglib::splitTokenQ(_cmdstr,' ',true);
	_cmdars = new const char* [_cmdstrv.size()+1];

	for(size_t i=0;i<_cmdstrv.size();i++){
		_cmdars[i]  = _cmdstrv[i].c_str();
	}
	_cmdars[_cmdstrv.size()]=NULL;
}

// -----------------------------------------------------------------------------
// 実行
// -----------------------------------------------------------------------------
int kgExcmd::run(void)
{
	try {
		return 0;

	}catch(kgError& err){

		errorEnd(err);
	}catch (const std::exception& e) {

		kgError err(e.what());
		errorEnd(err);
	}catch(char * er){

		kgError err(er);
		errorEnd(err);
	}catch(...){

		kgError err("unknown error" );
		errorEnd(err);
	}
	return 1;

}
// -----------------------------------------------------------------------------
// 実行
// -----------------------------------------------------------------------------
int kgExcmd::run(int inum,int *i_p,int onum, int* o_p ,string& msg)
{

	try{
		setArgs();
		if(inum>1 || onum>1){
			throw kgError("no match IO");
		}

		int i_p_t=-1; 
		int o_p_t=-1;
		// 入出力ファイルオープン
		if(inum==1 && *i_p > 0){ i_p_t = *i_p; }
		if(onum==1 && *o_p > 0){ o_p_t = *o_p; }

		pid_t pid;
		if ((pid = fork()) == 0) {	
			if(i_p_t>0){
				dup2(i_p_t, 0);
				close(i_p_t);
			} 
			if(o_p_t>0){
				dup2(o_p_t, 1);
				close(o_p_t);
			}
			if(execvp(_cmdars[0],(char*const*)_cmdars)==-1){
				//perror("exec ERROR");
				_exit(-1);
			}
		}
		else if (pid>0){//parent

			int status = 0;
			//int ret = // ステータス情報どうする？
			waitpid(pid, &status, 0);
			if(i_p_t>0){ close(i_p_t);}
			if(o_p_t>0){ close(o_p_t);}
			if( ( WEXITSTATUS(status) & 0xFF) == 0xFF ){
				throw kgError("exec err");
			}
			else{
				msg.append(successEndMsg());
			}
			return 0;
		}
		else {//err
			throw kgError("fork error" );
		}
		return 0;

	}catch(kgError& err){
		msg.append(errorEndMsg(err));


	}catch (const exception& e) {

		kgError err(e.what());
		msg.append(errorEndMsg(err));

	}catch(char * er){

		kgError err(er);
		msg.append(errorEndMsg(err));

	}
	//KG_ABI_CATCH
	catch(...){
		kgError err("unknown error" );
		msg.append(errorEndMsg(err));

	}
	return 1;
}
