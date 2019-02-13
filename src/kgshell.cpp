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
#include <fcntl.h>
#include <kgCSV.h>
#include <kgEnv.h>
#include <kgError.h>
#include <kgMethod.h>
#include <kgmsgincpysys.h>
#include <kgshell.h>
#include <kgTempfile.h>
#include <pthread.h>
#include <sys/time.h>
#include <sys/resource.h>
#include <cxxabi.h>

using namespace kgmod;
using namespace kglib;

template <class kgmodTP> 
void kgshell::setMap(std::string name,int runTP){
	_kgmod_map[name] = boost::lambda::bind(boost::lambda::new_ptr<kgmodTP>());
	_kgmod_run[name] = runTP;
	_kgmod_Iinfo[name] = kgmodTP::_ipara;
	_kgmod_Oinfo[name] = kgmodTP::_opara;
}

kgshell::kgshell(int mflg,int rumlim,size_t memttl,int pymsg){

	setMap<kgPyfunc>("runfunc",3);
	setMap<kgLoad>("writelist",1);
	setMap<kgLoad>("readlist",2);
	setMap<kg2Tee>("m2tee",0);
	setMap<kgFifo>("mfifo",0);
	setMap<kgExcmd>("cmd",0);
	setMap<kgLoad>("readcsv",0);
	setMap<kgLoad>("writecsv",0);
	setMap<kgLoad>("mstdin",0);
	setMap<kgLoad>("mstdout",0);
	setMap<kgSortchk>("msortchk",0);
	setMap<kgFifo>("mread",0);
	setMap<kgFifo>("mwrite",0);
	setMap<kgCut>("mcut",0);
	setMap<kgCat>("mcat",0);
	setMap<kgSum>("msum",0);
	setMap<kgCal>("mcal",0);
	setMap<kgJoin>("mjoin",0);
	setMap<kg2Cross>("m2cross",0);
	setMap<kgAccum>("maccum",0);
	setMap<kgAvg>("mavg",0);
	setMap<kgBest>("mbest",0);
	setMap<kgBucket>("mbucket",0);
	setMap<kgChgnum>("mchgnum",0);
	setMap<kgChgstr>("mchgstr",0);
	setMap<kgCombi>("mcombi",0);
	setMap<kgCommon>("mcommon",0);
	setMap<kgCount>("mcount",0);
	setMap<kgCross>("mcross",0);
	setMap<kgDelnull>("mdelnull",0);
	setMap<kgDformat>("mdformat",0);
	setMap<kgDuprec>("mduprec",0);
	setMap<kgFldname>("mfldname",0);
	setMap<kgFsort>("mfsort",0);
	setMap<kgHashavg>("mhashavg",0);
	setMap<kgHashsum>("mhashsum",0);
	setMap<kgKeybreak>("mkeybreak",0);
	setMap<kgMbucket>("mmbucket",0);
	setMap<kgMvavg>("mmvavg",0);
	setMap<kgMvsim>("mmvsim",0);
	setMap<kgMvstats>("mmvstats",0);
	setMap<kgNewnumber>("mnewnumber",0);
	setMap<kgNewrand>("mnewrand",0);
	setMap<kgNewstr>("mnewstr",0);
	setMap<kgNjoin>("mnjoin",0);
	setMap<kgNormalize>("mnormalize",0);
	setMap<kgNrcommon>("mnrcommon",0);
	setMap<kgNrjoin>("mnrjoin",0);
	setMap<kgNullto>("mnullto",0);
	setMap<kgNumber>("mnumber",0);
	setMap<kgPadding>("mpadding",0);
	setMap<kgPaste>("mpaste",0);
	setMap<kgProduct>("mproduct",0);
	setMap<kgRand>("mrand",0);
	setMap<kgRjoin>("mrjoin",0);
	setMap<kgSed>("msed",0);
	setMap<kgSel>("msel",0);
	setMap<kgSelnum>("mselnum",0);
	setMap<kgSelrand>("mselrand",0);
	setMap<kgSelstr>("mselstr",0);
	setMap<kgSetstr>("msetstr",0);
	setMap<kgShare>("mshare",0);
	setMap<kgSim>("msim",0);
	setMap<kgSlide>("mslide",0);
	setMap<kgSplit>("msplit",0);
	setMap<kgStats>("mstats",0);
	setMap<kgSummary>("msummary",0);
	setMap<kgTonull>("mtonull",0);
	setMap<kgTra>("mtra",0);
	setMap<kgTraflg>("mtraflg",0);
	setMap<kgTrafld>("mtrafld",0);
	setMap<kgUniq>("muniq",0);
	setMap<kgVcat>("mvcat",0);
	setMap<kgVcommon>("mvcommon",0);
	setMap<kgVcount>("mvcount",0);
	setMap<kgVdelim>("mvdelim",0);
	setMap<kgVdelnull>("mvdelnull",0);
	setMap<kgVjoin>("mvjoin",0);
	setMap<kgVnullto>("mvnullto",0);
	setMap<kgVreplace>("mvreplace",0);
	setMap<kgVsort>("mvsort",0);
	setMap<kgVuniq>("mvuniq",0);
	setMap<kgWindow>("mwindow",0);
	setMap<kgArff2csv>("marff2csv",0);
	setMap<kgXml2csv>("mxml2csv",0);
	setMap<kgSortf>("msortf",0);
	setMap<kgTab2csv>("mtab2csv",0);
	setMap<kgSep>("msep",0);
	setMap<kgShuffle>("mshuffle",0);
	setMap<kg2Cat>("m2cat",0);
	setMap<kgUnicat>("municat",0);

	_nfni = false;
	_iterrtn= NULL;
	_iterrtnk= NULL;
	_th_st_pp = NULL;
	_clen = 0;
	_modlist=NULL;
	_envlist=NULL;
	_save=NULL;
	_watchFlg = false;
	_runst   = NULL;
	_argst =NULL;
	_th_rtn = NULL;

	if(!mflg){  _env.verblvl(2);	}
	_runlim = rumlim;
	_memttl = memttl;
	_mflg   = mflg;
	_pymsg  = pymsg;


	if (pthread_mutex_init(&_mutex, NULL) == -1) { 
		ostringstream ss;
		ss << "init mutex error";
		throw kgError(ss.str());
	}
	 if (pthread_mutex_init(&_stsMutex, NULL) == -1) { 
		ostringstream ss;
		ss << "init mutex error";
		throw kgError(ss.str());
	}
	 if (pthread_mutex_init(&_iniMutex, NULL) == -1) { 
		ostringstream ss;
		ss << "init mutex error";
		throw kgError(ss.str());
	}

	if (pthread_cond_init(&_stsCond, NULL) == -1) { 
		ostringstream ss;
		ss << "init cond mutex error";
		throw kgError(ss.str());
	}
	  
	if (pthread_cond_init(&_forkCond, NULL) == -1) { 
		ostringstream ss;
		ss << "init cond mutex error";
		throw kgError(ss.str());
	}
	if (pthread_cond_init(&_iniCond, NULL) == -1) { 
		ostringstream ss;
		ss << "init cond mutex error";
		throw kgError(ss.str());
	}

	_tempFile.init(&_env);

}

