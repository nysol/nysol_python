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
#include <kgpyfunc.h>
#include <kgError.h>
#include <kgConfig.h>

using namespace std;
using namespace kglib;
using namespace kgmod;

// -----------------------------------------------------------------------------
// コンストラクタ(モジュール名，バージョン登録,パラメータ)
// -----------------------------------------------------------------------------
kgPyfunc::kgPyfunc(void)
{
	_name    = "kgpyfunc";
	_version = "###VERSION###";

	_paralist = "i=,o=,func=,args=";
	_paraflg = kgArgs::COMMON;

	_titleL = _title = "kgpyfunc";
	
}
// -----------------------------------------------------------------------------
// パラメータセット＆入出力ファイルオープン
// -----------------------------------------------------------------------------
void kgPyfunc::setArgs(void)
{

}

// -----------------------------------------------------------------------------
// 実行
// -----------------------------------------------------------------------------
int kgPyfunc::run(void)
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
int kgPyfunc::run(PyObject* f_p,PyObject* a_p,int inum,int *i_p,int onum, int* o_p,string & msg,pthread_mutex_t *mtx,vector<int> fdlist)
{
	cerr << "kgPyfunc::run" << endl;

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

	//	cerr << " inum " << inum << " "  << i_p_t << " onum " << onum  <<  " "  << o_p_t <<endl;

		// FD_CLOSE_FLGを外すほうがいい？（）
		//if(i_p_t!=-1){ fcntl(i_p_t, F_SETFD, 0);}
		//if(o_p_t!=-1){ fcntl(o_p_t, F_SETFD, 0);}
		/*
		if(i_p_t>0){
			dup2(i_p_t, 0);
			close(i_p_t);
		} 
		if(o_p_t>0){
			dup2(o_p_t, 1);
			close(o_p_t);
		}
		PyObject* rtn = PyObject_CallObject(f_p,a_p);
		close(0)
		close(1)
		return 0;
		*/


		pid_t pid;
		if ((pid = fork()) == 0) {	
			cerr << "kgPyfunc::run1" << endl;

			if(!Py_IsInitialized()){
				cerr << "kgPyfunc::run1-1" << endl;
				Py_Initialize();
			}
			cerr << "kgPyfunc::run2" << endl;
			if (!PyEval_ThreadsInitialized())	{ 
				cerr << "kgPyfunc::run2-1" << endl;
				PyEval_InitThreads();
			}
			cerr << "kgPyfunc::run3" << endl;

			PyOS_AfterFork();
			cerr << "kgPyfunc::run4" << endl;

			for(size_t i=0; i<fdlist.size();i++){
				if ( fdlist[i] != i_p_t && fdlist[i] != o_p_t ){
					close(fdlist[i]);
				}
			}
			if(i_p_t>0){
				dup2(i_p_t, 0);
				close(i_p_t);
			} 
			if(o_p_t>0){
				dup2(o_p_t, 1);
				close(o_p_t);
			}
			cerr << "kgPyfunc::run5" << endl;

/*
			PyInterpreterState* interp = PyInterpreterState_New();
				cerr << "init5" << endl;
			//PyThreadState *tstate = PyThreadState_New(interp);

			//PyThreadState *tstate2 = PyThreadState_New(interp);


				cerr << "init6" << endl;
			if(!PyGILState_Check()){
				cerr << "init6-0" << endl;
				PyEval_AcquireThread(tstate);
				cerr << "init6-1" << endl;
			}
	*/
			cerr << "kgPyfunc::run6" << endl;
			pthread_mutex_lock(mtx);
			{
			cerr << "kgPyfunc::run6-1" << endl;
				//PyObject* rtn = PyObject_CallObject(f_p,a_p);
				PyObject* rtn = PyObject_Call(f_p,a_p,NULL);
			cerr << "kgPyfunc::run6-2" << endl;
			}
			pthread_mutex_unlock(mtx);
			cerr << "kgPyfunc::run7" << endl;
/*
			PyEval_ReleaseThread(tstate);
			cerr << "init9" << endl;
			PyThreadState_Delete(tstate);
			cerr << "init10" << endl;
*/
			cerr << "kgPyfunc::run8" << endl;
			_exit(0);
		}
		else if (pid>0){//parent
			int status = 0;
			//int ret = 
			waitpid(pid, &status, 0);

			if(i_p_t>0){ close(i_p_t);}
			if(o_p_t>0){ close(o_p_t);}


			if(status==0){
				msg.append(successEndMsg());
			}
			else{
				throw kgError("exec err errno:(" + toString(status) + ")");
			}
			return status;
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

	}catch(...){

		kgError err("unknown error" );
		msg.append(errorEndMsg(err));

	}
	return 1;
}
