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


		_kgmod_map["writelist"] = boost::lambda::bind(boost::lambda::new_ptr<kgLoad>());
		_kgmod_map["readlist"] = boost::lambda::bind(boost::lambda::new_ptr<kgLoad>());
		_kgmod_map["readcsv"] = boost::lambda::bind(boost::lambda::new_ptr<kgCat>());
		_kgmod_map["writecsv"] = boost::lambda::bind(boost::lambda::new_ptr<kgLoad>());


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
		_kgmod_run["mrjoin"] = 2;
		_kgmod_run["msed"] = 0;
		_kgmod_run["msel"] = 0;
		_kgmod_run["mselnum"] = 0;
		_kgmod_run["mselrand"] = 0;
		_kgmod_run["mselstr"] = 0;
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



		_kgmod_run["mload"] = 0;
		_kgmod_run["msave"] = 0;
		_kgmod_run["writelist"] = 1;
		_kgmod_run["readlist"] = 2;


		_nfni = false;
 		_iterrtn= NULL;
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

}



void *kgshell::run_func(void *arg)try{
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

void *kgshell::run_writelist(void *arg)try{
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

void *kgshell::run_readlist(void *arg)try{
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


void kgshell::makePipeList(vector<linkST> & plist)
{
	rlimit rlim;
	int chfFlg;
	chfFlg = getrlimit(RLIMIT_NOFILE, &rlim);
	size_t pfilecnt = plist.size() * 32 ;
	if(rlim.rlim_cur < pfilecnt ){
		rlim.rlim_cur = pfilecnt;
		chfFlg = setrlimit(RLIMIT_NOFILE, &rlim);
		if (chfFlg <0 ) { throw kgError("change file limit on kgshell"); } 
	}

	for(size_t i=0;i<plist.size();i++){

		int piped[2];
		if( pipe(piped) < 0){ throw kgError("pipe open error on kgshell");}
		int flags0 = fcntl(piped[0], F_GETFD);
		int flags1 = fcntl(piped[1], F_GETFD);
		fcntl(piped[0], F_SETFD, flags0 | FD_CLOEXEC);
		fcntl(piped[1], F_SETFD, flags1 | FD_CLOEXEC);

		/*linkST{
		kgstr_t frTP;
		int frID;
		kgstr_t toTP;
		int toID;
		};*/
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

int kgshell::run(
	vector<cmdCapselST> &cmds,	
	vector<linkST> & plist
)try
{

	makePipeList(plist);

	//DEBUG
	/*
	for(iomap_t::iterator it=_ipipe_map.begin() ;it!=_ipipe_map.end();it++){
			cerr << it->first << "--|" << endl;
			for(map<string,vector<int> >::iterator it2=it->second.begin() ;it2!=it->second.end();it2++){
				cerr << " " << it2->first << ": ";
				for(vector<int>::iterator it3=it2->second.begin() ;it3!=it2->second.end();it3++){
					cerr << *it3 << " ";
				}
				cerr << endl;
			}
	}
	for(iomap_t::iterator it=_opipe_map.begin() ;it!=_opipe_map.end();it++){
			cerr << it->first << "--|" << endl;
			for(map<string,vector<int> >::iterator it2=it->second.begin() ;it2!=it->second.end();it2++){
				cerr << " " << it2->first << ": ";
				for(vector<int>::iterator it3=it2->second.begin() ;it3!=it2->second.end();it3++){
					cerr << *it3 << " ";
				}
				cerr << endl;
			}
	}
	*/
	_clen = cmds.size();

	_modlist = new kgMod*[_clen];
	
	for(size_t i=0;i<_clen;i++){
		if ( _kgmod_map.find(cmds[i].cmdname) == _kgmod_map.end()){
			cerr << "not 1 kgmod " << cmds[i].cmdname << endl;
			return 1;
		}
		_modlist[i] = _kgmod_map.find(cmds[i].cmdname)->second() ;
		kgArgs newArgs;
		for(size_t j=0;j<cmds[i].paralist.size();j++){
			newArgs.add(cmds[i].paralist[j]);
		}
		_modlist[i]->init(newArgs, &_env);
	}

	_th_st_pp = new pthread_t[_clen];
	argST *argst = new argST[_clen];
	int _th_rtn[_clen];

	for(int i=_clen-1;i>=0;i--){

		argst[i].mobj= _modlist[i];
		argst[i].tag= cmds[i].tag;
		argst[i].finflg = false;
		argst[i].outputEND = false;
		argst[i].status = 0;
		argst[i].stMutex = &_stsMutex;
		argst[i].stCond = &_stsCond;

		int typ = _kgmod_run.find(cmds[i].cmdname)->second ;

		//	DEBIG
		//	cerr << "-------------------" << endl;
		//	cerr << i << ":"<< argst[i].mobj->name() << endl;

		if( _ipipe_map.find(i) == _ipipe_map.end() ){ 
			if(typ==2){
				argst[i].i_cnt= 1;
				argst[i].i_p= NULL;
				argst[i].list = cmds[i].iobj;
			}
			else{
				argst[i].i_cnt= 0;
				argst[i].i_p= NULL;
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
			}
			if(cnt==0){
				argst[i].i_cnt= 0;
				argst[i].i_p= NULL;
			}
			else{
				argst[i].i_cnt= cnt;
				argst[i].i_p= new int[cnt];
				size_t pos = 0;
				if( _ipipe_map[i].find("i") != _ipipe_map[i].end()){
					for(size_t j=0;j<_ipipe_map[i]["i"].size();j++){
						argst[i].i_p[pos] = _ipipe_map[i]["i"][j];
						pos++;
					}
				}
				if( _ipipe_map[i].find("m") != _ipipe_map[i].end()){
					for(size_t j=0;j<_ipipe_map[i]["m"].size();j++){
						argst[i].i_p[pos] = _ipipe_map[i]["m"][j];
						pos++;
					}
				}
			}
		}

		if( _opipe_map.find(i) == _opipe_map.end() ){ 
			if(typ==1){
				argst[i].o_cnt= 1;
				argst[i].o_p = NULL;
				argst[i].mutex = &_mutex;
				argst[i].list = cmds[i].oobj;
			}
			else{
				argst[i].o_cnt= 0;
				argst[i].o_p= NULL;
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
				argst[i].o_cnt= 0;
				argst[i].o_p= NULL;
			}
			else{
				argst[i].o_cnt= cnt;
				argst[i].o_p= new int[cnt];
				size_t pos = 0;
				if( _opipe_map[i].find("o") != _opipe_map[i].end()){
					for(size_t j=0;j<_opipe_map[i]["o"].size();j++){
						argst[i].o_p[pos] = _opipe_map[i]["o"][j];
						pos++;
					}
				}
				if( _opipe_map[i].find("u") != _opipe_map[i].end()){
					for(size_t j=0;j<_opipe_map[i]["u"].size();j++){
						argst[i].o_p[pos] = _opipe_map[i]["u"][j];
						pos++;
					}
				}
			}
		}
		//debug
		/*
		cerr << i << ":"<< argst[i].mobj->name() << " " << argst[i].i_cnt << " " << argst[i].o_cnt ;
		if ( argst[i].i_cnt > 0&& argst[i].i_p!=NULL){
			cerr << " i:" ;
			for(size_t j=0; j< argst[i].i_cnt;j++){
				cerr <<  *(argst[i].i_p+j) << " " ;
			}
		}
		if ( argst[i].o_cnt > 0 && argst[i].o_p!=NULL){
			cerr << " o:" ;
			for(size_t j=0; j< argst[i].o_cnt;j++){
				cerr <<  *(argst[i].o_p+j) << " " ;
			}
		}
		cerr << endl;
		*/
		if(typ==0){
			_th_rtn[i] = pthread_create( &_th_st_pp[i], NULL, kgshell::run_func ,(void*)&argst[i]);
		}
		else if(typ==1){
			_th_rtn[i] = pthread_create( &_th_st_pp[i], NULL, kgshell::run_writelist ,(void*)&argst[i]);
		}
		else if(typ==2){
			_th_rtn[i] = pthread_create( &_th_st_pp[i], NULL, kgshell::run_readlist ,(void*)&argst[i]);
		}
	}
	// status check
	pthread_mutex_lock(&_stsMutex);
	while(1){
		int pos = 0;
		bool endFLG = true;
		while(pos<_clen){
			if(argst[pos].finflg==false){ endFLG=false;}
			else if(argst[pos].outputEND==false){
				if(!argst[pos].msg.empty()){
					cerr << argst[pos].msg << " " << "(" << argst[pos].endtime << ")" << endl; 
				}
				else if(!argst[pos].tag.empty()){
					cerr  << argst[pos].tag << "(" << argst[pos].endtime << ")" << endl; 
				}
				argst[pos].outputEND = true;
			}
			if(argst[pos].status!=0){
				//エラー発生時はthread cancel
				for(int j=0;j<_clen;j++){
					pthread_cancel(_th_st_pp[j]);	
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

	for(int i=_clen;i>0;i--){
		pthread_join(_th_st_pp[i-1],NULL);
	}
	if(_modlist){
		for(size_t i=0 ;i<_clen;i++){
			if(argst[i].outputEND == false){
				if(!argst[i].msg.empty()){
					cerr << argst[i].msg << " " <<  argst[i].tag << "(" << argst[i].endtime << ")" << endl;
				}
				else if(!argst[i].tag.empty()){
					cerr << argst[i].tag  << "(" << argst[i].endtime << ")" <<  endl;
				}
			}
			argst[i].outputEND = true;
			delete _modlist[i];
		}
		delete[] _modlist;
	}
	delete[] _th_st_pp;
	_th_st_pp = NULL;
	_modlist = NULL;

	return 0;
}catch(...){
	return 1;
}

kgCSVfld* kgshell::runiter(
	vector<cmdCapselST> &cmds,	
	vector<linkST> & plist
)try{

	makePipeList(plist);
	if( pipe(_csvpiped) < 0){ throw kgError("pipe open error on kgshell");}
	// pipe2(piped,O_CLOEXEC) pipe2なら省略化
	int flags0 = fcntl(_csvpiped[0], F_GETFD);
	int flags1 = fcntl(_csvpiped[1], F_GETFD);
	fcntl(_csvpiped[0], F_SETFD, flags0 | FD_CLOEXEC);
	fcntl(_csvpiped[1], F_SETFD, flags1 | FD_CLOEXEC);

	_clen = cmds.size();

	_modlist = new kgMod*[_clen];
	
	for(size_t i=0;i<_clen;i++){
		if ( _kgmod_map.find(cmds[i].cmdname) == _kgmod_map.end()){
			cerr << "not 1 kgmod " << cmds[i].cmdname << endl;
			return NULL;
		}
		_modlist[i] = _kgmod_map.find(cmds[i].cmdname)->second() ;
		kgArgs newArgs;
		for(size_t j=0;j<cmds[i].paralist.size();j++){
			newArgs.add(cmds[i].paralist[j]);
		}
		_modlist[i]->init(newArgs, &_env);
	}
	_th_st_pp = new pthread_t[_clen];
	int _th_rtn[_clen];
	argST *argst = new argST[_clen];

	for(int i=_clen-1;i>=0;i--){

		argst[i].mobj= _modlist[i];
		argst[i].tag= cmds[i].tag;
		argst[i].finflg = false;
		argst[i].outputEND = false;
		argst[i].status = 0;
		argst[i].stMutex = &_stsMutex;
		argst[i].stCond = &_stsCond;

		int typ = _kgmod_run.find(cmds[i].cmdname)->second ;

		//	DEBIG
		//	cerr << "-------------------" << endl;
		//	cerr << i << ":"<< argst[i].mobj->name() << endl;

		if( _ipipe_map.find(i) == _ipipe_map.end() ){ 
			if(typ==2){
				argst[i].i_cnt= 1;
				argst[i].list = cmds[i].iobj;
			}
			else{
				argst[i].i_cnt= 0;
				argst[i].i_p= NULL;
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
			}
			if(cnt==0){
				argst[i].i_cnt= 0;
				argst[i].i_p= NULL;
			}
			else{
				argst[i].i_cnt= cnt;
				argst[i].i_p= new int[cnt];
				size_t pos = 0;
				if( _ipipe_map[i].find("i") != _ipipe_map[i].end()){
					for(size_t j=0;j<_ipipe_map[i]["i"].size();j++){
						argst[i].i_p[pos] = _ipipe_map[i]["i"][j];
						pos++;
					}
				}
				if( _ipipe_map[i].find("m") != _ipipe_map[i].end()){
					for(size_t j=0;j<_ipipe_map[i]["m"].size();j++){
						argst[i].i_p[pos] = _ipipe_map[i]["m"][j];
						pos++;
					}
				}
			}
		}
		if(i==0){ // kgcsv用
			argst[i].o_cnt= 1;
			argst[i].o_p= new int[1];
			argst[i].o_p[0]= _csvpiped[1]; 
		} 
		else if( _opipe_map.find(i) == _opipe_map.end() ){ 
			if(typ==1){
				argst[i].o_cnt= 1;
				argst[i].list = cmds[i].oobj;
			}
			else{
				argst[i].o_cnt= 0;
				argst[i].o_p= NULL;
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
				argst[i].o_cnt= 0;
				argst[i].o_p= NULL;
			}
			else{
				argst[i].o_cnt= cnt;
				argst[i].o_p= new int[cnt];
				size_t pos = 0;
				if( _opipe_map[i].find("o") != _opipe_map[i].end()){
					for(size_t j=0;j<_opipe_map[i]["o"].size();j++){
						argst[i].o_p[pos] = _opipe_map[i]["o"][j];
						pos++;
					}
				}
				if( _opipe_map[i].find("u") != _opipe_map[i].end()){
					for(size_t j=0;j<_opipe_map[i]["u"].size();j++){
						argst[i].o_p[pos] = _opipe_map[i]["u"][j];
						pos++;
					}
				}
			}
		}
		//debug
		//cerr << i << ":"<< argst[i].mobj->name() << " " << argst[i].i_cnt << " " << argst[i].o_cnt ;
		//if ( argst[i].i_cnt > 0){
		//	cerr << " i:" ;
		//	for(size_t j=0; j< argst[i].i_cnt;j++){
		//		cerr <<  *(argst[i].i_p+j) << " " ;
		//	}
		//}
		//if ( argst[i].o_cnt > 0){
		//	cerr << " o:" ;
		//	for(size_t j=0; j< argst[i].o_cnt;j++){
		//		cerr <<  *(argst[i].o_p+j) << " " ;
		//	}
		//}
		//cerr << endl;
		if(typ==0){
			_th_rtn[i] = pthread_create( &_th_st_pp[i], NULL, kgshell::run_func ,(void*)&argst[i]);
		}
		else if(typ==1){//これは使えないようにする
			_th_rtn[i] = pthread_create( &_th_st_pp[i], NULL, kgshell::run_writelist ,(void*)&argst[i]);
		}
		else if(typ==2){
			_th_rtn[i] = pthread_create( &_th_st_pp[i], NULL, kgshell::run_readlist ,(void*)&argst[i]);
		}
	}
	// データ出力
	_iterrtn = new kgCSVfld;

	_iterrtn->popen(_csvpiped[0], &_env,_nfni);
	_iterrtn->read_header();	

	return _iterrtn;

}catch(...){
	return NULL;
}

int kgshell::getparams(
	kgstr_t cmdname,
	PyObject* list)try{

	kgMod *mod =NULL;
	if ( _kgmod_map.find(cmdname) == _kgmod_map.end()){
			cerr << "not kgmod " << cmdname << endl;
			return 1;	
	}
	kgArgs newArgs;
	mod	= _kgmod_map.find(cmdname)->second();
	mod->init(newArgs, &_env);
	vector < vector <kgstr_t> > paralist = mod->params();

	for (size_t i=0;i<paralist.size();i++){
		PyObject* tlist = PyList_New(0);
		for(size_t j=0 ;j<paralist[i].size();j++){
			PyList_Append(tlist,Py_BuildValue("s",paralist[i][j].c_str()));
		}
		PyList_Append(list,tlist);
	}
	if(mod) delete mod;
	return 0;
}catch(...){
	return 1;
}