void *kgshell::run_func(void *arg){

	try{

		string msg;
		argST *a =(argST*)arg; 

		int sts = a->mobj->run(a->i_cnt,a->i_p,a->o_cnt,a->o_p,msg);

		pthread_mutex_lock(a->stMutex);

		a->status =sts;
		a->finflg=true;
		a->msg.append(msg);
		a->endtime=getNowTime(true);

		pthread_cond_signal(a->stCond);
		pthread_mutex_unlock(a->stMutex);

	}
	catch(kgError& err){
		argST *a =(argST*)arg; 
		pthread_mutex_lock(a->stMutex);
		a->status = 1;
		a->finflg=true;
		a->endtime=getNowTime(true);
		a->msg.append(a->mobj->name());
		a->msg.append(" ");
		a->msg.append(err.message(0));
		pthread_cond_signal(a->stCond);
		pthread_mutex_unlock(a->stMutex);

	}catch (const exception& e) {
		argST *a =(argST*)arg; 
		pthread_mutex_lock(a->stMutex);
		a->status = 1;
		a->finflg=true;
		a->endtime=getNowTime(true);
		a->msg.append(a->mobj->name());
		a->msg.append(" ");
		a->msg.append(e.what());
		pthread_cond_signal(a->stCond);
		pthread_mutex_unlock(a->stMutex);

	}catch(char * er){
		argST *a =(argST*)arg; 
		pthread_mutex_lock(a->stMutex);
		a->status = 1;
		a->finflg=true;
		a->endtime=getNowTime(true);
		a->msg.append(a->mobj->name());
		a->msg.append(" ");
		a->msg.append(er);
		pthread_cond_signal(a->stCond);
		pthread_mutex_unlock(a->stMutex);
	}
#if !defined(__clang__) && defined(__GNUC__)
	catch(abi::__forced_unwind&){  
		argST *a =(argST*)arg; 
		pthread_mutex_lock(a->stMutex);
		a->status = 2;
		a->finflg=true;
		a->endtime=getNowTime(true);
		a->msg.append(a->mobj->name());
		a->msg.append(" ABI THROW");
		pthread_cond_signal(a->stCond);
		pthread_mutex_unlock(a->stMutex);
		throw;
	}
#endif
	catch(...){
		argST *a =(argST*)arg; 
		pthread_mutex_lock(a->stMutex);
		a->status = 1;
		a->finflg=true;
		a->endtime=getNowTime(true);
		a->msg.append(a->mobj->name());
		a->msg.append(" unKnown ERROR ");
		pthread_cond_signal(a->stCond);
		pthread_mutex_unlock(a->stMutex);
	}

	pthread_exit(0);

	return NULL;	
}

void *kgshell::run_writelist(void *arg){

	string msg;
	argST *a ;
	PyGILState_STATE gstate;
	
	try{
		a = (argST*)arg; 
		gstate = PyGILState_Ensure();

		int sts = a->mobj->run(a->i_cnt,a->i_p,a->list,a->mutex,msg);
		PyGILState_Release(gstate);

		pthread_mutex_lock(a->stMutex);
		a->status = sts;
		a->finflg=true;
		a->msg.append(msg);
		a->endtime=getNowTime(true);
		pthread_cond_signal(a->stCond);
		pthread_mutex_unlock(a->stMutex);

	}
	catch(kgError& err){
		PyGILState_Release(gstate);
		argST *a =(argST*)arg; 
		pthread_mutex_lock(a->stMutex);
		a->status = 1;
		a->finflg=true;
		a->endtime=getNowTime(true);
		a->msg.append(a->mobj->name());
		a->msg.append(" ");
		a->msg.append(err.message(0));
		pthread_cond_signal(a->stCond);
		pthread_mutex_unlock(a->stMutex);

	}catch (const exception& e) {
		PyGILState_Release(gstate);
		argST *a =(argST*)arg; 
		pthread_mutex_lock(a->stMutex);
		a->status = 1;
		a->finflg=true;
		a->endtime=getNowTime(true);
		a->msg.append(a->mobj->name());
		a->msg.append(" ");
		a->msg.append(e.what());
		pthread_cond_signal(a->stCond);
		pthread_mutex_unlock(a->stMutex);

	}catch(char * er){
		PyGILState_Release(gstate);
		argST *a =(argST*)arg; 
		pthread_mutex_lock(a->stMutex);
		a->status = 1;
		a->finflg=true;
		a->endtime=getNowTime(true);
		a->msg.append(a->mobj->name());
		a->msg.append(" ");
		a->msg.append(er);
		pthread_cond_signal(a->stCond);
		pthread_mutex_unlock(a->stMutex);

	}
#if !defined(__clang__) && defined(__GNUC__)
	catch(abi::__forced_unwind&){  
		PyGILState_Release(gstate);
		argST *a =(argST*)arg; 
		pthread_mutex_lock(a->stMutex);
		a->status = 2;
		a->finflg=true;
		a->endtime=getNowTime(true);
		a->msg.append(a->mobj->name());
		a->msg.append(" ABI THROW");
		pthread_cond_signal(a->stCond);
		pthread_mutex_unlock(a->stMutex);
		throw;
	}
#endif	
	catch(...){
		PyGILState_Release(gstate);
		argST *a =(argST*)arg; 
		pthread_mutex_lock(a->stMutex);
		a->status = 1;
		a->finflg=true;
		a->endtime=getNowTime(true);
		a->msg.append(a->mobj->name());
		a->msg.append(" unKnown ERROR");
		pthread_cond_signal(a->stCond);
		pthread_mutex_unlock(a->stMutex);
	}
	pthread_exit(0);
	return NULL;	
}

