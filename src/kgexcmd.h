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
// kgtab2csv.h tsv->csv
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
#include <kgmod.h>
#include <kgArgFld.h>
#include <kgMethod.h>
#include <sys/types.h>
#include <sys/wait.h>
using namespace kglib;

namespace kgmod { ////////////////////////////////////////////// start namespace

class kgExcmd : public kgMod 
{

	// 引数セット
	void setArgs(void);
//	char * const * 
	kgstr_t _cmdstr;
	vector<kgstr_t> _cmdstrv;
	char const ** _cmdars;

public:
	static const char * _ipara[];
	static const char * _opara[];
	// コンストラクタ
	kgExcmd(void);
	~kgExcmd(void){}


	//実行メソッド
	int run(void);
	//実行メソッド
	int run(int inum,int *i_p,int onum, int* o_p ,string & str);

};

}
