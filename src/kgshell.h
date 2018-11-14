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
#pragma once
#include "Python.h"
#include <fcntl.h>
#include <kgEnv.h>
#include <kgError.h>
#include <kgMessage.h>
#include <kgmod.h>
#include <kgCSV.h>
#include <kgshelltp.h>
#include <kgshellfunc.h>
#include <kgsplitblock.h>
#include <vector>
#include <set>
#include <map>
#include <pthread.h>


#ifndef KGMOD_RUN_LIMIT
	#define KGMOD_RUN_LIMIT 128
#endif

#ifndef KGMOD_THREAD_STK
	#define KGMOD_THREAD_STK 1048576
#endif

#define KGMOD_MEMBASE 47104000

using namespace kglib;
using namespace kgmod;

struct argST{
	kgMod * mobj;
	int i_cnt;
	int o_cnt;
	int *i_p;
	int *o_p;
	PyObject* list;
	PyObject* fobj;
	PyObject* aobj;
	PyObject* kobj;
	bool finflg;
	bool outputEND;
	int status;
	kgstr_t msg;
	kgstr_t tag;
	kgstr_t endtime;
	vector<int> fdlist;
	pthread_mutex_t *mutex;
	pthread_mutex_t *stMutex;
	pthread_mutex_t *iniMutex;
	pthread_cond_t *stCond;
	pthread_cond_t *forkCond;
	pthread_cond_t *iniCond;
	PyThreadState * newth;
	int *runst;
};
struct watchST{
	pthread_mutex_t *stMutex;
	pthread_cond_t *stCond;
	argST *argst;
	kgEnv *env;
	pthread_t* th_st_pp;
	int clen;
	bool pymsg;
};

struct cmdCapselST{
	kgstr_t cmdname;
	vector<kgstr_t> paralist;
	kgstr_t istr;
	PyObject* iobj;
	kgstr_t mstr;
	PyObject* mobj;
	PyObject* oobj;
	PyObject* fobj;
	PyObject* aobj;
	PyObject* kobj;
	kgstr_t tag;
	int weight;
};


class kgshell{

	pthread_mutex_t _mutex;
	pthread_mutex_t _stsMutex;
	pthread_mutex_t _iniMutex;

	pthread_cond_t 	_stsCond;
	pthread_cond_t 	_forkCond;
	pthread_cond_t _iniCond;

	template <class kgmodTP> void setMap(std::string,int runTP);

	typedef map<std::string, boost::function<kgMod* ()> > kgmod_map_t;
	typedef map<std::string, int > kgmod_run_t;
	typedef map<std::string, const char **> kgmod_ioinfo_t;

	kgmod_map_t    _kgmod_map; //keyword - 関数対応表
	kgmod_run_t    _kgmod_run;
	kgmod_ioinfo_t _kgmod_Iinfo;
	kgmod_ioinfo_t _kgmod_Oinfo;


  kgEnv _env;
	bool _nfni;
	bool _mflg;
	bool _pymsg;
 	kgCSVfld* _iterrtn;
 	kgCSVkey* _iterrtnk;
	pthread_t* _th_st_pp;
	int *_runst;
	int *_th_rtn;
	size_t _clen;
	kgMod **_modlist;
	kgEnv **_envlist;

	size_t _memttl;
	int	_runlim;

	PyThreadState *_save;
	watchST _watchST;

	bool _watchFlg;
	pthread_t _th_st_watch;

	// pipe LIST
	//map<int,int> _ipipe_map;
	//map<int,int> _mpipe_map;
	//map<int,int> _opipe_map;

	kgTempfile _tempFile; 

	typedef map<int, map<string,vector<int> > > iomap_t;
	iomap_t _ipipe_map;
	iomap_t _opipe_map;

	typedef map<int, vector<int> > edgemap_t;
	edgemap_t _edge_map;
	edgemap_t _f2t_map;
	edgemap_t _t2f_map;


	int _csvpiped[2];
	vector<int> _modBLkNo;
	vector<int> _likBLkNo;
	vector<int> _BLkcnt;
	vector<int> _FDlist;
	multimap<int, int> _countRank;
	vector< set<int> > _BLkRunlist;
	int _blockmax;
	kgSplitBlock _spblk;

	argST *_argst;

	// ####################################
	// DEBUG TOOLS
	// ####################################

	void debugARGST_OUTPUT(size_t i){
		if(_argst &&  i< _clen ){
			cerr << i << ":"<< _argst[i].mobj->name() << " " << _argst[i].i_cnt << " " << _argst[i].o_cnt ;
			if ( _argst[i].i_cnt > 0&& _argst[i].i_p!=NULL){
				cerr << " i:" ;
				for(int j=0; j< _argst[i].i_cnt;j++){
					cerr <<  *(_argst[i].i_p+j) << " " ;
				}
			}
			if ( _argst[i].o_cnt > 0 && _argst[i].o_p!=NULL){
				cerr << " o:" ;
				for(int j=0; j< _argst[i].o_cnt;j++){
					cerr <<  *(_argst[i].o_p+j) << " " ;
				}
			}
			cerr << endl;
		}
		else {
			cerr << "yet alocate" << endl;
		}
	}
	