void *kgshell::run_readlist(void *arg){
	try{

		string msg;
		argST *a =(argST*)arg; 

		int sts = a->mobj->run(a->list,a->o_cnt,a->o_p,msg);

		pthread_mutex_lock(a->stMutex);
		a->status = sts;
		a->finflg=true;
		a->msg.append(msg);
		a->endtime=getNowTime(true);

		pthread_cond_signal(a->stCond);
		pthread_mutex_unlock(a->stMutex);

	}catch(kgError& err){
		argST *a =(argST*)arg; 
		pthread_mutex_lock(a->stMutex);
		a->status = 1;
		a->finflg=true;
		a->endtime=getNowTime(true);
		a->msg.append(" unKnown ERROR");
		a->msg.append(a->mobj->name());
		a->msg.append(" ");
		a->msg.append(err.message(0));
		pthread_cond_signal(a->stCond);
		pthread_mutex_unlock(a->stMutex);

	}catch (const exception& e) {
		argST *a =(argST*)arg; 
		pthread_mutex_lock(a->stMutex);
		a->status = 1;
		a->finflg=true;
		a->endtime=getNowTime(true);
		a->msg.append(" unKnown ERROR");
		a->msg.append(a->mobj->name());
		a->msg.append(" ");
		a->msg.append(e.what());
		pthread_cond_signal(a->stCond);
		pthread_mutex_unlock(a->stMutex);

	}catch(char * er){
		argST *a =(argST*)arg; 
		pthread_mutex_lock(a->stMutex);
		a->status = 1;
		a->finflg=true;
		a->endtime=getNowTime(true);
		a->msg.append(" unKnown ERROR");
		a->msg.append(a->mobj->name());
		a->msg.append(" ");
		a->msg.append(er);
		pthread_cond_signal(a->stCond);
		pthread_mutex_unlock(a->stMutex);

	}
#if !defined(__clang__) && defined(__GNUC__)
	catch(abi::__forced_unwind&){  
		argST *a =(argST*)arg; 
		pthread_mutex_lock(a->stMutex);
		a->status = 2;
		a->finflg=true;
		a->endtime=getNowTime(true);
		a->msg.append(a->mobj->name());
		a->msg.append(" ABI THROW");
		pthread_cond_signal(a->stCond);
		pthread_mutex_unlock(a->stMutex);
		throw;
	}
#endif	
	catch(...){
		argST *a =(argST*)arg; 
		pthread_mutex_lock(a->stMutex);
		a->status = 1;
		a->finflg=true;
		a->endtime=getNowTime(true);
		a->msg.append(" unKnown ERROR");
		a->msg.append(a->mobj->name());
		pthread_cond_signal(a->stCond);
		pthread_mutex_unlock(a->stMutex);
	}
	pthread_exit(0);
	return NULL;	
}

void *kgshell::run_pyfunc(void *arg){
	try{
		string msg;
		argST *a =(argST*)arg; 

		int sts = a->mobj->run(
			a->fobj,a->aobj,a->kobj,
			a->i_cnt,a->i_p,a->o_cnt,a->o_p,msg,
			a->mutex,a->forkCond,a->runst,a->fdlist
		);


		pthread_mutex_lock(a->stMutex);
		a->status = sts;
		a->finflg=true;
		a->msg.append(msg);
		a->endtime=getNowTime(true);

		pthread_cond_signal(a->stCond);
		pthread_mutex_unlock(a->stMutex);

	}catch(kgError& err){
		argST *a =(argST*)arg; 
		pthread_mutex_lock(a->stMutex);
		a->status = 1;
		a->finflg=true;
		a->endtime=getNowTime(true);
		a->msg.append(a->mobj->name());
		a->msg.append(" ");
		a->msg.append(err.message(0));
		pthread_cond_signal(a->stCond);
		pthread_mutex_unlock(a->stMutex);

	}catch (const exception& e) {
		argST *a =(argST*)arg; 
		pthread_mutex_lock(a->stMutex);
		a->status = 1;
		a->finflg=true;
		a->endtime=getNowTime(true);
		a->msg.append(a->mobj->name());
		a->msg.append(" ");
		a->msg.append(e.what());
		pthread_cond_signal(a->stCond);
		pthread_mutex_unlock(a->stMutex);

	}catch(char * er){
		argST *a =(argST*)arg; 
		pthread_mutex_lock(a->stMutex);
		a->status = 1;
		a->finflg=true;
		a->endtime=getNowTime(true);
		a->msg.append(a->mobj->name());
		a->msg.append(" ");
		a->msg.append(er);
		pthread_cond_signal(a->stCond);
		pthread_mutex_unlock(a->stMutex);

	}
#if !defined(__clang__) && defined(__GNUC__)
	catch(abi::__forced_unwind&){  
		argST *a =(argST*)arg; 
		pthread_mutex_lock(a->stMutex);
		a->status = 2;
		a->finflg=true;
		a->endtime=getNowTime(true);
		a->msg.append(a->mobj->name());
		a->msg.append(" ABI THROW");
		pthread_cond_signal(a->stCond);
		pthread_mutex_unlock(a->stMutex);
		throw;
	}
#endif	
	catch(...){
		argST *a =(argST*)arg; 
		pthread_mutex_lock(a->stMutex);
		a->status = 1;
		a->finflg=true;
		a->endtime=getNowTime(true);
		a->msg.append(a->mobj->name());
		a->msg.append(" unKnown ERROR");
		pthread_cond_signal(a->stCond);
		pthread_mutex_unlock(a->stMutex);
	}
	pthread_exit(0);

	return NULL;	
}

