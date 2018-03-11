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

#include <kgsplitblock.h>
#include <vector>
#include <map>
#include <iostream> // debug
using namespace std;

//loopが無いことが前提
/*

void kgSplitBlock::reblockSub( int st, int blockNo, int layer, i_iv_t& layermap,int oldblk,int bp)
{
	cerr << "reblock sub start" << endl;

	if(_modBLkNo[st]!=oldblk || st==bp){ return ; }

	_modBLkNo[st] = blockNo;
	_BLkcnt[blockNo]++;
	if( layermap.find(layer) == layermap.end()){
		vector<int> newvec;
		layermap[layer] = newvec;
	}
	layermap[layer].push_back(st);

	if(_edge_map.find(st) == _edge_map.end()){ return; } 

	if(_t2f_map.find(st) != _t2f_map.end()){
		for ( size_t i=0 ; i<_t2f_map[st].size();i++){
			makeBLKSub(_t2f_map[st][i],blockNo,layer-1,layermap);
		}
	}
	if(_f2t_map.find(st) != _f2t_map.end()){
		for ( size_t i=0 ; i<_f2t_map[st].size();i++){
			makeBLKSub(_f2t_map[st][i],blockNo,layer+1,layermap);
		}
	}
	
}

void kgSplitBlock::reblockWW(int blockNo,int nowbpos){
	
	cerr << "reblock start" << endl;

	multimap<int, int> junctionRank;
	vector<int> newstpos;

	for(int i=0;i<_node;i++){

		if(_modBLkNo[i]!=blockNo){ continue; }
		if(_stPos.find(i)!=_stPos.end()){ newstpos.push_back(i); }

		int count =0;
		if(_f2t_map.find(i)!=_f2t_map.end() &&
				_f2t_map[i].size()>1){ count += _f2t_map[i].size(); }

		// とりあえず上だけ
		//if(_t2f_map.find(i)!=_t2f_map.end() &&
		//		_t2f_map[i].size()>1){ count += _t2f_map[i].size(); } 
		
		if(count!=0){
			junctionRank.insert(multimap<int, int>::value_type(count,i));
		}
	}

	for( multimap<int, int>::reverse_iterator j = junctionRank.rbegin(); j != junctionRank.rend() ; ++j ) {
		i_iv_t layermap0;

		for(vector<int>::iterator i =newstpos.begin();i!=newstpos.end();i++){
			if (_modBLkNo[*i] != blockNo) { continue; }
			reblockSub(*i,nowmaxblock,0,layermap0,blockNo,j->second);
			nowbpos++;
		}

		reblockSub(_f2t_map[j->second][i],nowmaxblock,0,layermap1,blockNo,j->second);
			nowmaxblock++;
		}
		cerr << "=======" << endl;
		cerr << "=====check==" << endl;
		
		
		cerr << "=======" << endl;
		break;
	}
}
*/

//layer split
int kgSplitBlock::reblock(int blockNo,int nowbpos){

	int nowsize = 0;
	for(i_iv_itr it = _layer_maps[blockNo].begin() ; it!= _layer_maps[blockNo].end();it++){
		if(nowsize + it->second.size() > _blockLimit){
			nowbpos++;
			nowsize = 0 ;
			for(size_t j=0;j<it->second.size();j++){
				_splitNode.insert(it->second[j]);
			}
		}
		if(it->second.size() >_blockLimit){
			int slot = it->second.size()/_blockLimit + 1 ;
			int val = it->second.size()/slot+1;
			int cnt=0;
			for(size_t j=0;j<it->second.size();j++){

				if(cnt > val){  cnt=0;  nowbpos++; }
				_modBLkNo[it->second.at(j)] = nowbpos;
				cnt++;

			}
			nowbpos++;
			nowsize = 0 ;
		}
		else{
			for(size_t j=0;j<it->second.size();j++){
				_modBLkNo[it->second.at(j)] = nowbpos;
			}
			nowsize += it->second.size();
		}
	}
	return nowsize;

}

void kgSplitBlock::makeBLKSub( int st, int blockNo, int layer, i_iv_t& layermap)
{
	if(_modBLkNo[st]!=-1){ return ; }

	_modBLkNo[st] = blockNo;
	_BLkcnt[blockNo]++;
	if( layermap.find(layer) == layermap.end()){
		vector<int> newvec;
		layermap[layer] = newvec;
	}
	layermap[layer].push_back(st);

	if(_edge_map.find(st) == _edge_map.end()){ return; } 

	if(_t2f_map.find(st) != _t2f_map.end()){
		for ( size_t i=0 ; i<_t2f_map[st].size();i++){
			makeBLKSub(_t2f_map[st][i],blockNo,layer-1,layermap);
		}
	}
	if(_f2t_map.find(st) != _f2t_map.end()){
		for ( size_t i=0 ; i<_f2t_map[st].size();i++){
			makeBLKSub(_f2t_map[st][i],blockNo,layer+1,layermap);
		}
	}
	
}


