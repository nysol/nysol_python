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
#include <kgshell.h>
#include <kgTempfile.h>
#include <pthread.h>
#include <sys/time.h>
#include <sys/resource.h>

using namespace kgmod;
using namespace kglib;


kgshell::kgshell(int mflg){

		_kgmod_map["m2tee"] = boost::lambda::bind(boost::lambda::new_ptr<kg2Tee>());
		_kgmod_map["mfifo"] = boost::lambda::bind(boost::lambda::new_ptr<kgFifo>());
		_kgmod_map["mcut"] = boost::lambda::bind(boost::lambda::new_ptr<kgCut>());
		_kgmod_map["cmd"] = boost::lambda::bind(boost::lambda::new_ptr<kgExcmd>());
		_kgmod_map["runfunc"] = boost::lambda::bind(boost::lambda::new_ptr<kgPyfunc>());
		_kgmod_map["mcat"] = boost::lambda::bind(boost::lambda::new_ptr<kgCat>());
		_kgmod_map["msum"] = boost::lambda::bind(boost::lambda::new_ptr<kgSum>());
		_kgmod_map["mcal"] = boost::lambda::bind(boost::lambda::new_ptr<kgCal>());
		_kgmod_map["mjoin"] = boost::lambda::bind(boost::lambda::new_ptr<kgJoin>());
		_kgmod_map["m2cross"] = boost::lambda::bind(boost::lambda::new_ptr<kg2Cross>());
		_kgmod_map["maccum"] = boost::lambda::bind(boost::lambda::new_ptr<kgAccum>());
		_kgmod_map["mavg"] = boost::lambda::bind(boost::lambda::new_ptr<kgAvg>());
		_kgmod_map["mbest"] = boost::lambda::bind(boost::lambda::new_ptr<kgBest>());
		_kgmod_map["mbucket"] = boost::lambda::bind(boost::lambda::new_ptr<kgBucket>());
		_kgmod_map["mchgnum"] = boost::lambda::bind(boost::lambda::new_ptr<kgChgnum>());
		_kgmod_map["mchgstr"] = boost::lambda::bind(boost::lambda::new_ptr<kgChgstr>());
		_kgmod_map["mcombi"] = boost::lambda::bind(boost::lambda::new_ptr<kgCombi>());
		_kgmod_map["mcommon"] = boost::lambda::bind(boost::lambda::new_ptr<kgCommon>());
		_kgmod_map["mcount"] = boost::lambda::bind(boost::lambda::new_ptr<kgCount>());
		_kgmod_map["mcross"] = boost::lambda::bind(boost::lambda::new_ptr<kgCross>());
		_kgmod_map["mdelnull"] = boost::lambda::bind(boost::lambda::new_ptr<kgDelnull>());
		_kgmod_map["mdformat"] = boost::lambda::bind(boost::lambda::new_ptr<kgDformat>());
		_kgmod_map["mduprec"] = boost::lambda::bind(boost::lambda::new_ptr<kgDuprec>());
		_kgmod_map["mfldname"] = boost::lambda::bind(boost::lambda::new_ptr<kgFldname>());
		_kgmod_map["mfsort"] = boost::lambda::bind(boost::lambda::new_ptr<kgFsort>());
		_kgmod_map["mhashavg"] = boost::lambda::bind(boost::lambda::new_ptr<kgHashavg>());
		_kgmod_map["mhashsum"] = boost::lambda::bind(boost::lambda::new_ptr<kgHashsum>());
		_kgmod_map["mkeybreak"] = boost::lambda::bind(boost::lambda::new_ptr<kgKeybreak>());
		_kgmod_map["mmbucket"] = boost::lambda::bind(boost::lambda::new_ptr<kgMbucket>());
		_kgmod_map["mmvavg"] = boost::lambda::bind(boost::lambda::new_ptr<kgMvavg>());
		_kgmod_map["mmvsim"] = boost::lambda::bind(boost::lambda::new_ptr<kgMvsim>());
		_kgmod_map["mmvstats"] = boost::lambda::bind(boost::lambda::new_ptr<kgMvstats>());
		_kgmod_map["mnewnumber"] = boost::lambda::bind(boost::lambda::new_ptr<kgNewnumber>());
		_kgmod_map["mnewrand"] = boost::lambda::bind(boost::lambda::new_ptr<kgNewrand>());
		_kgmod_map["mnewstr"] = boost::lambda::bind(boost::lambda::new_ptr<kgNewstr>());
		_kgmod_map["mnjoin"] = boost::lambda::bind(boost::lambda::new_ptr<kgNjoin>());
		_kgmod_map["mnormalize"] = boost::lambda::bind(boost::lambda::new_ptr<kgNormalize>());
		_kgmod_map["mnrcommon"] = boost::lambda::bind(boost::lambda::new_ptr<kgNrcommon>());
		_kgmod_map["mnrjoin"] = boost::lambda::bind(boost::lambda::new_ptr<kgNrjoin>());
		_kgmod_map["mnullto"] = boost::lambda::bind(boost::lambda::new_ptr<kgNullto>());
		_kgmod_map["mnumber"] = boost::lambda::bind(boost::lambda::new_ptr<kgNumber>());
		_kgmod_map["mpadding"] = boost::lambda::bind(boost::lambda::new_ptr<kgPadding>());
		_kgmod_map["mpaste"] = boost::lambda::bind(boost::lambda::new_ptr<kgPaste>());
		_kgmod_map["mproduct"] = boost::lambda::bind(boost::lambda::new_ptr<kgProduct>());
		_kgmod_map["mrand"] = boost::lambda::bind(boost::lambda::new_ptr<kgRand>());
		_kgmod_map["mrjoin"] = boost::lambda::bind(boost::lambda::new_ptr<kgRjoin>());
		_kgmod_map["msed"] = boost::lambda::bind(boost::lambda::new_ptr<kgSed>());
		_kgmod_map["msel"] = boost::lambda::bind(boost::lambda::new_ptr<kgSel>());
		_kgmod_map["mselnum"] = boost::lambda::bind(boost::lambda::new_ptr<kgSelnum>());
		_kgmod_map["mselrand"] = boost::lambda::bind(boost::lambda::new_ptr<kgSelrand>());
		_kgmod_map["mselstr"] = boost::lambda::bind(boost::lambda::new_ptr<kgSelstr>());
		_kgmod_map["msetstr"] = boost::lambda::bind(boost::lambda::new_ptr<kgSetstr>());
		_kgmod_map["mshare"] = boost::lambda::bind(boost::lambda::new_ptr<kgShare>());
		_kgmod_map["msim"] = boost::lambda::bind(boost::lambda::new_ptr<kgSim>());
		_kgmod_map["mslide"] = boost::lambda::bind(boost::lambda::new_ptr<kgSlide>());
		_kgmod_map["msplit"] = boost::lambda::bind(boost::lambda::new_ptr<kgSplit>());
		_kgmod_map["mstats"] = boost::lambda::bind(boost::lambda::new_ptr<kgStats>());
		_kgmod_map["msummary"] = boost::lambda::bind(boost::lambda::new_ptr<kgSummary>());
		_kgmod_map["mtonull"] = boost::lambda::bind(boost::lambda::new_ptr<kgTonull>());
		_kgmod_map["mtra"] = boost::lambda::bind(boost::lambda::new_ptr<kgTra>());
		_kgmod_map["mtraflg"] = boost::lambda::bind(boost::lambda::new_ptr<kgTraflg>());
		_kgmod_map["mtrafld"] = boost::lambda::bind(boost::lambda::new_ptr<kgTrafld>());
		_kgmod_map["muniq"] = boost::lambda::bind(boost::lambda::new_ptr<kgUniq>());
		_kgmod_map["mvcat"] = boost::lambda::bind(boost::lambda::new_ptr<kgVcat>());
		_kgmod_map["mvcommon"] = boost::lambda::bind(boost::lambda::new_ptr<kgVcommon>());
		_kgmod_map["mvcount"] = boost::lambda::bind(boost::lambda::new_ptr<kgVcount>());
		_kgmod_map["mvdelim"] = boost::lambda::bind(boost::lambda::new_ptr<kgVdelim>());
		_kgmod_map["mvdelnull"] = boost::lambda::bind(boost::lambda::new_ptr<kgVdelnull>());
		_kgmod_map["mvjoin"] = boost::lambda::bind(boost::lambda::new_ptr<kgVjoin>());
		_kgmod_map["mvnullto"] = boost::lambda::bind(boost::lambda::new_ptr<kgVnullto>());
		_kgmod_map["mvreplace"] = boost::lambda::bind(boost::lambda::new_ptr<kgVreplace>());
		_kgmod_map["mvsort"] = boost::lambda::bind(boost::lambda::new_ptr<kgVsort>());
		_kgmod_map["mvuniq"] = boost::lambda::bind(boost::lambda::new_ptr<kgVuniq>());
		_kgmod_map["mwindow"] = boost::lambda::bind(boost::lambda::new_ptr<kgWindow>());
		_kgmod_map["marff2csv"] = boost::lambda::bind(boost::lambda::new_ptr<kgArff2csv>());
		_kgmod_map["mxml2csv"] = boost::lambda::bind(boost::lambda::new_ptr<kgXml2csv>());
		_kgmod_map["msortf"]    = boost::lambda::bind(boost::lambda::new_ptr<kgSortf>());
		_kgmod_map["mtab2csv"]  = boost::lambda::bind(boost::lambda::new_ptr<kgTab2csv>());
		_kgmod_map["msep"]  = boost::lambda::bind(boost::lambda::new_ptr<kgSep>());
		_kgmod_map["mshuffle"]  = boost::lambda::bind(boost::lambda::new_ptr<kgShuffle>());

		_kgmod_map["m2cat"]  = boost::lambda::bind(boost::lambda::new_ptr<kg2Cat>());


		_kgmod_map["writelist"] = boost::lambda::bind(boost::lambda::new_ptr<kgLoad>());
		_kgmod_map["readlist"] = boost::lambda::bind(boost::lambda::new_ptr<kgLoad>());
		_kgmod_map["readcsv"] = boost::lambda::bind(boost::lambda::new_ptr<kgCat>());
		_kgmod_map["writecsv"] = boost::lambda::bind(boost::lambda::new_ptr<kgLoad>());

		_kgmod_map["mstdin"] = boost::lambda::bind(boost::lambda::new_ptr<kgLoad>());
		_kgmod_map["mstdout"] = boost::lambda::bind(boost::lambda::new_ptr<kgLoad>());

		_kgmod_run["m2tee"] = 0;
		_kgmod_run["mfifo"] = 0;
		_kgmod_run["mcut"] = 0;
		_kgmod_run["cmd"] = 0;
		_kgmod_run["mcat"] = 0;
		_kgmod_run["msum"] = 0;
		_kgmod_run["mcal"] = 0;
		_kgmod_run["mjoin"] = 0;
		_kgmod_run["m2cross"] = 0;
		_kgmod_run["maccum"] = 0;
		_kgmod_run["mavg"] = 0;
		_kgmod_run["mbest"] = 0;
		_kgmod_run["mbucket"] = 0;
		_kgmod_run["mchgnum"] = 0;
		_kgmod_run["mchgstr"] = 0;
		_kgmod_run["mcombi"] = 0;
		_kgmod_run["mcommon"] = 0;
		_kgmod_run["mcount"] = 0;
		_kgmod_run["mcross"] = 0;
		_kgmod_run["mdelnull"] = 0;
		_kgmod_run["mdformat"] = 0;
		_kgmod_run["mduprec"] = 0;
		_kgmod_run["mfldname"] = 0;
		_kgmod_run["mfsort"] = 0;
		_kgmod_run["mhashavg"] = 0;
		_kgmod_run["mhashsum"] = 0;
		_kgmod_run["mkeybreak"] = 0;
		_kgmod_run["mmbucket"] = 0;
		_kgmod_run["mmvavg"] = 0;
		_kgmod_run["mmvsim"] = 0;
		_kgmod_run["mmvstats"] = 0;
		_kgmod_run["mnewnumber"] = 0;
		_kgmod_run["mnewrand"] = 0;
		_kgmod_run["mnewstr"] = 0;
		_kgmod_run["mnjoin"] = 0;
		_kgmod_run["mnormalize"] = 0;
		_kgmod_run["mnrcommon"] = 0;
		_kgmod_run["mnrjoin"] = 0;
		_kgmod_run["mnullto"] = 0;
		_kgmod_run["mnumber"] = 0;
		_kgmod_run["mpadding"] = 0;
		_kgmod_run["mpaste"] = 0;
		_kgmod_run["mproduct"] = 0;
		_kgmod_run["mrand"] = 0;
		_kgmod_run["mrjoin"] = 0;
		_kgmod_run["msed"] = 0;
		_kgmod_run["msel"] = 0;
		_kgmod_run["mselnum"] = 0;
		_kgmod_run["mselrand"] = 0;
		_kgmod_run["mselstr"] = 0;
		_kgmod_run["msep"] = 0;
		_kgmod_run["mshuffle"] = 0;
		_kgmod_run["msetstr"] = 0;
		_kgmod_run["mshare"] = 0;
		_kgmod_run["msim"] = 0;
		_kgmod_run["mslide"] = 0;
		_kgmod_run["msplit"] = 0;
		_kgmod_run["mstats"] = 0;
		_kgmod_run["msummary"] = 0;
		_kgmod_run["mtonull"] = 0;
		_kgmod_run["mtra"] = 0;
		_kgmod_run["mtraflg"] = 0;
		_kgmod_run["mtrafld"] = 0;
		_kgmod_run["muniq"] = 0;
		_kgmod_run["mvcat"] = 0;
		_kgmod_run["mvcommon"] = 0;
		_kgmod_run["mvcount"] = 0;
		_kgmod_run["mvdelim"] = 0;
		_kgmod_run["mvdelnull"] = 0;
		_kgmod_run["mvjoin"] = 0;
		_kgmod_run["mvnullto"] = 0;
		_kgmod_run["mvreplace"] = 0;
		_kgmod_run["mvsort"] = 0;
		_kgmod_run["mvuniq"] = 0;
		_kgmod_run["mwindow"] = 0;
		_kgmod_run["marff2csv"] =0;
		_kgmod_run["mxml2csv"] =0;
		_kgmod_run["msortf"] =0;
		_kgmod_run["mtab2csv"] = 0;
		_kgmod_run["writecsv"] = 0;
		_kgmod_run["readcsv"] = 0;
		_kgmod_run["runfunc"] = 3;
		_kgmod_run["mstdin"] = 0;
		_kgmod_run["mstdout"] = 0;
		_kgmod_run["m2cat"] = 0;



		_kgmod_run["mload"] = 0;
		_kgmod_run["msave"] = 0;
		_kgmod_run["writelist"] = 1;
		_kgmod_run["readlist"] = 2;


		_nfni = false;
 		_iterrtn= NULL;
 		_iterrtnk= NULL;
		_th_st_pp = NULL;
		_clen = 0;
		_modlist=NULL;

		if(!mflg){  _env.verblvl(2);	}

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

	  if (pthread_cond_init(&_stsCond, NULL) == -1) { 
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
		a->msg.append("unKnown ERROR ");
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
		a->msg.append("unKnown ERROR ");
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
		a->msg.append("unKnown ERROR ");
		a->msg.append(a->mobj->name());
		a->msg.append(" ");
		a->msg.append(er);
		pthread_cond_signal(a->stCond);
		pthread_mutex_unlock(a->stMutex);

	}catch(...){
		argST *a =(argST*)arg; 
		pthread_mutex_lock(a->stMutex);
		a->status = 1;
		a->finflg=true;
		a->endtime=getNowTime(true);
		a->msg.append("unKnown ERROR ");
		a->msg.append(a->mobj->name());
		pthread_cond_signal(a->stCond);
		pthread_mutex_unlock(a->stMutex);
	}

	pthread_exit(0);

	return NULL;	
}