	void debugIOinfo_OUTPUT(){
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
	}

	void raw_OUTPUT(const string& v);
 	void end_OUTPUT(const string& v);
	void war_OUTPUT(const string& v);
	void err_OUTPUT(const string& v);

	void runClean(void){
		if(_watchFlg){
			PyThreadState *savex = PyEval_SaveThread();
			pthread_join(_th_st_watch,NULL);
			PyEval_RestoreThread(savex);
			_watchFlg=false;
		}
		if(_th_st_pp){
			//エラー発生時はthread cancel
			for(size_t j=0;j<_clen;j++){
				pthread_cancel(_th_st_pp[j]);	
			}
			for(size_t i=_clen;i>0;i--){
				pthread_join(_th_st_pp[i-1],NULL);
			}
			if(_modlist){
				for(size_t i=0 ;i<_clen;i++){
					if(_argst[i].outputEND == false){
						if(!_argst[i].msg.empty()){
							raw_OUTPUT(_argst[i].msg);
						}
						if(!_argst[i].tag.empty()){
							raw_OUTPUT(_argst[i].tag);
						}
					}
					_argst[i].outputEND = true;
					if(_modlist[i]) { delete _modlist[i]; _modlist[i]=NULL; }
					if(_envlist[i]) { delete _envlist[i]; _envlist[i]=NULL; }
				}
				delete[] _modlist;
				delete[] _envlist;
			}
			delete[] _th_st_pp;
			_th_st_pp = NULL;
			_modlist = NULL;
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
			delete[] _argst;
			_argst =NULL;
		}
		if(_th_rtn){
			delete[] _th_rtn;
			_th_rtn = NULL;
		}
		if(_runst){
			delete[] _runst;
			_runst = NULL;
		}

	}
	int setArgStIO(kgmod_ioinfo_t& iomap,string& cmdname,map<string,vector<int> > & iopipeMap,int **io_p);
	void runInit(vector<cmdCapselST> &cmds,vector<linkST> & plist);
	void makePipeList(vector<linkST>& plist,int iblk,bool outpipe);
	int threadStkINIT(pthread_attr_t *pattr);
	int runMain(vector<cmdCapselST> &cmds,vector<linkST> & plisti,int iblk,bool outpipe=false);

public:
	// コンストラクタ
	kgshell(int mflg=false,int modlim = -1,size_t memttl=2000000000,int pymsg=0);
	~kgshell(void){
		if(_watchFlg){
			PyThreadState *savex = PyEval_SaveThread();
			pthread_join(_th_st_watch,NULL);
			PyEval_RestoreThread(savex);

			_watchFlg=false;
		}
		if(_th_st_pp){
			vector<int> chk(_clen);
			for(size_t i=0 ;i<_clen;i++){ 
				chk[i] = pthread_cancel(_th_st_pp[i]);
				if (chk[i]!=0&&chk[i]!=3){
					kgMsg msg(kgMsg::MSG, &_env);
					msg.output("waring destruct fail thread cancel :( "+ toString(chk[i]) + ")");
				}
			}
			for(size_t i=0 ;i<_clen;i++){ 
				if(chk[i]==0||chk[i]==3){
					int rtn = pthread_join(_th_st_pp[i],NULL);
					if(rtn!=0) {
						kgMsg msg(kgMsg::MSG, &_env);
						msg.output("waring destruct fail thread join :( " + toString(rtn) + ")");
					}
				}
			}
			delete[] _th_st_pp ;
		}
		if(_modlist){
			for(size_t i=0 ;i<_clen;i++){
				if(_modlist[i]){ delete _modlist[i]; _modlist[i] = NULL; }
				if(_envlist[i]){ delete _envlist[i]; _envlist[i] = NULL; }
			}
			delete[] _modlist;
			delete[] _envlist;
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
			delete[] _argst;
			_argst =NULL;
		}
		if(_th_rtn){
			delete[] _th_rtn;
			_th_rtn = NULL;
		}
		if(_runst){
			delete[] _runst;
			_runst = NULL;
		}


		if(_iterrtn){ delete _iterrtn;}
		if(_iterrtnk){ delete _iterrtnk;}
	}
	//実行メソッド
	static void *run_func(void *arg);
	static void *run_writelist(void *arg);
	static void *run_readlist(void *arg);
	static void *run_pyfunc(void *arg);
	static void *run_watch(void *arg);

	int run(vector<cmdCapselST> &cmdcap,vector<linkST> & plist);
	int runx(vector<cmdCapselST> &cmdcap,vector<linkST> & plist);

	kgCSVfld* runiter(vector<cmdCapselST> &cmdcap,vector<linkST> & plist);
	kgCSVkey* runkeyiter(vector<cmdCapselST> &cmdcap,vector<linkST> & plist,vector<string> & klist);

	void cancel(void){ runClean();}

	int getparams(kgstr_t cmdname,PyObject* list);
	int getIoInfo(kgstr_t cmdname,PyObject* list,int iotp);



};