int kgSplitBlock::makeBLK(void){
	// calc start point & edge map
	vector<bool> notStart(_node,false);
	for(int i=0;i<_edge.size();i++){
		notStart[_edge[i].toID]=true;
		if ( _edge_map.find(_edge[i].toID) == _edge_map.end() ){
			vector<int> newvec;
			_edge_map[_edge[i].toID] = newvec;
		}
		if ( _t2f_map.find(_edge[i].toID) == _t2f_map.end() ){
			vector<int> newvec;
			_t2f_map[_edge[i].toID] = newvec;
		}
		_edge_map[_edge[i].toID].push_back(_edge[i].frID);
		_t2f_map[_edge[i].toID].push_back(_edge[i].frID);


		if ( _edge_map.find(_edge[i].frID) == _edge_map.end() ){
			vector<int> newvec;
			_edge_map[_edge[i].frID] = newvec;
		}
		if ( _f2t_map.find(_edge[i].frID) == _f2t_map.end() ){
			vector<int> newvec;
			_f2t_map[_edge[i].frID] = newvec;
		}
		_edge_map[_edge[i].frID].push_back(_edge[i].toID);
		_f2t_map[_edge[i].frID].push_back(_edge[i].toID);
	}
	// find  start point & juction
	for(int i=0;i<_node;i++){
		if(notStart[i]==false){ _stPos.insert(i); }
	}

	//find BLOCK
	_modBLkNo.resize(_node,-1);
	_likBLkNo.resize(_edge.size(),-1);
	_BLkcnt  .resize(_node,0);
	int blockNo = 0;
	for(set<int>::iterator i =_stPos.begin();i!=_stPos.end();i++){
		if (_modBLkNo[*i] != -1) { continue; }
		i_iv_t layermap;
		makeBLKSub(*i,blockNo,0,layermap);
		_layer_maps.push_back(layermap);
		blockNo++;
	}
	//find lim over BLOCK
	int nowbpos = blockNo;
	for(int i = 0 ; i<blockNo;i++){
		if(_BLkcnt[i]>_blockLimit){
			nowbpos = reblock(i,nowbpos);
		}		
	}
	
	return nowbpos;
}


void kgSplitBlock::blockSplit(int maxsize){

	_blockLimit=maxsize;
	_modBLkNo.clear();
	_likBLkNo.clear();
	_BLkcnt.clear();
	_stPos.clear();
	_BLkmodlist.clear();
	_BLklinklist.clear();


	if(_blockLimit > _node ){
		//_modBLkNo.resize(_node,0);
		//_likBLkNo.resize(_edge.size(),0);
		//_BLkRunlist.resize(1);
		_BLkmodlist.resize(1);
		_BLklinklist.resize(1);
		for(size_t i=0 ; i<_node ;i++){
			_BLkmodlist[0].push_back(i);
		}
		for(size_t i=0 ; i<_edge.size();i++){
			_BLklinklist[0].push_back(i);
		}
		return;
	}

	int blockNo = makeBLK();
	_BLkmodlist .resize(blockNo);
	_BLklinklist.resize(blockNo);


//	for(set<int>::iterator j =_splitNode.begin();j!=_splitNode.end();j++){
//		cerr << *j << endl;
//	}

	
	// set block No (mod&link) 
	for(int i=0;i<_edge.size();i++){
		//cerr << i << " " << _modBLkNo[i] << endl;
		if( _splitNode.find(_edge[i].toID) != _splitNode.end()){
			_splitEdge.push_back(_edge[i]);
			continue;
		}
		if(_modBLkNo[_edge[i].toID]!=_modBLkNo[_edge[i].frID]){
			cerr << "no match id  "<< _edge[i].toID << " " << _edge[i].frID ;
			cerr << " " << _modBLkNo[_edge[i].toID] << " " << _modBLkNo[_edge[i].frID] << endl;
		}
		_BLklinklist[_modBLkNo[_edge[i].toID]].push_back(i);
	}

	for(int i=0;i<_node;i++){
		_BLkmodlist[_modBLkNo[i]].push_back(i);
	}

	//DEBUG
	/*
	for(int i=0;i<blockNo;i++){
		cerr << "===== BLOCK " << i << " =======" << endl;
		cerr << "node : size=" << _BLkcnt[i] << endl;
		for( i_iv_t::iterator j=_layer_maps[i].begin() ;j!=_layer_maps[i].end() ;j++){
			cerr << j->first << ":";
			for(size_t jj=0 ; jj< j->second.size() ; jj++){
				cerr << j->second[jj]  << " ";
			}
			cerr << endl;
		}
		for(int j=0;j<_modBLkNo.size();j++){
			if(_modBLkNo[j]==i) { cerr << j << " ";}
		}
		cerr << endl;
		cerr << "edge : " << endl;
		for(int j=0;j<_likBLkNo.size();j++){
			if(_likBLkNo[j]==i) { 
				cerr << "("<< _edge[j].frID << ":" <<  _edge[j].frTP << "=>" <<  _edge[j].toID << ":"<< _edge[j].toTP << ")" << endl;
			}
		}
*/


	

}