void *kgshell::run_writelist(void *arg){
	try{
		string msg;
		argST *a =(argST*)arg; 
		int sts = a->mobj->run(a->i_cnt,a->i_p,a->list,a->mutex,msg);
		pthread_mutex_lock(a->stMutex);
		a->status = sts;
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
		a->msg.append("unKnown ERROR");
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
		a->msg.append("unKnown ERROR");
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
		a->msg.append("unKnown ERROR");
		a->msg.append(a->mobj->name());
		a->msg.append(" ");
		a->msg.append(er);
		pthread_cond_signal(a->stCond);
		pthread_mutex_unlock(a->stMutex);

	}catch(...){
		argST *a =(argST*)arg; 
		pthread_mutex_lock(a->stMutex);
		a->status = 1;
		a->finflg=true;
		a->endtime=getNowTime(true);
		a->msg.append("unKnown ERROR");
		a->msg.append(a->mobj->name());
		a->msg.append(" ");
		pthread_cond_signal(a->stCond);
		pthread_mutex_unlock(a->stMutex);
	}
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
		a->msg.append("unKnown ERROR");
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
		a->msg.append("unKnown ERROR");
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
		a->msg.append("unKnown ERROR");
		a->msg.append(a->mobj->name());
		a->msg.append(" ");
		a->msg.append(er);
		pthread_cond_signal(a->stCond);
		pthread_mutex_unlock(a->stMutex);

	}catch(...){
		argST *a =(argST*)arg; 
		pthread_mutex_lock(a->stMutex);
		a->status = 1;
		a->finflg=true;
		a->endtime=getNowTime(true);
		a->msg.append("unKnown ERROR");
		a->msg.append(a->mobj->name());
		pthread_cond_signal(a->stCond);
		pthread_mutex_unlock(a->stMutex);
	}
	return NULL;	
}