static void watch_raw_OUTPUT(const string& v,kgEnv *env,bool pymsg){
	if(pymsg){
		kgMsgIncPySys msg(kgMsg::IGN, env);
		msg.output_ignore(v);
	}else{
		kgMsg msg(kgMsg::IGN, env);
		msg.output_ignore(v);
	}
}
static void watch_end_OUTPUT(const string& v,kgEnv *env,bool pymsg){
	if(pymsg){
		kgMsgIncPySys msg(kgMsg::END, env);
		ostringstream ss;
		ss << "kgshell (" << v << ")"; 
		msg.output(ss.str());
	}else{
		kgMsg msg(kgMsg::END, env);
		ostringstream ss;
		ss << "kgshell (" << v << ")"; 
		msg.output(ss.str());
	
	}
}
/*  今の所必要なし
static void watch_war_OUTPUT(const string& v,kgEnv *env,bool pymsg){
	if(pymsg){
		kgMsgIncPySys msg(kgMsg::WAR, env);
		ostringstream ss;
		ss << "kgshell (" << v << ")";
		msg.output(ss.str());
	}else{
		kgMsg msg(kgMsg::WAR, env);
		ostringstream ss;
		ss << "kgshell (" << v << ")"; 
		msg.output(ss.str());	
	}
}
static void watch_err_OUTPUT(const string& v,kgEnv *env,bool pymsg){
	if(pymsg){
		kgMsgIncPySys msg(kgMsg::ERR, env);	
		ostringstream ss;
		ss << "kgshell (" << v << ")";
		msg.output(ss.str());
	}else{
		kgMsg msg(kgMsg::ERR, env);	
		ostringstream ss;
		ss << "kgshell (" << v << ")";
		msg.output(ss.str());
	
	}
}
*/
void *kgshell::run_watch(void *arg){
	watchST *wst =(watchST*)arg;
	argST *a =wst->argst; 
	int clen = wst->clen ;
	pthread_mutex_t *stsMutex = wst->stMutex ;
	pthread_cond_t *stCond  =	wst->stCond;
	pthread_t * th_st_pp = wst->th_st_pp;
	kgEnv * env = wst->env;
	bool pymsg = wst->pymsg;
	// status check
	bool endFLG = true;	
	pthread_mutex_lock(stsMutex);
	while(1){
		int pos = 0;
		endFLG = true;
		while(pos<clen){
			if(a[pos].finflg==false){ endFLG=false;}
			else if(a[pos].outputEND==false){
				if(!a[pos].msg.empty()){
					if(a[pos].status==2){
						watch_end_OUTPUT(a[pos].msg,env,pymsg);
					}
					else{
						watch_raw_OUTPUT(a[pos].msg,env,pymsg);
					}
				}
				if(!a[pos].tag.empty()){
					watch_raw_OUTPUT("#TAG# " + a[pos].tag,env,pymsg);
				}
				a[pos].outputEND = true;
			}
			if(a[pos].status!=0&&a[pos].status!=2){
 				//エラー発生時はthread cancel
				for(int j=0;j<clen;j++){
					if(!a[j].finflg){
						pthread_cancel(th_st_pp[j]);	
					}
				}
				endFLG=true;
				//errflg = true;
				break;
			}
			pos++;
		}
		if (endFLG) break;
		pthread_cond_wait(stCond,stsMutex);
	}
	pthread_mutex_unlock(stsMutex);

	pthread_exit(0);
	return NULL;	
}

void kgshell::raw_OUTPUT(const string& v){
	if(_pymsg){
		kgMsgIncPySys msg(kgMsg::IGN, &_env);
		msg.output_ignore(v);
	}
	else{
		kgMsg msg(kgMsg::IGN, &_env);
		msg.output_ignore(v);
	
	}
}
void kgshell::end_OUTPUT(const string& v){
	if(_pymsg){
		kgMsgIncPySys msg(kgMsg::END, &_env);
		ostringstream ss;
		ss << "kgshell (" << v << ")"; 
		msg.output(ss.str());
	}
	else{
		kgMsg msg(kgMsg::END, &_env);
		ostringstream ss;
		ss << "kgshell (" << v << ")"; 
		msg.output(ss.str());
	}
}

void kgshell::war_OUTPUT(const string& v){

	if(_pymsg){
		kgMsgIncPySys msg(kgMsg::WAR, &_env);
		ostringstream ss;
		ss << "kgshell (" << v << ")";
		msg.output(ss.str());
	}
	else{
		kgMsg msg(kgMsg::WAR, &_env);
		ostringstream ss;
		ss << "kgshell (" << v << ")";
		msg.output(ss.str());
	}

}
void kgshell::err_OUTPUT(const string& v){

	if(_pymsg){
		kgMsgIncPySys msg(kgMsg::ERR, &_env);	
		ostringstream ss;
		ss << "kgshell (" << v << ")";
		msg.output(ss.str());
	}
	else{
		kgMsg msg(kgMsg::ERR, &_env);	
		ostringstream ss;
		ss << "kgshell (" << v << ")";
		msg.output(ss.str());
	}
}

