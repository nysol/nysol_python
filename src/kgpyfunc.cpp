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


#if PY_MAJOR_VERSION >= 3 && PY_MINOR_VERSION >=7
	#define pyAfterFORK PyOS_AfterFork_Child
#else		
	#define pyAfterFORK PyOS_AfterFork
#endif


// -----------------------------------------------------------------------------
// コンストラクタ(モジュール名，バージョン登録,パラメータ)
// -----------------------------------------------------------------------------
const char * kgPyfunc::_ipara[] = {"i",""};
const char * kgPyfunc::_opara[] = {"o",""};

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
int kgPyfunc::run(void) //ダミー
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
int kgPyfunc::run(
	PyObject* f_p,PyObject* a_p,PyObject* k_p,
	int inum,int *i_p,int onum, int* o_p,string & msg,
	pthread_mutex_t *mtx,pthread_cond_t *forkCond,volatile int *runst,vector<int> fdlist)
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

		// ============
		/// TH Vesion
		// ============
#define THVER 0		
#if THVER  
{

		pid_t pid;
		if ((pid = fork()) == 0) {	

			if(!Py_IsInitialized()){ Py_Initialize(); }
			if (!PyEval_ThreadsInitialized())	{  PyEval_InitThreads(); }
			pyAfterFORK();

			//for(size_t i=0; i<fdlist.size();i++){
			//	if ( fdlist[i] != i_p_t && fdlist[i] != o_p_t ){
			//		close(fdlist[i]);
			//	}
			//}
			if(i_p_t>0){
				dup2(i_p_t, 0);
				close(i_p_t);
			} 
			if(o_p_t>0){
				dup2(o_p_t, 1);
				close(o_p_t);
			}

/*
			PyInterpreterState* interp = PyInterpreterState_New();
			//PyThreadState *tstate = PyThreadState_New(interp);
			if(!PyGILState_Check()){
				PyEval_AcquireThread(tstate);
			}
*/
//			pthread_mutex_lock(mtx);
//			{
				//PyObject* rtn = PyObject_CallObject(f_p,a_p);
				PyObject* rtn = PyObject_Call(f_p,a_p,k_p);
				if(rtn == NULL){
					// ERR返す？
					PyErr_Print();
					_exit(1);
				}
				else{
					_exit(0);
				}
//			}
//			pthread_mutex_unlock(mtx);
/*
			PyEval_ReleaseThread(tstate);
			PyThreadState_Delete(tstate);
*/
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
				throw kgError("exec call python func");
			}
			return status;
		}
		else {//err
			throw kgError("fork error" );
		}
		

		return 0;
}
#else

		int initchek[2];
		int finchek[2];
		if( pipe(initchek) < 0){ 
			cerr << "call python func pipe open err(init)" << endl;
			throw kgError("call python func pipe open err(init)");
		}
		if( pipe(finchek) < 0){ 
			cerr << "call python func pipe open err(fin)" << endl;
			throw kgError("call python func pipe open err(fin)");
		}
		pid_t pid;
		if ((pid = fork()) == 0) {	
			close(initchek[0]);
			close(finchek[1]);

			PyGILState_STATE gstate;
			gstate = PyGILState_Ensure();
			pyAfterFORK();
			for(size_t i=0; i<fdlist.size();i++){ // 不必要FDclose
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

			write(initchek[1],"OK", strlen("OK"));
			close(initchek[1]);
			char buf[256];
			while (read(finchek[0], &buf, 256) > 0){
				if(!strncmp(buf,"OK",strlen("OK"))){ break;}
			}
			close(finchek[0]);

			PyObject* rtn = PyObject_Call(f_p,a_p,k_p);
			PyGILState_Release(gstate);

			if(rtn == NULL){
				// ERR返す？
				//PyErr_Print();
				_exit(1);
			}
			else{
				_exit(0);
			}

			
		}else if (pid>0){//parent
			close(initchek[1]);
			close(finchek[0]);

			char buf[256];
			while (read(initchek[0], &buf, 256) > 0){
				if(!strncmp(buf,"OK",strlen("OK"))){ break;}
			}
			close(initchek[0]);

			pthread_mutex_lock(mtx);
			int rtnx = pthread_cond_signal(forkCond);
			pthread_mutex_unlock(mtx);
			if(rtnx!=0){
				throw kgError("pthread_cond_signal error");
			}
			while(1){
				if(*runst){
					write(finchek[1],"OK", strlen("OK"));
					close(finchek[1]);		
					break;
				}
			}
			int status;
			waitpid(pid, &status, 0);
			if(i_p_t>0){ close(i_p_t);}
			if(o_p_t>0){ close(o_p_t);}
			if(status==0){
				msg.append(successEndMsg());
			}
			else{
				throw kgError("error occured in the function, check the detail error message using try-exception in the function.");
			}
			return status;
		}
		else {//err
			throw kgError("fork error" );
		}

		return 0;


#endif

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
		throw;

	}
	return 1;
}
