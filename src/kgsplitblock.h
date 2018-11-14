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

#include <vector>
#include <set>
#include <map>
#include <kgshelltp.h>


using namespace std;

struct laySt{
	int _lay;
	bool _sumi;
	laySt(int lay=0,bool sumi=false){
		_lay = lay; 
		_sumi = sumi;
	}
	
};

class kgSplitBlock{

	size_t _node;
	size_t _blockLimit;

	vector <linkST> _edge;
	vector<int>	_modBLkNo;
	vector<int> _likBLkNo;
	vector<size_t> _BLkcnt;
	set<int>    _stPos;
	vector< vector<int> > _BLkstPos;


	//最終ケータイ
	vector< vector<int> > _BLkmodlist;
	vector< vector<int> > _BLklinklist;


	vector< vector<int> > _runUnitBLklist;

	int _runBlkCnt;
	set<int>           _splitNode;
	vector <linkST>    _splitEdge;

	typedef map<int, vector<int> > i_iv_t;
	typedef map<int, vector<int> >::iterator i_iv_itr;

	i_iv_t _edge_map;
	i_iv_t _f2t_map;
	i_iv_t _t2f_map;

	vector<i_iv_t> _layer_maps;

	map<int,laySt> _id_layer;
	
	int reblock(int blockNo,int nowmaxblock);
	int reblock(int blockNo,int nowmaxblock,i_iv_t & layermap);

//	void reblockSub( int st, int blockNo, int layer, i_iv_t& layermap,int oldblk,int bp);
//	void makeBLKSub(int st,int blockNo,int layer,i_iv_t& layermap);
	void makeBLKSub(int st,int blockNo);

	int  makeBLK(void);

	public:
		kgSplitBlock():_node(0){}
		kgSplitBlock(int node, vector <linkST>edge):_node(node),_edge(edge){}
		void blockSplit(int maxsize);
		void blockSplit(int maxsize,int node, vector <linkST>edge){
			_node = node;
			_edge = edge;
			blockSplit(maxsize);
		}
		vector<int>& getModBlkInfo(int i){ return  _BLkmodlist[i]; }
		vector<int>& getLinkBlkInfo(int i){ return  _BLklinklist[i]; }


		vector <linkST>& getsplitEdge(void){ return  _splitEdge; }

		int getModBlkSize(int i){ return  _BLkmodlist[i].size(); }
		int getLinkBlkSize(int i){ return  _BLklinklist[i].size(); }

		int getBlksize(){ return _BLkmodlist.size();}

		int getBlksize_M(){ return _runBlkCnt;}

		// ============================
		// f.w
		// ============================
		void makeLayer(int blockNo,vector<int>& stps,i_iv_t & layermap);

		int getModBlkSize_M(int i){ 
			int rtn = 0;
			for(size_t j=0;j<_runUnitBLklist[i].size();j++){
				rtn += getModBlkSize(_runUnitBLklist[i][j]);
			}
			return rtn; 
		}

		int getLinkBlkSize_M(int i){
			int rtn = 0;
			for(size_t j=0;j<_runUnitBLklist[i].size();j++){
				rtn += getLinkBlkSize(_runUnitBLklist[i][j]);
			}
			return rtn;
		}
		
		vector<int> getLinkBlkInfo_M(int i){
			vector<int> rtn;
			for(size_t j=0;j<_runUnitBLklist[i].size();j++){
				vector<int> addvec = getLinkBlkInfo(_runUnitBLklist[i][j]);
				rtn.insert(rtn.end(), addvec.begin(), addvec.end());
			}
			return rtn;
		}
		vector<int> getModBlkInfo_M(int i){
			vector<int> rtn;
			for(size_t j=0;j<_runUnitBLklist[i].size();j++){
				vector<int> addvec = getModBlkInfo(_runUnitBLklist[i][j]);
				rtn.insert(rtn.end(), addvec.begin(), addvec.end());
			}
			return rtn;
		}


};
