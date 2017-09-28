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
//#undef tolower
//#undef toupper
//#undef isalpha
//#undef islower
//#undef isupper
//#undef isspace
//#undef isalnum
#include <fcntl.h>
#include <kgEnv.h>
#include <kgError.h>
#include <kgmod.h>
#include <kgCSV.h>
#include <kgshellfunc.h>
#include <vector>
#include <map>

#include <pthread.h>


using namespace kglib;
using namespace kgmod;

struct argST{
	kgMod * mobj;
	int i_p;
	int o_p;
	int m_p;
	int u_p;
	PyObject* list;
	
};

struct cmdCapselST{
	kgstr_t cmdname;
	vector<kgstr_t> paralist;
	int iotype; // 1:i,2:o,4:m,8:u
	kgstr_t istr;
	PyObject* iobj;
	kgstr_t mstr;
	PyObject* mobj;

};


class kgshell{

	typedef map<std::string, boost::function<kgMod* ()> > kgmod_map_t;
	typedef map<std::string, int > kgmod_run_t;
	kgmod_map_t    _kgmod_map; //keyword - 関数対応表
	kgmod_run_t    _kgmod_run;
  kgEnv _env;
	bool _nfni;
 	kgCSVfld* _iterrtn;
	pthread_t* _th_st_pp;
	int _clen;
	kgMod **_modlist;

	// pipe LIST
	map<int,int> _ipipe_map;
	map<int,int> _mpipe_map;
	map<int,int> _opipe_map;
	int _lastpiped[2];

	void makePipeList(vector< vector<int> >& plist,bool tp);


public:
	// コンストラクタ
	kgshell(void);
	~kgshell(void){
		if(_th_st_pp){
			vector<int> chk(_clen);
			for(size_t i=0 ;i<_clen;i++){ 
				chk[i] = pthread_cancel(_th_st_pp[i]);
				if (chk[i]!=0&&chk[i]!=3){ cerr << "waring destruct fail thread cancel : " << chk[i] << endl;}
			}
			for(size_t i=0 ;i<_clen;i++){ 
				if(chk[i]==0||chk[i]==3){
					int rtn = pthread_join(_th_st_pp[i],NULL);
					if(rtn!=0) { cerr << "waring destruct fail thread join : " << rtn << endl; }
				}
			}
			delete[] _th_st_pp ;
		}
		if(_modlist){
			for(size_t i=0 ;i<_clen;i++){
				delete _modlist[i];
			}
			delete[] _modlist;
			
		}
		if(_iterrtn){
			delete _iterrtn;
		}
	}
	//実行メソッド
	static void *run_noargs_pths(void *arg);
	static void *run_noargs_pthsm(void *arg);
	static void *run_noargs_pths1(void *arg);
	static void *run_noargs_pthsp(void *arg);

	int run(vector< cmdCapselST > &cmdcap,	vector< vector<int> >& plist,bool tp,PyObject* list);

	kgCSVfld* runiter(vector< cmdCapselST > &cmdcap,	vector< vector<int> >& plist,bool tp,PyObject* list);
	int getparams(kgstr_t cmdname,PyObject* list);


};
