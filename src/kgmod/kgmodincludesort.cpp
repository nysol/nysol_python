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
// ============================================================================
// kgmodincludesort
// ============================================================================
#include <kgmod.h>
#include <kgMessage.h>
#include <kgsortf.h>
#include <kgmodincludesort.h>
#include <fcntl.h>

using namespace kglib;
using namespace kgmod;

void *kgModIncludeSort::run_noargs_pth(void *arg){
	((kgSortf*)arg)->run_noargs();
	return NULL;
}

void kgModIncludeSort::th_cancel(void){
	vector<int> chk(_sortModSize);

	for(size_t i=0 ;i<_sortModSize;i++){ 
		chk[i] = pthread_cancel(_th_st_p[i]);
		if (chk[i]!=0&&chk[i]!=3){ 
			kgMsg msg(kgMsg::MSG, _env);
			msg.output("waring destruct fail thread cancel :( "+ toString(chk[i]) + ")");
		}
	}
	for(size_t i=0 ;i<_sortModSize;i++){ 
		if(chk[i]==0||chk[i]==3){
			int rtn = pthread_join(_th_st_p[i],NULL);
			if(rtn!=0) { 
				kgMsg msg(kgMsg::MSG, _env);
				msg.output("waring destruct fail thread cancel :( "+ toString(chk[i]) + ")");
			}
		}
	}
}


void kgModIncludeSort::setSortMod(size_t num){
	_sortModSize=num;
	try {
		_aps.set    ( new kgSortf[_sortModSize] );
		_aps_pth.set( new pthread_t[_sortModSize] );
		_ap_env.set( new kgEnv[_sortModSize] );
		_ap_tp.set( new kgTempfile[_sortModSize] ) ;
		
	} catch(bad_alloc) {
		throw kgError("memory allocation error on CSVin");
	}
	_inner_sort = _aps.get();
	_th_st_p = _aps_pth.get();
	_env_sort =	_ap_env.get();
	_sortingFile =_ap_tp.get();
	
	int vlvl = 2;
	if(_env->getVerboseLevel() != KG_VerboseLevel){
		vlvl = _env->getVerboseLevel();
	}
	for(size_t i=0;i<_sortModSize;i++){
		_env_sort[i].verblvl(vlvl);
		_sortingFile[i].init(&_env_sort[i]);
	}

}

void kgModIncludeSort::sortingRunMain(kgCSVfld* csv ,kgstr_t fldname ,size_t num,bool numflg){

	if(num>=_sortModSize){
		throw kgError("sort module allocation error on kgModIncludeSort");
	}
	int piped[2];
	if( pipe(piped) < 0){ 
		throw kgError("sort pipe open error on kgModIncludeSort");		
	} 
	//pipe2(piped,O_CLOEXEC)
	//pipe2なら以下省略できる 
	int flags0 = fcntl(piped[0], F_GETFD);
	int flags1 = fcntl(piped[1], F_GETFD);
	fcntl(piped[0], F_SETFD, flags0 | FD_CLOEXEC);
	fcntl(piped[1], F_SETFD, flags1 | FD_CLOEXEC);

	kgArgs newArgs;// 引数
	newArgs.add("i=",csv->fileName());
	newArgs.add("f=",fldname);
	if(numflg){newArgs.add("-x","");} 
	_inner_sort[num].init(newArgs, &_env_sort[num]);
	_inner_sort[num].replace_ifile(csv);
	_inner_sort[num].prerun_noargs(piped[1]);
	csv->clear();
	csv->popen(piped[0], _env,_nfn_i);
	kgMsg(kgMsg::DEB, _env).output("O sorting 0 " );
	int rtn = pthread_create( &_th_st_p[num], NULL, 
			kgModIncludeSort::run_noargs_pth ,(void *)&_inner_sort[num]);
	if(rtn){ throw kgError("cant't create thread onxx kgModIncludeSort");}

	kgMsg(kgMsg::DEB, _env).output("O sorting 1 " );
}

void kgModIncludeSort::sortingRun(vector<kgCSVfld*> csv,vector< vector<kgstr_t> > fldname,bool sortF)
{
	vector<bool> sortrun(csv.size(),true);
	size_t sortMcnt = csv.size();
	if(!sortF){
		sortMcnt=0;
		for(size_t i=0;i<csv.size();i++){
			sortrun[i] = csv.at(i)->sortFldchk(fldname.at(i),_fldByNum);
			if(sortrun[i]){ sortMcnt++;}
		}
	}
	if(sortMcnt==0){ return;}
	setSortMod(sortMcnt);
	int pos=0;
	for(size_t i=0;i<sortrun.size();i++){
		if(sortrun.at(i)==true){
			sortingRunMain(csv.at(i),toString(fldname.at(i)),pos,_fldByNum);
			csv.at(i)->read_header();	
			pos++;
		}
	}
}

void kgModIncludeSort::sortingRun(kgCSVfld* csv,vector<kgstr_t>  fldname,bool sortF)
{
	if(!sortF){ sortF = csv->sortFldchk(fldname,_fldByNum); }
	if(!sortF){ return;}

	setSortMod(1);
	sortingRunMain(csv,toString(fldname),0,_fldByNum);
	csv->read_header();	
}

void kgModIncludeSort::sortingRun(kgCSVfld* csv,vector<int>  fldnum,bool sortF)
{
	if(!sortF){ sortF = csv->sortFldchk(fldnum); }
	if(!sortF){ return;}

	setSortMod(1);
	sortingRunMain(csv,toString(fldnum),0,true);
	csv->read_header();	
}
	

void kgModIncludeSort::init(kgArgs args, kgEnv* env)
{
	kgMod::init(args,env);
	/*
	if(_env->getVerboseLevel() == KG_VerboseLevel){_env_sort.verblvl(2);}
	else{_env_sort.verblvl(_env->getVerboseLevel());}		
	_sortingFile.init(&_env_sort);
	*/
}

void kgModIncludeSort::init(size_t argc, const char* argv[], kgEnv* env)
{
	kgMod::init(argc,argv,env);		
	/*
	if(_env->getVerboseLevel() == KG_VerboseLevel){_env_sort.verblvl(2);}
	else{_env_sort.verblvl(_env->getVerboseLevel());}		
	_sortingFile.init(&_env_sort);
	*/
}