void *kgshell::run_pyfunc(void *arg){
	try{
		string msg;
		argST *a =(argST*)arg; 

		//PyGILState_STATE gstate;
		//cerr << "run_pyfunc:0" << endl;
		//gstate = PyGILState_Ensure();
		//cerr << "run_pyfunc:1" << endl;
		int sts = a->mobj->run(a->fobj,a->aobj,a->kobj,a->i_cnt,a->i_p,a->o_cnt,a->o_p,msg,a->mutex,a->fdlist);
		//cerr << "run_pyfunc:2" << endl;
		//PyGILState_Release(gstate);
		//cerr << "run_pyfunc:3" << endl;

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
		a->msg.append("unKnown ERROR");
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
		a->msg.append("unKnown ERROR");
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
		a->msg.append("unKnown ERROR");
		a->msg.append(a->mobj->name());
		a->msg.append(" ");
		a->msg.append(er);
		pthread_cond_signal(a->stCond);
		pthread_mutex_unlock(a->stMutex);

	}catch(...){
		argST *a =(argST*)arg; 
		pthread_mutex_lock(a->stMutex);
		a->status = 1;
		a->finflg=true;
		a->endtime=getNowTime(true);
		a->msg.append("unKnown ERROR");
		a->msg.append(a->mobj->name());
		pthread_cond_signal(a->stCond);
		pthread_mutex_unlock(a->stMutex);
	}
	return NULL;	
}