void kgshell::makePipeList(vector<linkST> & plist,int iblk,bool outpipe)
{
	_ipipe_map.clear();
	_opipe_map.clear();
	_FDlist.clear();
	
	// check file LIMIT
	rlimit rlim;
	size_t pfilecnt = _spblk.getModBlkSize_M(iblk) * 32 ;
	int    chfFlg   = getrlimit(RLIMIT_NOFILE, &rlim);
	if(rlim.rlim_cur < pfilecnt ){
		rlim.rlim_cur = pfilecnt;
		chfFlg = setrlimit(RLIMIT_NOFILE, &rlim);
		if (chfFlg <0 ) { 
			throw kgError("change file limit on kgshell"); 
		}
	}

	vector<int> linklist = _spblk.getLinkBlkInfo_M(iblk);

	for(size_t j=0;j<linklist.size();j++){

		int i = linklist[j];		
		int piped[2];
		if( pipe(piped) < 0){ throw kgError("pipe open error on kgshell :("+toString(errno)+")");}
		int flags0 = fcntl(piped[0], F_GETFD);
		int flags1 = fcntl(piped[1], F_GETFD);
		fcntl(piped[0], F_SETFD, flags0 | FD_CLOEXEC);
		fcntl(piped[1], F_SETFD, flags1 | FD_CLOEXEC);
		_FDlist.push_back(piped[0]);
		_FDlist.push_back(piped[1]);

		if ( _ipipe_map.find(plist[i].toID) == _ipipe_map.end()){
			map< string,vector<int> > newmap;
			_ipipe_map[plist[i].toID] = newmap; 
		}
		if ( _ipipe_map[plist[i].toID].find(plist[i].toTP) == _ipipe_map[plist[i].toID].end()){
			vector<int> newvec;
			_ipipe_map[plist[i].toID][plist[i].toTP] = newvec;
		}
		_ipipe_map[plist[i].toID][plist[i].toTP].push_back(piped[0]);

		if ( _opipe_map.find(plist[i].frID) == _opipe_map.end()){
			map< string,vector<int> > newmap;
			_opipe_map[plist[i].frID] = newmap; 
		}
		if ( _opipe_map[plist[i].frID].find(plist[i].frTP) == _opipe_map[plist[i].frID].end()){
			vector<int> newvec;
			_opipe_map[plist[i].frID][plist[i].frTP] = newvec;
		}
		_opipe_map[plist[i].frID][plist[i].frTP].push_back(piped[1]);
	}
	if(outpipe){
		if( pipe(_csvpiped) < 0){ throw kgError("pipe open error on kgshell");}
		// pipe2(piped,O_CLOEXEC) pipe2なら省略化
		int flags0 = fcntl(_csvpiped[0], F_GETFD);
		int flags1 = fcntl(_csvpiped[1], F_GETFD);
		fcntl(_csvpiped[0], F_SETFD, flags0 | FD_CLOEXEC);
		fcntl(_csvpiped[1], F_SETFD, flags1 | FD_CLOEXEC);
	}

}

int kgshell::threadStkINIT(pthread_attr_t *pattr){

	size_t stacksize;
	char * envStr=getenv("KG_THREAD_STK");
	if(envStr!=NULL){
		stacksize = aToSizeT(envStr);
	}else{
		stacksize = KGMOD_THREAD_STK;
	}

	size_t base ;
	int ret = pthread_attr_init(pattr);
	if(ret!=0){
		err_OUTPUT("pythead init error"  );
		return 1;
	}
	pthread_attr_getstacksize(pattr,&base);
	if( stacksize > base ){
		if( pthread_attr_setstacksize(pattr,stacksize)	){
			err_OUTPUT("stack size change error ");
			return 1;
		}
	}
	return 0;
}

int kgshell::setArgStIO(
	kgmod_ioinfo_t& ioinfo,
	string& cmdname,
	map<string,vector<int> > & iopipeMap,
	int **io_p){
	
	int cnt=0;

	//件数チェック (後ろNULLはカウントしない)セットも一緒にする？
	if( ioinfo.find(cmdname) == ioinfo.end()){
		return cnt;
	}
	int dmycnt= 0;

	const char ** ikwd_st = ioinfo.find(cmdname)->second;
	const char ** ikwd = ikwd_st;
	while(**ikwd){

		if( iopipeMap.find(*ikwd) == iopipeMap.end()){
			dmycnt ++; 
		}
		else{
			size_t addcnt = iopipeMap[*ikwd].size();
			if (addcnt==0){
				dmycnt ++;
			}
			else {
				if(dmycnt!=0){
					cnt += dmycnt;
					dmycnt=0;
				}
				cnt += addcnt;
			}
		}
		ikwd++;
	}

	if(cnt==0){ return cnt;}

	// ip num セット 
	*io_p = new int[cnt];
	int pos;
	for(pos=0 ; pos < cnt ; pos++){ (*io_p)[pos] = -1; }
	pos = 0;
	dmycnt =0;
	ikwd = ikwd_st;
	while(**ikwd){
		if( iopipeMap.find(*ikwd) == iopipeMap.end()){
			dmycnt ++; 
		}
		else{
			if(iopipeMap[*ikwd].size()==0){
				dmycnt ++; 
			}
			else{
				pos += dmycnt;
			}
			for(size_t jj=0 ; jj<iopipeMap[*ikwd].size() ;jj++){
				(*io_p)[pos] = iopipeMap[*ikwd][jj];
				pos++;
			}
		}
		ikwd++;
	}

	return cnt;
}


