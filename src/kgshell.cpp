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
#include <kgshell.h>
#include <pthread.h>

using namespace kgmod;
using namespace kglib;


kgshell::kgshell(void){
		_kgmod_map["mcut"] = boost::lambda::bind(boost::lambda::new_ptr<kgCut>());
		_kgmod_map["mcat"] = boost::lambda::bind(boost::lambda::new_ptr<kgCat>());
		_kgmod_map["msum"] = boost::lambda::bind(boost::lambda::new_ptr<kgSum>());
		_kgmod_map["mcal"] = boost::lambda::bind(boost::lambda::new_ptr<kgCal>());
		_kgmod_map["mjoin"] = boost::lambda::bind(boost::lambda::new_ptr<kgJoin>());
		_kgmod_map["mload"] = boost::lambda::bind(boost::lambda::new_ptr<kgLoad>());
		_kgmod_map["msave"] = boost::lambda::bind(boost::lambda::new_ptr<kgLoad>());
		_kgmod_map["mbuffer"] = boost::lambda::bind(boost::lambda::new_ptr<kgFifo>());
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

		_kgmod_run["mcut"] = 1;
		_kgmod_run["mcat"] = 1;
		_kgmod_run["msum"] = 1;
		_kgmod_run["mcal"] = 1;
		_kgmod_run["mjoin"] = 2;
		_kgmod_run["mload"] = 3;
		_kgmod_run["msave"] = 1;
		_kgmod_run["mbuffer"] = 1;
		_kgmod_run["m2cross"] = 1;
		_kgmod_run["maccum"] = 1;
		_kgmod_run["mavg"] = 1;
		_kgmod_run["mbest"] = 1;
		_kgmod_run["mbucket"] = 1;
		_kgmod_run["mchgnum"] = 1;
		_kgmod_run["mchgstr"] = 1;
		_kgmod_run["mcombi"] = 1;
		_kgmod_run["mcommon"] = 2;
		_kgmod_run["mcount"] = 1;
		_kgmod_run["mcross"] = 1;
		_kgmod_run["mdelnull"] = 1;
		_kgmod_run["mdformat"] = 1;
		_kgmod_run["mduprec"] = 1;
		_kgmod_run["mfldname"] = 1;
		_kgmod_run["mfsort"] = 1;
		_kgmod_run["mhashavg"] = 1;
		_kgmod_run["mhashsum"] = 1;
		_kgmod_run["mkeybreak"] = 1;
		_kgmod_run["mmbucket"] = 1;
		_kgmod_run["mmvavg"] = 1;
		_kgmod_run["mmvsim"] = 1;
		_kgmod_run["mmvstats"] = 1;
		_kgmod_run["mnewnumber"] = 0;
		_kgmod_run["mnewrand"] = 0;
		_kgmod_run["mnewstr"] = 0;
		_kgmod_run["mnjoin"] = 2;
		_kgmod_run["mnormalize"] = 1;
		_kgmod_run["mnrcommon"] = 2;
		_kgmod_run["mnrjoin"] = 2;
		_kgmod_run["mnullto"] = 1;
		_kgmod_run["mnumber"] = 1;
		_kgmod_run["mpadding"] = 1;
		_kgmod_run["mpaste"] = 2;
		_kgmod_run["mproduct"] = 2;
		_kgmod_run["mrand"] = 1;
		_kgmod_run["mrjoin"] = 2;
		_kgmod_run["msed"] = 1;
		_kgmod_run["msel"] = 1;
		_kgmod_run["mselnum"] = 1;
		_kgmod_run["mselrand"] = 1;
		_kgmod_run["mselstr"] = 1;
		_kgmod_run["msetstr"] = 1;
		_kgmod_run["mshare"] = 1;
		_kgmod_run["msim"] = 1;
		_kgmod_run["mslide"] = 1;
		_kgmod_run["msplit"] = 1;
		_kgmod_run["mstats"] = 1;
		_kgmod_run["msummary"] = 1;
		_kgmod_run["mtonull"] = 1;
		_kgmod_run["mtra"] = 1;
		_kgmod_run["mtraflg"] = 1;
		_kgmod_run["muniq"] = 1;
		_kgmod_run["mvcat"] = 1;
		_kgmod_run["mvcommon"] = 2;
		_kgmod_run["mvcount"] = 1;
		_kgmod_run["mvdelim"] = 1;
		_kgmod_run["mvdelnull"] = 1;
		_kgmod_run["mvjoin"] = 2;
		_kgmod_run["mvnullto"] = 1;
		_kgmod_run["mvreplace"] = 2;
		_kgmod_run["mvsort"] = 1;
		_kgmod_run["mvuniq"] = 1;
		_kgmod_run["mwindow"] = 1;
		_kgmod_run["marff2csv"] =1;
		_kgmod_run["mxml2csv"] =1;
		_kgmod_run["msortf"] =1;
		_kgmod_run["mtab2csv"] = 1;

		_nfni = false;
 		_iterrtn= NULL;
		_th_st_pp = NULL;
		_clen = 0;
		_modlist=NULL;
	

}