void kgshell::makePipeList(vector<linkST> & plist,int iblk)
{

	_ipipe_map.clear();
	_opipe_map.clear();
	_FDlist.clear();
	
	int pcnt = _spblk.getLinkBlkSize(iblk);
	rlimit rlim;
	int chfFlg;
	chfFlg = getrlimit(RLIMIT_NOFILE, &rlim);
	size_t pfilecnt = pcnt * 32 ;
	if(rlim.rlim_cur < pfilecnt ){
		rlim.rlim_cur = pfilecnt;
		chfFlg = setrlimit(RLIMIT_NOFILE, &rlim);
		if (chfFlg <0 ) { throw kgError("change file limit on kgshell"); } 
	}

	vector<int> linklist = _spblk.getLinkBlkInfo(iblk);

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


		/*linkST{ kgstr_t frTP; int frID; kgstr_t toTP; int toID;};*/
		//typedef map<int, map<string,vector<int> > > iomap_t;
		
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
}


int kgshell::runMain(vector<cmdCapselST> &cmds,vector<linkST> & plist,int iblk){

	pthread_attr_t pattr;
	char * envStr=getenv("KG_THREAD_STK");

	size_t stacksize;
	if(envStr!=NULL){
		stacksize = aToSizeT(envStr);
	}else{
		stacksize = KGMOD_THREAD_STK;
	}

	size_t base ;
	int ret = pthread_attr_init(&pattr);
	pthread_attr_getstacksize(&pattr,&base);

	if( stacksize > base ){
		if( pthread_attr_setstacksize(&pattr,stacksize)	){
			cerr << "stack size change error " << endl;
		}
	}

	makePipeList(plist,iblk);

	_clen = _spblk.getModBlkSize(iblk);
	
	//debugIOinfo_OUTPUT(); //DEBUG
	//_clen = cmds.size();
	_modlist = new kgMod*[_clen];
	vector<int> cmdlist = _spblk.getModBlkInfo(iblk);

	
	int clenpos= 0;
	for(size_t j=0;j<cmdlist.size();j++){

		int i = cmdlist[j];
		if ( _kgmod_map.find(cmds[i].cmdname) == _kgmod_map.end()){
			cerr << "not 1 kgmod " << cmds[i].cmdname << endl;
			return 1;
		}
		_modlist[clenpos] = _kgmod_map.find(cmds[i].cmdname)->second() ;
		kgArgs newArgs;
		for(size_t j=0;j<cmds[i].paralist.size();j++){
			newArgs.add(cmds[i].paralist[j]);
		}
		_modlist[clenpos]->init(newArgs, &_env);
		clenpos++;
	}


	_th_st_pp = new pthread_t[clenpos];
	_argst = new argST[clenpos];
	int _th_rtn[clenpos];
	int clenpos_a = clenpos;
	// before run 


	for(int j=cmdlist.size()-1;j>=0;j--){

		int i=cmdlist[j];

		clenpos_a--;

		_argst[clenpos_a].mobj= _modlist[clenpos_a];
		_argst[clenpos_a].tag= cmds[i].tag;
		_argst[clenpos_a].finflg = false;
		_argst[clenpos_a].outputEND = false;
		_argst[clenpos_a].status = 0;
		_argst[clenpos_a].stMutex = &_stsMutex;
		_argst[clenpos_a].stCond = &_stsCond;
		_argst[clenpos_a].fobj= cmds[i].fobj;
		_argst[clenpos_a].aobj= cmds[i].aobj;
		_argst[clenpos_a].kobj= cmds[i].kobj;
		_argst[clenpos_a].mutex = &_mutex;

		int typ = _kgmod_run.find(cmds[i].cmdname)->second ;
		if(typ==3){
			_argst[clenpos_a].fdlist= _FDlist;
		}

		if( _ipipe_map.find(i) == _ipipe_map.end() ){ 
			if(typ==2){
				_argst[clenpos_a].i_cnt= 1;
				_argst[clenpos_a].i_p= NULL;
				_argst[clenpos_a].list = cmds[i].iobj;
			}
			else{
				_argst[clenpos_a].i_cnt= 0;
				_argst[clenpos_a].i_p= NULL;
			}
		}
		else{
			// ここは今のところ固定//全パラメータやる必要＆パラメータ順位をkgmodから
			size_t cnt=0;
			if( _ipipe_map[i].find("i") != _ipipe_map[i].end()){
				cnt += _ipipe_map[i]["i"].size();
			}
			if( _ipipe_map[i].find("m") != _ipipe_map[i].end()){
				cnt += _ipipe_map[i]["m"].size();
				if(cnt==1) { cnt++; } //mのみの場合はdmy追加 
			}
			if(cnt==0){
				_argst[clenpos_a].i_cnt= 0;
				_argst[clenpos_a].i_p= NULL;
			}
			else{
				_argst[clenpos_a].i_cnt= cnt;
				_argst[clenpos_a].i_p= new int[cnt];
				size_t pos = 0;
				if( _ipipe_map[i].find("i") != _ipipe_map[i].end()){
					for(size_t jj=0;jj<_ipipe_map[i]["i"].size();jj++){
						_argst[clenpos_a].i_p[pos] = _ipipe_map[i]["i"][jj];
						pos++;
					}
				}
				if( _ipipe_map[i].find("m") != _ipipe_map[i].end()){
					if(pos==0 && cnt>1){ // mのみ対応
						_argst[clenpos_a].i_p[pos]=-1; pos++;
					}
					for(size_t jj=0;jj<_ipipe_map[i]["m"].size();jj++){
						_argst[clenpos_a].i_p[pos] = _ipipe_map[i]["m"][jj];
						pos++;
					}
				}
			}
		}
		if( _opipe_map.find(i) == _opipe_map.end() ){ 
			if(typ==1){
				_argst[clenpos_a].o_cnt= 1;
				_argst[clenpos_a].o_p = NULL;
				_argst[clenpos_a].mutex = &_mutex;
				_argst[clenpos_a].list = cmds[i].oobj;
			}
			else{
				_argst[clenpos_a].o_cnt= 0;
				_argst[clenpos_a].o_p= NULL;
			}
		}
		else{
			// ここは今のところ固定//全パラメータやる必要＆パラメータ順位をkgmodから
			size_t cnt=0;
			if( _opipe_map[i].find("o") != _opipe_map[i].end()){
				cnt += _opipe_map[i]["o"].size();
			}
			if( _opipe_map[i].find("u") != _ipipe_map[i].end()){
				cnt += _opipe_map[i]["u"].size();
			}
			if(cnt==0){
				_argst[clenpos_a].o_cnt= 0;
				_argst[clenpos_a].o_p= NULL;
			}
			else{
				_argst[clenpos_a].o_cnt= cnt;
				_argst[clenpos_a].o_p= new int[cnt];
				size_t pos = 0;
				if( _opipe_map[i].find("o") != _opipe_map[i].end()){
					for(size_t j=0;j<_opipe_map[i]["o"].size();j++){
					_argst[clenpos_a].o_p[pos] = _opipe_map[i]["o"][j];
						pos++;
					}
				}
				if( _opipe_map[i].find("u") != _opipe_map[i].end()){
					for(size_t j=0;j<_opipe_map[i]["u"].size();j++){
						_argst[clenpos_a].o_p[pos] = _opipe_map[i]["u"][j];
						pos++;
					}
				}
			}
		}

		//debug
			
		//debugARGST_OUTPUT(i);

		if(typ==0){
			_th_rtn[clenpos_a] = pthread_create( &_th_st_pp[clenpos_a], &pattr, kgshell::run_func ,(void*)&_argst[clenpos_a]);
		}
		else if(typ==1){
			_th_rtn[clenpos_a] = pthread_create( &_th_st_pp[clenpos_a], &pattr, kgshell::run_writelist ,(void*)&_argst[clenpos_a]);
		}
		else if(typ==2){
			_th_rtn[clenpos_a] = pthread_create( &_th_st_pp[clenpos_a], &pattr, kgshell::run_readlist ,(void*)&_argst[clenpos_a]);
		}
		else if(typ==3){
			_th_rtn[clenpos_a] = pthread_create( &_th_st_pp[clenpos_a], &pattr, kgshell::run_pyfunc ,(void*)&_argst[clenpos_a]);
		}
	}

	// status check
	pthread_mutex_lock(&_stsMutex);

	while(1){
		size_t pos = 0;
		bool endFLG = true;
		while(pos<clenpos){
			if(_argst[pos].finflg==false){ endFLG=false;}
			else if(_argst[pos].outputEND==false){
				if(!_argst[pos].msg.empty()){
					if(!_argst[pos].tag.empty()){
						cerr << _argst[pos].msg << " " << _argst[pos].tag << "(" << _argst[pos].endtime << ")" << endl; 
					}
					else{
						cerr << _argst[pos].msg  << endl; 					
					}
				}
				else if(!_argst[pos].tag.empty()){
					cerr  << _argst[pos].tag << "(" << _argst[pos].endtime << ")" << endl; 
				}
				_argst[pos].outputEND = true;
			}
			if(_argst[pos].status!=0){
 				//エラー発生時はthread cancel
				for(size_t j=0;j<clenpos;j++){
					if(!_argst[j].finflg){
						pthread_cancel(_th_st_pp[j]);	
					}
				}
				endFLG=true;
				break;
			}
			pos++;
		}
		if (endFLG) break;
		pthread_cond_wait(&_stsCond,&_stsMutex);
	}

	pthread_mutex_unlock(&_stsMutex);

	for(size_t i=clenpos;i>0;i--){
		pthread_join(_th_st_pp[i-1],NULL);
	}

	if(_modlist){
		for(size_t i=0 ;i<clenpos;i++){
			try {
				if(_argst[i].outputEND == false){
					if(!_argst[i].msg.empty()){
						if(!_argst[i].tag.empty()){
							cerr << _argst[i].msg << " " << _argst[i].tag << "(" << _argst[i].endtime << ")" << endl; 
						}
						else{
							cerr << _argst[i].msg  << endl; 					
						}
					}
					else if(!_argst[i].tag.empty()){
						cerr << _argst[i].tag  << "(" << _argst[i].endtime << ")" <<  endl;
					}
				}
				_argst[i].outputEND = true;
				delete _modlist[i];
				_modlist[i] =NULL;
			}
			catch(kgError& err){
				cerr << "script RUN KGERROR " << err.message(0) << endl;
			}
			catch(...){ 
				cerr  << "closing.. " << endl; 
			}
		}
		delete[] _modlist;
	}

	delete[] _th_st_pp;
	delete[] _argst;
	_th_st_pp = NULL;
	_argst = NULL;
	_modlist = NULL;

}


