#!/usr/bin/env python
# -*- coding: utf-8 -*-
#/* ////////// LICENSE INFO ////////////////////
#
# * Copyright (C) 2013 by NYSOL CORPORATION
# *
# * Unless you have received this program directly from NYSOL pursuant
# * to the terms of a commercial license agreement with NYSOL, then
# * this program is licensed to you under the terms of the GNU Affero General
# * Public License (AGPL) as published by the Free Software Foundation,
# * either version 3 of the License, or (at your option) any later version.
# * 
# * This program is distributed in the hope that it will be useful, but
# * WITHOUT ANY EXPRESS OR IMPLIED WARRANTY, INCLUDING THOSE OF 
# * NON-INFRINGEMENT, MERCHANTABILITY OR FITNESS FOR A PARTICULAR PURPOSE.
# *
# * Please refer to the AGPL (http://www.gnu.org/licenses/agpl-3.0.txt)
# * for more details.
#
# ////////// LICENSE INFO ////////////////////*/

#あとで作り直す
def mrecount(**kw_args):
	import sys
	import subprocess
	if sys.version_info < (3, 0, 0) :
		print kw_args["i"]
		res = subprocess.check_output(["wc","-l",kw_args["i"]])
		cnt = int(res.strip().split()[0])

	else:
		import subprocess
		res = subprocess.run(["wc", "-l", kw_args["i"]], stdout=subprocess.PIPE)
		cnt = int(res.stdout.decode('utf-8').strip().split()[0])

	if ("nfni" in kw_args and kw_args["nfni"] == True) or ("nfn" in kw_args and kw_args["nfn"] == True):
		return cnt
	else:
		if cnt > 0:
			cnt -=1
		return cnt

