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
// kgcut.h 項目の選択クラス
// =============================================================================
#pragma once
#include <kgmod.h>
#include <kgError.h>
#include <kgArgFld.h>
#include <kgCSV.h>
#include <kgCSVout.h>

using namespace kglib;

namespace kgmod { ////////////////////////////////////////////// start namespace


class kg2Cat : public kgMod {

	// 引数
	kgArgFld _fField;  // f=
	kgCSVfld **_iFiles;   // i=
	kgCSVout _oFile;   // o=

	size_t _itotal;
	size_t _iCnt;
	bool _first ;
	vector<kgstr_t> _fldname1;

	// 引数セット
	void setArgs(void);
	void setArgs(int inum,int *i,int onum, int* o);

	int runMain(void);
	void setArgsMain(void);	
	void readwrite(size_t pos);

public:
	static const char * _ipara[];
	static const char * _opara[];

	// コンストラクタ
	kg2Cat(void);
	~kg2Cat(void){
		for(size_t i=0 ; i<_itotal ;i++){
			delete _iFiles[i];
		}
		delete _iFiles;
	}

	// 処理行数取得メソッド
	size_t iRecNo(void) const { return _iCnt; }
	size_t oRecNo(void) const { return _oFile.recNo(); }


	//実行メソッド
	int run(void);
	int run(int inum,int *i_p,int onum, int* o_p ,string & str);

	void runErrEnd(void){
		//_iFile.close();
		_oFile.forceclose();
	}


};


}


