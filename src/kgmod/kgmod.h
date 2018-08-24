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
// kgmod.h : モジュールの基本クラス
// =============================================================================
#pragma once
#include "Python.h"
#undef tolower
#undef toupper
#undef isalpha
#undef islower
#undef isupper
#undef isspace
#undef isalnum
#include <kgArgs.h>
#include <kgEnv.h>
#include <kgConfig.h>
#include <csignal>
#include <cstring>
#include <pthread.h>
#include <cxxabi.h>

#if !defined(__clang__) && defined(__GNUC__)
#define KG_ABI_CATCH catch(abi::__forced_unwind&){ runErrEnd(); throw; }
#else
#define KG_ABI_CATCH  
#endif

using namespace kglib;

namespace kglib  ////////////////////////////////////////////// start namespace
{
class kgMod
{
	protected:
	kgEnv*       _env;     // 環境変数
	kgArgs       _args;    // 引数
	const char*  _name;    // コマンド名
	const char*  _version; // バージョン
	const char*  _doc; 		 // デフォルトヘルプドキュメント
	const char*  _title; 	 // デフォルトモジュールタイトル
	const char*  _libversion; // バージョン
	const char*  _modversion; // MODバージョン
	const char*  _docL; 		 // ローカルヘルプドキュメント
	const char*  _titleL; 	 // ローカルモジュールタイトル
	const char*  _paralist; //paralist
	int    _paraflg;
	
	// 以下は全コマンドに共通する引数
	// 引数として指定されていなければkgEnvにセットされている値をセットする
	// セットされるタイミング
	//  1. kgEnvに#defineで定義された値に初期化される．
	//  2. 環境変数が設定されていれば上書きする．
	//  3. 引数として指定されていればさらに上書きする．
	// -nfnの指定で _nfn_i=true,  _nfn_o=true,  _fldByNum=true
	// -xの指定で   _nfn_i=false, _nfn_o=false, _fldByNum=true
	kgstr_t _tmpPath;   // TmpPath=
	string  _encoding;  // encoding
	int     _precision; // Double型の有効桁数
	bool    _nfn_i;     // 1行目が項目行かどうか
	bool    _nfn_o;     // 項目名を出力しない
	bool    _fldByNum;  // 項目の番号指定フラグ
	int 		_status;		// exitステータス
	bool    _localhelp;  // 項目の番号指定フラグ

	// ASSERT
	bool _assertDIff; // in-out-check
	bool _assertNullIN;
	bool _assertNullOUT;
	bool _assertNullKEY;
	bool _existNullIN;
	bool _existNullOUT;
	bool _existNullKEY;

	static void signalHandler(int sigNo, siginfo_t* info, void* ctx);

	void AssertWarnig(void);

	// 初期設定
	virtual void init(void);
	void signalset(void);
	void signalreset(void);
	
	public:
	kgMod(void);
	virtual ~kgMod(void){
		signalreset();
	}
	
	virtual void init(kgArgs args, kgEnv* env);
	virtual void init(size_t argc, const char* argv[], kgEnv* env);


	// accessor
	const char* name(void)		 const{ return _name; }
	const char* version(void)	 const{ return _version; }
	kgstr_t tmpPath(void)			 const{ return _tmpPath; }
	string  encoding(void)		 const{ return _encoding; }
	kgEnv*  env(void)          const{ return _env; }
	bool    nfn_i(void)				 const{ return _nfn_i; }
	bool    nfn_o(void)				 const{ return _nfn_o; }
	int     precision(void)		 const{ return _precision; }
	bool    fldByNum(void)		 const{ return _fldByNum; }
	int status( void )  			 const{ return _status; }
	const char* title(void)		 const{ return _title; }

	const char* doc(void)	 		 const{ 
		char *envStr = getenv("KG_LOCALHELP");
		if(envStr!=NULL && *envStr !='\0') { return _docL;} 
		else						 { return _doc;} 
	}

	const char* titlel(void)		 const{ return _titleL; }
	const char* docl(void)	 		 const{ return _docL; }


	string lver(void){ 
		if(!strcmp(_version,"###VERSION###")){
			return  string("lib Version ") + _libversion + string(": mod Version ") + _modversion ;
		}
		else{
			return  string("lib Version ") + _libversion + string(": mod Vesion ") + _modversion + string(": cmdmod Version ") + _version ;
		}
	}


	kgArgs* args(void)	 			 			{ return &_args; }

	string cmdline(void) const;
	
	// 正常終了処理(メッセージ出力)
	void successEnd(void);
	void successEnd(const string& comment);

	// エラー終了処理(メッセージ出力)
	void errorEnd(kgError& err);
	void errorEnd(void);
	void errorEnd(kgError& err, const string& comment);
	void errorEnd(const string& comment);

	string errorEndMsg(kgError& err);
	string successEndMsg(void);


	// 入力データ件数と出力データ件数
	// 各モジュールで独自に登録する．
	// 登録しなければ-1を返し，メッセージとして表示されない．
	virtual size_t iRecNo(void) const {return -1;}
	virtual size_t oRecNo(void) const {return -1;}
	virtual size_t getSmod(void){ return 0;}
	virtual void signalEnd(void){ return ;}
	// ----------------------------------------------------------------
	// 各コマンドクラスにおいて実装しなければならない純粋仮想関数
	// ----------------------------------------------------------------
	virtual int run(void)=0;
	vector< vector<string> > params(void);

	// ----------------------------------------------------------------
	// run仮想関数
	// ----------------------------------------------------------------
	virtual int run(int i,int o){return 0;};
	virtual int run(int inum,int *i,int onum, int* o,string & msg){return 0;}
	virtual int run(int inum,int *i,PyObject* o,pthread_mutex_t *mtx,string & msg){return 0;};
	virtual int run(PyObject* i_p,int onum, int* o_p,string &msg){return 0;}
	virtual int run(PyObject* f_p,PyObject* a_p,PyObject* k_p,int inum,int *i,int onum, int* o,string & msg,pthread_mutex_t *mtx,vector<int> fdlist){return 0;}

	virtual int run(PyObject* f_p,PyObject* a_p,PyObject* k_p,int inum,int *i,int onum, int* o,string & msg,pthread_mutex_t *mtx,pthread_cond_t *fCond,volatile int *runst){return 0;}

	virtual int run(PyObject* f_p,PyObject* a_p,PyObject* k_p,int inum,int *i,int onum, int* o,string & msg,pthread_mutex_t *mtx,pthread_cond_t *fCond,volatile int *runst,vector<int> fdlist){return 0;}



};

} //////////////////////////////////////////////////////////////// end namespace