void *kgshell::run_noargs_pths(void *arg)try{
	argST *a =(argST*)arg; 
	a->mobj->run(a->i_p,a->o_p);
	return NULL;
}catch(...){
	argST *a =(argST*)arg; 
	if(a->i_p>0){ ::close(a->i_p);}
	if(a->o_p>0){ ::close(a->o_p);}
	return NULL;
}
void *kgshell::run_noargs_pthsp(void *arg)try{
	argST *a =(argST*)arg; 
	a->mobj->run(a->list,a->o_p);
	return NULL;
}catch(...){
	argST *a =(argST*)arg; 
	if(a->o_p>0){ ::close(a->o_p);}
	return NULL;
}
void *kgshell::run_noargs_pthsm(void *arg)try{
	argST *a =(argST*)arg; 
	a->mobj->run(a->i_p,a->o_p,a->m_p);
	return NULL;
}catch(...){
	argST *a =(argST*)arg; 
	if(a->i_p>0){ ::close(a->i_p);}
	if(a->o_p>0){ ::close(a->o_p);}
	return NULL;
}

void *kgshell::run_noargs_pths1(void *arg)try{
	argST *a =(argST*)arg; 
	a->mobj->run(a->o_p);
	return NULL;
}catch(...){
	argST *a =(argST*)arg; 
	if(a->o_p>0){ ::close(a->o_p);}
	return NULL;
}


void kgshell::makePipeList(vector< vector<int> >& plist,bool tp)
{
	for(size_t i=0;i<plist.size();i++){
		int piped[2];
		if( pipe(piped) < 0){ throw kgError("pipe open error on kgshell");}
		int flags0 = fcntl(piped[0], F_GETFD);
		int flags1 = fcntl(piped[1], F_GETFD);
		fcntl(piped[0], F_SETFD, flags0 | FD_CLOEXEC);
		fcntl(piped[1], F_SETFD, flags1 | FD_CLOEXEC);
		if(plist[i][2]==0){
			_ipipe_map[plist[i][0]]=piped[0];
			_opipe_map[plist[i][1]]=piped[1];
		}
		else{
			_mpipe_map[plist[i][0]]=piped[0];
			_opipe_map[plist[i][1]]=piped[1];		
		}		
	}
	if(tp){
		if( pipe(_lastpiped) < 0){ throw kgError("pipe open error on kgshell");}
		int flags0 = fcntl(_lastpiped[0], F_GETFD);
		int flags1 = fcntl(_lastpiped[1], F_GETFD);
		fcntl(_lastpiped[0], F_SETFD, flags0 | FD_CLOEXEC);
		fcntl(_lastpiped[1], F_SETFD, flags1 | FD_CLOEXEC);
	}
	else{
		_lastpiped[0] = -1;
		_lastpiped[1] = -1;
	}
}