int kgshell::runiter_SUB(vector<cmdCapselST> &cmds,vector<linkST> & plist,int iblk){

	pthread_attr_t pattr;
	char * envStr=getenv("KG_THREAD_STK");

	size_t stacksize;
	if(envStr!=NULL){
		stacksize = aToSizeT(envStr);
	}else{
		stacksize = KGMOD_THREAD_STK;
	}

	size_t base ;
	int ret = pthread_attr_init(&pattr);
	pthread_attr_getstacksize(&pattr,&base);

	if( stacksize > base ){
		if( pthread_attr_setstacksize(&pattr,stacksize)	){
			cerr << "stack size change error " << endl;
		}
	}

	makePipeList(plist,iblk);

	if( pipe(_csvpiped) < 0){ throw kgError("pipe open error on kgshell");}
	// pipe2(piped,O_CLOEXEC) pipe2なら省略化
	int flags0 = fcntl(_csvpiped[0], F_GETFD);
	int flags1 = fcntl(_csvpiped[1], F_GETFD);
	fcntl(_csvpiped[0], F_SETFD, flags0 | FD_CLOEXEC);
	fcntl(_csvpiped[1], F_SETFD, flags1 | FD_CLOEXEC);


	_clen = _spblk.getModBlkSize(iblk);
	
	//debugIOinfo_OUTPUT(); //DEBUG
	//_clen = cmds.size();
	_modlist = new kgMod*[_clen];
	vector<int> cmdlist = _spblk.getModBlkInfo(iblk);

	
	int clenpos= 0;
	for(size_t j=0;j<cmdlist.size();j++){

		int i = cmdlist[j];
		if ( _kgmod_map.find(cmds[i].cmdname) == _kgmod_map.end()){
			cerr << "not 1 kgmod " << cmds[i].cmdname << endl;
			return 1;
		}
		_modlist[clenpos] = _kgmod_map.find(cmds[i].cmdname)->second() ;
		kgArgs newArgs;
		for(size_t j=0;j<cmds[i].paralist.size();j++){
			newArgs.add(cmds[i].paralist[j]);
		}
		_modlist[clenpos]->init(newArgs, &_env);
		clenpos++;
	}


	_th_st_pp = new pthread_t[clenpos];
	_argst = new argST[clenpos];
	int _th_rtn[clenpos];
	int clenpos_a = clenpos;
	// before run 


	for(int j=cmdlist.size()-1;j>=0;j--){

		int i=cmdlist[j];

		clenpos_a--;

		_argst[clenpos_a].mobj= _modlist[clenpos_a];
		_argst[clenpos_a].tag= cmds[i].tag;
		_argst[clenpos_a].finflg = false;
		_argst[clenpos_a].outputEND = false;
		_argst[clenpos_a].status = 0;
		_argst[clenpos_a].stMutex = &_stsMutex;
		_argst[clenpos_a].stCond = &_stsCond;
		_argst[clenpos_a].fobj= cmds[i].fobj;
		_argst[clenpos_a].aobj= cmds[i].aobj;
		_argst[clenpos_a].kobj= cmds[i].kobj;
		_argst[i].mutex = &_mutex;

		int typ = _kgmod_run.find(cmds[i].cmdname)->second ;
		if(typ==3){
			_argst[clenpos_a].fdlist= _FDlist;
		}

		if( _ipipe_map.find(i) == _ipipe_map.end() ){ 
			if(typ==2){
				_argst[clenpos_a].i_cnt= 1;
				_argst[clenpos_a].i_p= NULL;
				_argst[clenpos_a].list = cmds[i].iobj;
			}
			else{
				_argst[clenpos_a].i_cnt= 0;
				_argst[clenpos_a].i_p= NULL;
			}
		}
		else{
			// ここは今のところ固定//全パラメータやる必要＆パラメータ順位をkgmodから
			size_t cnt=0;
			if( _ipipe_map[i].find("i") != _ipipe_map[i].end()){
				cnt += _ipipe_map[i]["i"].size();
			}
			if( _ipipe_map[i].find("m") != _ipipe_map[i].end()){
				cnt += _ipipe_map[i]["m"].size();
				if(cnt==1) { cnt++; } //mのみの場合はdmy追加 
			}
			if(cnt==0){
				_argst[clenpos_a].i_cnt= 0;
				_argst[clenpos_a].i_p= NULL;
			}
			else{
				_argst[clenpos_a].i_cnt= cnt;
				_argst[clenpos_a].i_p= new int[cnt];
				size_t pos = 0;
				if( _ipipe_map[i].find("i") != _ipipe_map[i].end()){
					for(size_t j=0;j<_ipipe_map[i]["i"].size();j++){
						_argst[clenpos_a].i_p[pos] = _ipipe_map[i]["i"][j];
						pos++;
					}
				}
				if( _ipipe_map[i].find("m") != _ipipe_map[i].end()){
					if(pos==0 && cnt>1){ // mのみ対応
						_argst[clenpos_a].i_p[pos]=-1; pos++;
					}
					for(size_t j=0;j<_ipipe_map[i]["m"].size();j++){
						_argst[clenpos_a].i_p[pos] = _ipipe_map[i]["m"][j];
						pos++;
					}
				}
			}
		}
		if(i==0){ // kgcsv用
			_argst[i].o_cnt= 1;
			_argst[i].o_p= new int[1];
			_argst[i].o_p[0]= _csvpiped[1]; 
		} 
		else if( _opipe_map.find(i) == _opipe_map.end() ){ 
			if(typ==1){
				_argst[clenpos_a].o_cnt= 1;
				_argst[clenpos_a].o_p = NULL;
				_argst[clenpos_a].mutex = &_mutex;
				_argst[clenpos_a].list = cmds[i].oobj;
			}
			else{
				_argst[clenpos_a].o_cnt= 0;
				_argst[clenpos_a].o_p= NULL;
			}
		}
		else{
			// ここは今のところ固定//全パラメータやる必要＆パラメータ順位をkgmodから
			size_t cnt=0;
			if( _opipe_map[i].find("o") != _opipe_map[i].end()){
				cnt += _opipe_map[i]["o"].size();
			}
			if( _opipe_map[i].find("u") != _ipipe_map[i].end()){
				cnt += _opipe_map[i]["u"].size();
			}
			if(cnt==0){
				_argst[clenpos_a].o_cnt= 0;
				_argst[clenpos_a].o_p= NULL;
			}
			else{
				_argst[clenpos_a].o_cnt= cnt;
				_argst[clenpos_a].o_p= new int[cnt];
				size_t pos = 0;
				if( _opipe_map[i].find("o") != _opipe_map[i].end()){
					for(size_t j=0;j<_opipe_map[i]["o"].size();j++){
					_argst[clenpos_a].o_p[pos] = _opipe_map[i]["o"][j];
						pos++;
					}
				}
				if( _opipe_map[i].find("u") != _opipe_map[i].end()){
					for(size_t j=0;j<_opipe_map[i]["u"].size();j++){
						_argst[clenpos_a].o_p[pos] = _opipe_map[i]["u"][j];
						pos++;
					}
				}
			}
		}

		//debug
			
		//debugARGST_OUTPUT(i);

		if(typ==0){
			_th_rtn[clenpos_a] = pthread_create( &_th_st_pp[clenpos_a], NULL, kgshell::run_func ,(void*)&_argst[clenpos_a]);
		}
		else if(typ==1){
			_th_rtn[clenpos_a] = pthread_create( &_th_st_pp[clenpos_a], NULL, kgshell::run_writelist ,(void*)&_argst[clenpos_a]);
		}
		else if(typ==2){
			_th_rtn[clenpos_a] = pthread_create( &_th_st_pp[clenpos_a], NULL, kgshell::run_readlist ,(void*)&_argst[clenpos_a]);
		}
		else if(typ==3){
			_th_rtn[clenpos_a] = pthread_create( &_th_st_pp[clenpos_a], NULL, kgshell::run_pyfunc ,(void*)&_argst[clenpos_a]);
		}
	}
	// iterバージョンの終了確認？別スレッドたちあげる?
	return _csvpiped[0];

}


