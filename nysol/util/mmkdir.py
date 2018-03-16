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
import errno
import shutil

def _makedirs27(name, mode=0o777, exist_ok=False):
	"""makedirs(name [, mode=0o777][, exist_ok=False])
	Super-mkdir; create a leaf directory and all intermediate ones.  Works like
	mkdir, except that any intermediate path segment (not just the rightmost)
	will be created if it does not exist. If the target directory already
	exists, raise an OSError if exist_ok is False. Otherwise no exception is
	raised.  This is recursive.
	"""
	head, tail = os.path.split(name)
	if not tail:
		head, tail = os.path.split(head)
	if head and tail and not os.path.exists(head):
		try:
			_makedirs27(head, exist_ok=exist_ok)
		except IOError as err:
			if err.errno != errno.ENOENT: # ENOENT: no such file or directory
				raise
			# Defeats race condition when another thread created the path
			pass
			cdir = curdir
			if isinstance(tail, bytes):
				cdir = bytes(curdir, 'ASCII')
			if tail == cdir:           # xxx/newdir/. exists if xxx/newdir exists
				return
	try:
		os.mkdir(name, mode)
	except OSError:
		# Cannot rely on checking for EEXIST, since the operating system
		# could give priority to other errors like EACCES or EROFS
		if not exist_ok or not os.path.isdir(name):
			raise


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
					_makedirs27(dname)
			
			else:
				_makedirs27(dname)

		except OSError as err:
			if err.errno != errno.EEXIST:
				raise

