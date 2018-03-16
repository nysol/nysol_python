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
import os
import shutil

def mkDir(dname,rm=False):
	import sys
	if sys.version_info.major >= 3:

		try:
			if os.path.exists(dname):
				if rm :
					shutil.rmtree(dname)
					os.makedirs(dname)
			
			else:
				os.makedirs(dname)

		except FileExistsError:
			pass

	else:

		try:
			if os.path.exists(dname):
				if rm :
					shutil.rmtree(dname)
					os.makedirs(dname)
			
			else:
				os.makedirs(dname)

		except OSError as err:
			if err.errno != errno.EEXIST:
				raise