void kgshell::runInit(
	vector<cmdCapselST> &cmds,	
	vector<linkST> & plist
){

	if(!Py_IsInitialized()){
		Py_Initialize();
	}
	if (!PyEval_ThreadsInitialized())	{ 
		PyEval_InitThreads();
	}
	
	_spblk.blockSplit(KGMOD_RUN_LIMIT,cmds.size(),plist);

	// パラメータ変更		
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

		for(int iblk=0;iblk<_spblk.getBlksize();iblk++){
			if ( _spblk.getModBlkSize(iblk) == 0 ){ continue; }
			//cerr << "blk " << iblk << " start (size:" <<  _spblk.getModBlkSize(iblk) <<  ")==============================" << endl;
			runMain(cmds,plist,iblk);
			//cerr << "blk " << iblk << " end ================================" << endl;
		}
		return 0;

	}
	catch(kgError& err){
	
		cerr << "script RUN KGERROR " << err.message(0) << endl;
		runClean();

	}catch (const exception& e) {

		cerr << "script RUN EX ERR " << e.what() << endl;
		runClean();

	}catch(char * er){
		cerr << "script RUN ERROR CHAR " << er << endl;
		runClean();

	}catch(...){	
		cerr << "script RUN ERROR UNKNOWN TYPE" << endl;
		runClean();
	}

	return 1;
}


