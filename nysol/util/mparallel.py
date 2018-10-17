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
import copy

	
class MparallelManager(object):

	def __init__(self,mp=4):
		self.mp = mp 					# パラレルサイズ
		self.runpid = {} 			# pid => laneNo ## 動いてるPROCESS
		self.LaneQue = list(range(mp))

		# stsチェックするなら追加
		#self.thInterval = tim # チェック間隔
		#self.slppid = []			# [ [pid ,laneNo child pid] ... ## 休止中PROCESS
		#self.mtx =  Mutex.new if @thInterval > 0
		#self.mtx =  None


	# プロセス終了確認
	def waitLane(self):

		finLane =[]
		
		while(True):
			waitbrk=False
			rpid = None
			while(True):
				for k in self.runpid:
					if os.waitpid(k,os.WNOHANG) != (0,0):
						waitbrk = True
						rpid = k
						break
				if waitbrk:
					break

			new_pno = None
			if rpid != None:
				new_pno = self.runpid.pop(rpid)	

			if new_pno != None :
				finLane.append(new_pno)
				self.LaneQue.append(new_pno)
				break
				
		return finLane
		
	# 全プロセス終了確認
	def waitall(self):
		rtn = []
		while(len(self.runpid)!=0 or len(self.runpid)!=0):
			rtn.extend(self.waitLane()) 

		return rtn

	# 空き実行レーン取得
	def getLane(self , wait=True):
		if wait and len(self.LaneQue)==0:
			self.waitLane()
		return self.LaneQue.pop(0)
		

	def addPid(self,pid,lane):

		self.runpid[pid]=lane
			

# 並列処理each
def meach(func , para , *, mpCount=4 ,addcntNo=False,addprocNo=False ):

	params = copy.deepcopy(para)
	ttl    = len(params)
	addpara=0
	if addcntNo and addprocNo:
		addpara=3
	elif addcntNo :
		addpara=1
	elif addprocNo :
		addpara=2

	nowcnt = 0 

	mpm = MparallelManager(mpCount)

	while len(params)>0 :

		param=params.pop(0) 
		nowlane = mpm.getLane()
		pid=os.fork()
		if pid == 0:
			if addpara ==0 :
				func(param)
			elif addpara ==1 :
				func(param,nowcnt)
			elif addpara ==2 :
				func(param,nowlane)
			elif addpara ==3 :
				func(param,nowcnt,nowlane)
			os._exit(0)
		else:
			mpm.addPid(pid,nowlane)
		nowcnt+=1
		
	mpm.waitall()
	return []





