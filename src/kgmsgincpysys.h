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
// kgMsgIncPySys.h メッセージ関係クラス
// =============================================================================
#pragma once
#include "Python.h"
#include <iostream>
#include <string>
#include <cstdio>
#include <kgConfig.h>
#include <kgMessage.h>

using namespace std;

namespace kglib { ////////////////////////////////////////////// start namespace

// メッセージ出力クラス
class kgMsgIncPySys : public kgMsg{

	private:

		void WriteMsgPySys(string v ,string t);

	public:
		kgMsgIncPySys(kgMsg::Plevel plevel, kgEnv* env):kgMsg(plevel,env){}

		void output(const string& v, const string& comment="");
		void output_ignore(const string& v);


};

} //////////////////////////////////////////////////////////////// end namespace