kgCSVfld* kgshell::runiter(
	vector<cmdCapselST> &cmds,	
	vector<linkST> & plist
){
	try{

		runInit(cmds,plist);

		for(int iblk=0;iblk<_spblk.getBlksize()-1;iblk++){
			runMain(cmds,plist,iblk);
		}

		int itrfd = runiter_SUB(cmds,plist,_spblk.getBlksize()-1);

		// データ出力
		_iterrtn = new kgCSVfld;
		_iterrtn->popen(itrfd, &_env,_nfni);
		_iterrtn->read_header();	

		return _iterrtn;

	}catch(kgError& err){
	
		cerr << "script RUN KGERROR " << err.message(0) << endl;
		runClean();

	}catch (const exception& e) {

		cerr << "script RUN EX ERR " << e.what() << endl;
		runClean();

	}catch(char * er){
		cerr << "script RUN ERROR CHAR " << er << endl;
		runClean();

	}catch(...){	
		cerr << "script RUN ERROR UNKNOWN TYPE" << endl;
		runClean();
	}
	return NULL;
}


kgCSVkey* kgshell::runkeyiter(
	vector<cmdCapselST> &cmds,	
	vector<linkST> & plist,
	vector<string> & klist
){

	try{

		runInit(cmds,plist);

		for(int iblk=0;iblk<_spblk.getBlksize()-1;iblk++){
			runMain(cmds,plist,iblk);
		}

		int itrfd = runiter_SUB(cmds,plist,_spblk.getBlksize()-1);

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
	
		cerr << "script RUN KGERROR " << err.message(0) << endl;
		runClean();

	}catch (const exception& e) {

		cerr << "script RUN EX ERR " << e.what() << endl;
		runClean();

	}catch(char * er){
		cerr << "script RUN ERROR CHAR " << er << endl;
		runClean();

	}catch(...){	
		cerr << "script RUN ERROR UNKNOWN TYPE" << endl;
		runClean();
	}	
	return NULL;

}


int kgshell::getparams( kgstr_t cmdname, PyObject* list){

	kgMod *mod =NULL;

	try{
		if ( _kgmod_map.find(cmdname) == _kgmod_map.end()){
			cerr << "Not unspport mod " << cmdname << endl;
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
		cerr << "UnKnown ERROR IN GET PARAMETER " << endl;
		return 1;	
	}
	return 1;

}