int kgshell::runMain(
	vector<cmdCapselST> &cmds,
	vector<linkST> & plist,
	int iblk,
	bool outpipe){


	_save = PyEval_SaveThread();

	// thread attr init
	pthread_attr_t pattr;
	if ( threadStkINIT(&pattr) ){ return 1; } 
	
	makePipeList(plist,iblk,outpipe);

	bool errflg=false;

	// initlize
	_clen = _spblk.getModBlkSize_M(iblk);	
	_modlist = new kgMod*[_clen];
	_argst   = new argST[_clen];
	_runst   = new int[_clen];
	_envlist = new kgEnv*[_clen];

	vector<int> cmdlist = _spblk.getModBlkInfo_M(iblk);

	for(size_t i=0;i<cmdlist.size();i++){

		int cmdNo = cmdlist[i];
		string cmdname = cmds[cmdNo].cmdname;
		
		if ( _kgmod_map.find(cmdname) == _kgmod_map.end()){
			err_OUTPUT("not 1 kgmod "+ cmdname);
			return 1;
		}
		_envlist[i] = new kgEnv(&_env);
		_modlist[i] = _kgmod_map.find(cmdname)->second() ;
		kgArgs newArgs;
		for(size_t j=0;j<cmds[cmdNo].paralist.size();j++){
			newArgs.add(cmds[cmdNo].paralist[j]);
		}
		_modlist[i]->init(newArgs, _envlist[i]);

		_argst[i].mobj= _modlist[i];
		_argst[i].tag= cmds[cmdNo].tag;
		_argst[i].finflg = false;
		_argst[i].outputEND = false;
		_argst[i].status = 0;
		_argst[i].stMutex = &_stsMutex;
		_argst[i].stCond = &_stsCond;
		_argst[i].fobj= cmds[cmdNo].fobj;
		_argst[i].aobj= cmds[cmdNo].aobj;
		_argst[i].kobj= cmds[cmdNo].kobj;
		_argst[i].mutex = &_mutex;
		_argst[i].forkCond = &_forkCond;
		_argst[i].iniMutex = &_iniMutex;
		_argst[i].iniCond = &_iniCond;

		_argst[i].i_cnt= 0;
		_argst[i].o_cnt= 0;
		_argst[i].i_p= NULL;
		_argst[i].o_p= NULL;

		int typ = _kgmod_run.find(cmdname)->second ;
		if(typ==3){
			_argst[i].fdlist= _FDlist;
			_runst[i] = 0;
		}
		else{
			_runst[i]= 1;
		}
		_argst[i].runst= &(_runst[i]) ;

		if( _ipipe_map.find(cmdNo) == _ipipe_map.end() ){ 
			if(typ==2){
				_argst[i].i_cnt= 1;
				_argst[i].list = cmds[cmdNo].iobj;
			}
		}
		else{
			_argst[i].i_cnt = setArgStIO(_kgmod_Iinfo,cmdname,_ipipe_map[cmdNo],&(_argst[i].i_p));
		}

		if(i==0 && outpipe ){ // kgcsv用
			_argst[i].o_cnt= 1;
			_argst[i].o_p= new int[1];
			_argst[i].o_p[0]= _csvpiped[1]; 
		}
		else if( _opipe_map.find(cmdNo) == _opipe_map.end() ){ 
			if(typ==1){
				_argst[i].o_cnt= 1;
				_argst[i].mutex = &_mutex;
				_argst[i].list = cmds[cmdNo].oobj;
			}
		}
		else{
			_argst[i].o_cnt = setArgStIO(_kgmod_Oinfo,cmdname,_opipe_map[cmdNo],&(_argst[i].o_p));
		}
	}

	_th_st_pp = new pthread_t[_clen];
	_th_rtn   = new int[_clen];


	for(int i=cmdlist.size()-1;i>=0;i--){

		if(3 == _kgmod_run.find(cmds[cmdlist[i]].cmdname)->second){
			pthread_mutex_lock(&_mutex);
			_th_rtn[i] = pthread_create( &_th_st_pp[i], &pattr, kgshell::run_pyfunc ,(void*)&_argst[i]);
			pthread_cond_wait(&_forkCond,&_mutex);
			pthread_mutex_unlock(&_mutex);
		}
	}

	for(int i=cmdlist.size()-1;i>=0;i--){

		//debugARGST_OUTPUT(i);
		if(3 == _kgmod_run.find(cmds[cmdlist[i]].cmdname)->second){
			_runst[i]=1;
		}
	}

	for(int i=cmdlist.size()-1;i>=0;i--){

		//debugARGST_OUTPUT(i);
		int typ = _kgmod_run.find(cmds[cmdlist[i]].cmdname)->second ;

		if(typ==0){
			_th_rtn[i] = pthread_create( &_th_st_pp[i], &pattr, kgshell::run_func ,(void*)&_argst[i]);
		}
		else if(typ==1){
			_th_rtn[i] = pthread_create( &_th_st_pp[i], &pattr, kgshell::run_writelist ,(void*)&_argst[i]);
		}
		else if(typ==2){
			_th_rtn[i] = pthread_create( &_th_st_pp[i], &pattr, kgshell::run_readlist ,(void*)&_argst[i]);
		}
	}
	if(outpipe){
		// 呼び出しもとでキャンセルさせる
		// チェック必要ここでしても問題ない iter場合の処理
		// 別thread監視させる？
		_watchST.stMutex = &_stsMutex;
		_watchST.stCond  = &_stsCond;
		_watchST.argst   = _argst ;
		_watchST.th_st_pp = _th_st_pp;
		_watchST.clen = _clen;
		_watchST.env = &_env;
		_watchST.pymsg = _pymsg;
		PyEval_RestoreThread(_save);
		pthread_create(&_th_st_watch, &pattr, kgshell::run_watch ,(void*)&_watchST);
		_watchFlg=true;
		return _csvpiped[0];
	}
	//_save = PyEval_SaveThread();
	// status check

	pthread_mutex_lock(&_stsMutex);

	while(1){
		size_t pos = 0;
		bool endFLG = true;
		while(pos<_clen){
			if(_argst[pos].finflg==false){ endFLG=false;}
			else if(_argst[pos].outputEND==false){

				PyEval_RestoreThread(_save);

				if(!_argst[pos].msg.empty()){
					if(_argst[pos].status==2){
						err_OUTPUT(_argst[pos].msg);
					}
					else{
						raw_OUTPUT(_argst[pos].msg);
					}
				}
				if(!_argst[pos].tag.empty()){
					raw_OUTPUT("#TAG# " + _argst[pos].tag);
				}
				_save = PyEval_SaveThread();
				_argst[pos].outputEND = true;

			}
			if(_argst[pos].status!=0&&_argst[pos].status!=2){
 				//エラー発生時はthread cancel した方がいい？ <<=ややこしくなりそうなので保留
 				/*
				for(size_t j=0;j<_clen;j++){
					if(!_argst[j].finflg){
						pthread_cancel(_th_st_pp[j]);	
					}
				}*/
				endFLG=true;
				errflg = true;
				break;
			}
			pos++;
		}
		if (endFLG) break;
		pthread_cond_wait(&_stsCond,&_stsMutex);
	}

	pthread_mutex_unlock(&_stsMutex);
	for(size_t i=_clen;i>0;i--){
		int ret;
		int status;
		ret = pthread_join(_th_st_pp[i-1],(void**)&status);
	}

	if (!outpipe){ 
		PyEval_RestoreThread(_save);
	}
	

	if(_modlist){
		for(size_t i=0 ;i<_clen;i++){
			try {
				if(_argst[i].outputEND == false){
					if(!_argst[i].msg.empty()){
						if(_argst[i].status==2){
							err_OUTPUT(_argst[i].msg);
						}
						else{
							raw_OUTPUT(_argst[i].msg);
						}
					}
					if(!_argst[i].tag.empty()){
						raw_OUTPUT("#TAG# " + _argst[i].tag);
					}
				}
				_argst[i].outputEND = true;
				delete _modlist[i];
				delete _envlist[i];
				_modlist[i] =NULL;
				_envlist[i] =NULL;
			}
			catch(kgError& err){
				err_OUTPUT("script RUN KGERROR " + err.message(0));
				errflg = true;
			}
			catch(...){ 
				err_OUTPUT("closing.. ");
				errflg = true;
			}
		}
		delete[] _modlist;
		delete[] _envlist;
		_modlist = NULL;
		_envlist =NULL;
	}

	if(_argst){
		for(size_t i=0 ;i<_clen;i++){
			if(_argst[i].i_cnt!=0){
				delete [] _argst[i].i_p;
			}
			if(_argst[i].o_cnt!=0){
				delete [] _argst[i].o_p;
			}
		}
		delete [] _argst; 
		_argst =NULL;
	}
	if(_th_st_pp){
		delete[] _th_st_pp;
		_th_st_pp = NULL;
	}
	if(_th_rtn){
		delete[] _th_rtn;
		_th_rtn = NULL;
	}
	if(_runst){
		delete[] _runst;
		_runst = NULL;
	}
	if (errflg) { throw kgError("runmain on kgshell"); }
	return 0;
}