int kgshell::run(
	vector< cmdCapselST > &cmds,	
	vector< vector<int> >& plist,
	bool tp,
	PyObject* list)try
{

	makePipeList(plist,tp);
	
	size_t clen = cmds.size();

	_modlist = new kgMod*[clen];
	
	for(size_t i=0;i<clen;i++){
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

	int dmy = -1;	
	pthread_t _th_st_p[clen];
	int _th_rtn[clen];
	argST argst[clen];
	for(int i=clen-1;i>=0;i--){
	// データ出力
		argst[i].mobj= _modlist[i];
		if( _ipipe_map.find(i) == _ipipe_map.end() ){ argst[i].i_p= dmy; }
		else{ argst[i].i_p= _ipipe_map[i]; }
		if(i==0){ argst[i].o_p= _lastpiped[1]; } 
		else { 
			if( _opipe_map.find(i) == _opipe_map.end() ){ argst[i].o_p= dmy; }
			else{ argst[i].o_p= _opipe_map[i]; }
		}
		//debug
		//cerr << argst[i].mobj->name() << " " <<  argst[i].i_p << " " << argst[i].o_p << endl;

		int typ =  _kgmod_run.find(cmds[i].cmdname)->second ;
		if(typ==1){
			_th_rtn[i] = pthread_create( &_th_st_p[i], NULL, kgshell::run_noargs_pths ,(void*)&argst[i]);
		}else if(typ==2){
			if( _mpipe_map.find(i) == _mpipe_map.end() ){ argst[i].m_p= dmy; }
			else{ argst[i].m_p= _mpipe_map[i]; }
			_th_rtn[i] = pthread_create( &_th_st_p[i], NULL, kgshell::run_noargs_pthsm ,(void*)&argst[i]);		
		}
		else if(typ==3){
			argst[i].list=cmds[i].iobj;
			_th_rtn[i] = pthread_create( &_th_st_p[i], NULL, kgshell::run_noargs_pthsp ,(void*)&argst[i]);
		}
		else if(typ==0){
			_th_rtn[i] = pthread_create( &_th_st_p[i], NULL, kgshell::run_noargs_pths1 ,(void*)&argst[i]);
		}

		if(_th_rtn[i]){ throw kgError("cant't create thread onxx kgModIncludeSort");}
	}
	if(tp){
		// データ出力
		kgCSVfld rls;
		bool nfn_i = false;
		rls.popen(_lastpiped[0], &_env,nfn_i);
		rls.read_header();	
		while( EOF != rls.read() ){
			PyObject* tlist = PyList_New(0);
			for(size_t j=0 ;j<rls.fldSize();j++){
				PyList_Append(tlist,Py_BuildValue("s",rls.getVal(j)));
			}
			PyList_Append(list,tlist);
		}
	}
	for(int i=clen;i>0;i--){
		pthread_join(_th_st_p[i-1],NULL);
	}
	return 0;
}catch(...){
	return 1;
}

kgCSVfld* kgshell::runiter(
	vector< cmdCapselST > &cmds,	
	vector< vector<int> >& plist,
	bool tp,
	PyObject* list)try{

	size_t clen = cmds.size();

	makePipeList(plist,tp);

	_modlist = new kgMod*[clen];
	
	for(size_t i=0;i<clen;i++){
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

	int dmy = -1;	
	pthread_t _th_st_p[clen];
	int _th_rtn[clen];
	argST argst[clen];
	for(int i=clen-1;i>=0;i--){
	// データ出力
		argst[i].mobj= _modlist[i];
		if( _ipipe_map.find(i) == _ipipe_map.end() ){ argst[i].i_p= dmy; }
		else{ argst[i].i_p= _ipipe_map[i]; }
		if(i==0){ argst[i].o_p= _lastpiped[1]; } 
		else { 
			if( _opipe_map.find(i) == _opipe_map.end() ){ argst[i].o_p= dmy; }
			else{ argst[i].o_p= _opipe_map[i]; }
		}
		//debug
		//cerr << argst[i].mobj->name() << " " <<  argst[i].i_p << " " << argst[i].o_p << endl;

		int typ =  _kgmod_run.find(cmds[i].cmdname)->second ;
		if(typ==1){
			_th_rtn[i] = pthread_create( &_th_st_p[i], NULL, kgshell::run_noargs_pths ,(void*)&argst[i]);
		}else if(typ==2){
			if( _mpipe_map.find(i) == _mpipe_map.end() ){ argst[i].m_p= dmy; }
			else{ argst[i].m_p= _mpipe_map[i]; }
			_th_rtn[i] = pthread_create( &_th_st_p[i], NULL, kgshell::run_noargs_pthsm ,(void*)&argst[i]);		
		}
		else if(typ==3){
			argst[i].list=cmds[i].iobj;
			_th_rtn[i] = pthread_create( &_th_st_p[i], NULL, kgshell::run_noargs_pthsp ,(void*)&argst[i]);
		}
		else if(typ==0){
			_th_rtn[i] = pthread_create( &_th_st_p[i], NULL, kgshell::run_noargs_pths1 ,(void*)&argst[i]);
		}

		if(_th_rtn[i]){ throw kgError("cant't create thread onxx kgModIncludeSort");}
	}
	
	

	// データ出力
	_iterrtn = new kgCSVfld;

	bool nfn_i = false;
	_iterrtn->popen(_lastpiped[0], &_env,_nfni);
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
			cerr << "not kgmod " << endl;
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