void kgshell::runInit(
	vector<cmdCapselST> &cmds,	
	vector<linkST> & plist
){

	//if(!Py_IsInitialized()){
	//	Py_Initialize();
	//}
	//if (!PyEval_ThreadsInitialized())	{ 
	//	PyEval_InitThreads();
	//}
	
	// set block size  &  make run block 
	if ( _runlim == -1){
		char * envStr = getenv("KG_RUN_LIMIT");
		if(envStr!=NULL)	{ _runlim = atoi(envStr); }
		else							{ 
			_runlim = _memttl / KGMOD_MEMBASE;
			if(_runlim > KGMOD_RUN_LIMIT){
				_runlim = KGMOD_RUN_LIMIT;
			}//ファイルリミットでもチェックする
		}

	}
	if ( _runlim <=0 ){
		throw kgError("not valid runlimit");
		return;
	}
	_spblk.blockSplit(_runlim,cmds.size(),plist);

	// パラメータ UPDATE
	vector<linkST>&spedge = _spblk.getsplitEdge();
	for(size_t i=0;i<spedge.size();i++){

		kgstr_t tp = _tempFile.create(false,"kgshellspilt");

		int pos = -1;
		for(size_t j=0;j<cmds[spedge[i].frID].paralist.size();j++){
			if(cmds[spedge[i].frID].paralist[j].find(spedge[i].frTP+"=")==0){
				pos = j; break;
			} 
		} 
		
		if(pos==-1){
			cmds[spedge[i].frID].paralist.push_back(spedge[i].frTP+"="+tp);
		}
		else{
			kgstr_t newpara = cmds[spedge[i].frID].paralist[pos] + "," + tp;
			cmds[spedge[i].frID].paralist[pos]=newpara;
		}

		pos = -1;
		for(size_t j=0;j<cmds[spedge[i].toID].paralist.size();j++){
			if(cmds[spedge[i].toID].paralist[j].find(spedge[i].toTP+"=")==0){
				pos = j;
				break;
			} 
		} 
		if(pos==-1){
			cmds[spedge[i].toID].paralist.push_back(spedge[i].toTP+"="+tp);
		}
		else{
			kgstr_t newpara = cmds[spedge[i].toID].paralist[pos]+","+ tp ;
			cmds[spedge[i].toID].paralist[pos]=newpara;
		}
	}
	
	return ;
}


int kgshell::runx(
	vector<cmdCapselST> &cmds,	
	vector<linkST> & plist
)
{
	try{

		runInit(cmds,plist);

		for(int iblk=0;iblk<_spblk.getBlksize_M();iblk++){
			runMain(cmds,plist,iblk);
		}
		return 0;

	}catch(kgError& err){
		ostringstream ss;
		ss << "script RUN KGERROR " << err.message(0);
		err_OUTPUT(ss.str());
		runClean();
		
		PyErr_SetString(PyExc_RuntimeError,err.message(0).c_str());
		PyErr_Print();

	}catch (const exception& e) {
		ostringstream ss;
		ss << "script RUN KGERROR " << e.what();
		err_OUTPUT(ss.str());
		runClean();

	}catch(char * er){
		ostringstream ss;
		ss << "script RUN EX ERR " << er;
		err_OUTPUT(ss.str());
		runClean();

	}catch(...){	
		err_OUTPUT("script RUN ERROR UNKNOWN TYPE");
		runClean();
	}
	return 1;
}


kgCSVfld* kgshell::runiter(
	vector<cmdCapselST> &cmds,	
	vector<linkST> & plist
){
	kgCSVfld* _iterrtn = NULL;
	try{

		runInit(cmds,plist);
		int itrfd = -1;
		for(int iblk=0;iblk<_spblk.getBlksize_M();iblk++){
			itrfd = runMain(cmds,plist,iblk,iblk==_spblk.getBlksize_M()-1);
		}
		if(itrfd<0){ return NULL; }
		// データ出力
		_iterrtn = new kgCSVfld;
		_iterrtn->popen(itrfd, &_env,_nfni);
		_iterrtn->read_header();	
		return _iterrtn;

	}catch(kgError& err){

		if(_iterrtn==NULL) { delete _iterrtn; } 		
		ostringstream ss;
		ss << "script RUN KGERROR " << err.message(0);
		err_OUTPUT(ss.str());
		runClean();

	}catch (const exception& e) {
		if(_iterrtn==NULL) { delete _iterrtn; } 		
		ostringstream ss;
		ss << "script RUN KGERROR " << e.what();
		err_OUTPUT(ss.str());
		runClean();

	}catch(char * er){
		if(_iterrtn==NULL) { delete _iterrtn; } 		
		ostringstream ss;
		ss << "script RUN EX ERR " << er;
		err_OUTPUT(ss.str());
		runClean();

	}catch(...){	
		if(_iterrtn==NULL) { delete _iterrtn; } 		
		err_OUTPUT("script RUN ERROR UNKNOWN TYPE");
		runClean();
	}

	return NULL;
}


kgCSVkey* kgshell::runkeyiter(
	vector<cmdCapselST> &cmds,	
	vector<linkST> & plist,
	vector<string> & klist
){
	kgCSVkey *_iterrtnk=NULL;
	try{

		runInit(cmds,plist);
		int itrfd=-1;
		for(int iblk=0;iblk<_spblk.getBlksize_M();iblk++){
			itrfd = runMain(cmds,plist,iblk,iblk==_spblk.getBlksize_M()-1);
		}
		if(itrfd<0){ return NULL; }

		// データ出力
		_iterrtnk = new kgCSVkey;
 
		_iterrtnk->popen(itrfd, &_env,_nfni);
		_iterrtnk->read_header();	
		kgArgFld fField;
		fField.set(klist, _iterrtnk, false);
		// 入力ファイルにkey項目番号をセットする．
		_iterrtnk->setKey(fField.getNum());

		return _iterrtnk;

	}catch(kgError& err){
		if(_iterrtnk==NULL) { delete _iterrtnk; } 		
		ostringstream ss;
		ss << "script RUN KGERROR " << err.message(0);
		err_OUTPUT(ss.str());
		runClean();

	}catch (const exception& e) {
		if(_iterrtnk==NULL) { delete _iterrtnk; } 		
		ostringstream ss;
		ss << "script RUN KGERROR " << e.what();
		err_OUTPUT(ss.str());
		runClean();

	}catch(char * er){
		if(_iterrtnk==NULL) { delete _iterrtnk; } 		
		ostringstream ss;
		ss << "script RUN EX ERR " << er;
		err_OUTPUT(ss.str());
		runClean();

	}catch(...){	
		if(_iterrtnk==NULL) { delete _iterrtnk; } 		
		err_OUTPUT("script RUN ERROR UNKNOWN TYPE");
		runClean();
	}

	return NULL;

}


int kgshell::getparams( kgstr_t cmdname, PyObject* list){

	kgMod *mod =NULL;

	try{
		if ( _kgmod_map.find(cmdname) == _kgmod_map.end()){
			err_OUTPUT("Not unspport mod " + cmdname);
			return 1;	
		}

		kgArgs newArgs;
		mod	= _kgmod_map.find(cmdname)->second();
		mod->init(newArgs, &_env);
		vector < vector <kgstr_t> > paralist = mod->params();

		for (size_t i=0;i<paralist.size();i++){
			PyObject* tlist = PyList_New(paralist[i].size());
			for(size_t j=0 ;j<paralist[i].size();j++){
				PyList_SetItem(tlist,j,Py_BuildValue("s",paralist[i][j].c_str()));
			}
			PyList_Append(list,tlist);
			Py_DECREF(tlist);
			
		}
		if(mod) delete mod;
		return 0;

	}catch(...){
		err_OUTPUT("UnKnown ERROR IN GET PARAMETER " );
		return 1;	
	}
	return 1;

}

int kgshell::getIoInfo(kgstr_t cmdname,PyObject* list,int iotp){
	
	kgmod_ioinfo_t * IOinfo_tmp;	
	if(iotp == 0){
		IOinfo_tmp = &_kgmod_Iinfo;
	}
	else{
		IOinfo_tmp = &_kgmod_Oinfo;
	}
	
	try{
		if ( IOinfo_tmp->find(cmdname) == IOinfo_tmp->end()){
			err_OUTPUT("Not unspport mod " + cmdname);
			return 1;	
		}

		const char ** args	= IOinfo_tmp->find(cmdname)->second;
		while(**args){
			PyList_Append(list,Py_BuildValue("s",*args));
			args++;
		}		
		return 0;

	}catch(...){
		err_OUTPUT("UnKnown ERROR IN GET IOINFO " );
		return 1;	
	}
	return 1;	
	

}


