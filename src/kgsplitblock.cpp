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

// ここは整理し直す
//layer split
int kgSplitBlock::reblock(int blockNo,int nowbpos){

	int nowsize = 0;
	for(i_iv_itr it = _layer_maps[blockNo].begin() ; it!= _layer_maps[blockNo].end();it++){

		if(nowsize + it->second.size() > _blockLimit){
			nowbpos++;
			nowsize = 0 ;
			/*for(size_t j=0;j<it->second.size();j++){
				_splitNode.insert(it->second[j]);
			}*/
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

//layer split
int kgSplitBlock::reblock(int blockNo,int nowbpos,i_iv_t & layermap){

	int nowsize = 0;
	for(i_iv_itr it = layermap.begin() ; it!= layermap.end();it++){
		//cerr << "it-second:" << "(" << it->second.size() << ")" ;
		//for(size_t j=0;j<it->second.size();j++){
		//	cerr << " " << it->second.at(j);
		//}
		//cerr << endl;
		if(nowsize + it->second.size() > _blockLimit){
			nowbpos++;
			nowsize = 0 ;
			//for(size_t j=0;j<it->second.size();j++){
			//	_splitNode.insert(it->second[j]);
			//}
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
	return nowbpos+1;

}

//layer split
void kgSplitBlock::makeLayer(int blockNo,vector<int>& stps,i_iv_t & layermap){
	
	vector<int> newstps = stps;
	//vector<int> newlayer =layer;
	set<int> nextstps;
	//vector<int> nextlayer;
	
	while(1){
		for ( size_t i =0 ; i<newstps.size() ; i++){
			int st = newstps[i];
			
			while(1){
				if( _id_layer.find(st)  != _id_layer.end() ){
					if(_id_layer[st]._sumi == true){ break;}
				}
				if(_t2f_map.find(st) != _t2f_map.end()){
					bool preSumi = true;
					for(size_t j=0 ;j<_t2f_map[st].size();j++){
						if ( _id_layer.find(_t2f_map[st][j]) == _id_layer.end()){
							preSumi = false;
							break;
						}
						if( _id_layer[_t2f_map[st][j]]._sumi==false){
							preSumi = false;
							break;
						}
						else{
							if( _id_layer.find(st) == _id_layer.end()){
								_id_layer[st] = laySt( _id_layer[_t2f_map[st][j]]._lay+1,false);
							}
							if ( _id_layer[st]._lay < _id_layer[_t2f_map[st][j]]._lay + 1){
								_id_layer[st]._lay = _id_layer[_t2f_map[st][j]]._lay + 1;
							}
						}
					}
					if(!preSumi){
						 nextstps.insert(st);
						 break;
					}
					_id_layer[st]._sumi = true;

					if( layermap.find(_id_layer[st]._lay) == layermap.end()){
						vector<int> newvec;
						layermap[_id_layer[st]._lay] = newvec;
					}
					layermap[_id_layer[st]._lay].push_back(st);


				}
				else{
					_id_layer[st] = laySt(0,true);

					if( layermap.find(0) == layermap.end()){
						vector<int> newvec;
						layermap[0] = newvec;
					}
					layermap[0].push_back(st);

				}

				if(_f2t_map.find(st) == _f2t_map.end()){
					break;
				}
				if(_f2t_map[st].size()==1){
					st = _f2t_map[st][0];
				}
				else{
					for(size_t j=0 ;j<_f2t_map[st].size();j++){
						 nextstps.insert(_f2t_map[st][j]);
					}
					break;
				}
			}
		}
		newstps.clear();
		for(set<int>::iterator i = nextstps.begin();i!=nextstps.end();i++){
			newstps.push_back(*i);
		}
		nextstps.clear();
		if ( newstps.size() == 0 ) break;
	}

}
/*
void kgSplitBlock::makeBLKSub_OLD( int st, int blockNo, int layer, i_iv_t& layermap)
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
*/
void kgSplitBlock::makeBLKSub( int st, int blockNo)
{
	if(_modBLkNo[st]!=-1){ return ; }

	_modBLkNo[st] = blockNo;
	_BLkcnt[blockNo]++;
	/*
	if( layermap.find(layer) == layermap.end()){
		vector<int> newvec;
		layermap[layer] = newvec;
	}
	layermap[layer].push_back(st);
	*/
	if(_edge_map.find(st) == _edge_map.end()){ return; } 

	if(_t2f_map.find(st) != _t2f_map.end()){
		for ( size_t i=0 ; i<_t2f_map[st].size();i++){
			makeBLKSub(_t2f_map[st][i],blockNo);
		}
	}
	if(_f2t_map.find(st) != _f2t_map.end()){
		for ( size_t i=0 ; i<_f2t_map[st].size();i++){
			makeBLKSub(_f2t_map[st][i],blockNo);
		}
	}
	
}


int kgSplitBlock::makeBLK(void){

	// calc start point & edge map
	vector<bool> notStart(_node,false);
	for(size_t i=0;i<_edge.size();i++){
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

	// find  start point & (juction f.w.)
	for(size_t i=0;i<_node;i++){
		if(notStart[i]==false){ _stPos.insert(i); }
	}

	//find BLOCK
	_modBLkNo.resize(_node,-1);
	_likBLkNo.resize(_edge.size(),-1);
	_BLkcnt  .resize(_node,0);

	int blockNo = 0;	
	for(set<int>::iterator i =_stPos.begin();i!=_stPos.end();i++){
		if (_modBLkNo[*i] != -1) { 
			_BLkstPos[_modBLkNo[*i]].push_back(*i);
			continue; 
		}
		_BLkstPos.resize(blockNo+1);
		_BLkstPos[blockNo].push_back(*i);
		//i_iv_t layermap;
		//makeBLKSub(*i,blockNo,0,layermap);
		makeBLKSub(*i,blockNo);
		//_layer_maps.push_back(layermap);
		blockNo++;
	}
	//find lim over BLOCK
	int nowbpos = blockNo;
	/*
	cerr << "blk " << blockNo << endl;
	for(int i=0;i<_BLkstPos.size();i++){
		cerr << "blk : " << i << "st" ;
		for(int j=0;j<_BLkstPos[i].size();j++){
			cerr << " " << _BLkstPos[i][j];
		}
		cerr << endl ;
	}
	*/

	for(int i = 0 ; i<blockNo;i++){
		if(_BLkcnt[i]>_blockLimit){
			_id_layer.clear();
			//for(int j=0;i<_BLkstPos[i].size();j++){
			//	_id_layer[_BLkstPos[i][j]]=laySt(0,true);
			//}
			//vector<int> stlay(_BLkstPos[i].size(),0)  
			i_iv_t layermap;
			makeLayer(i,_BLkstPos[i],layermap);
			nowbpos = reblock(i,nowbpos,layermap);
			
			//cerr << "check" << endl;
			//for ( map<int,laySt>::iterator it = _id_layer.begin(); it != _id_layer.end(); it++){
			//	cerr << it->first << ":" << it->second._lay << "(" << it->second._sumi << ")" << endl;
			//}
		}		
	}

/*
	for(int i = 0 ; i<blockNo;i++){
		if(_BLkcnt[i]>_blockLimit){
			nowbpos = reblock(i,nowbpos);
		}		
	}
*/	
//	cerr << "checkx" << nowbpos << endl;
	
	return nowbpos;
}


// ここ整理する
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
		_runUnitBLklist.resize(1);
		_runUnitBLklist[0].push_back(0);
		_runBlkCnt = 1;
		return;
	}

	int blockNo = makeBLK();


	// _modBLkNo :  mod No =>  block No
	_BLkmodlist .resize(blockNo); // block No => mod No
	_BLklinklist.resize(blockNo); // block No => link No
	_runUnitBLklist.resize(blockNo); // merge block No => block No
	vector<int> b2mb(blockNo);

	
	//_runUnitBLklist.resize(blockNo); // block No => merge block No  
/*
//	for(set<int>::iterator j =_splitNode.begin();j!=_splitNode.end();j++){
//		cerr << *j << endl;
//	}
*/

	// set block No (mod&link) 
/*	for(int i=0;i<_edge.size();i++){
		//cerr << i << " " << _modBLkNo[i] << endl;
		if( _splitNode.find(_edge[i].toID) != _splitNode.end()){
			_splitEdge.push_back(_edge[i]);
			continue;
		}
		if(_modBLkNo[_edge[i].toID]!=_modBLkNo[_edge[i].frID]){
			cerr << "no match id  "<< _edge[i].toID << " " << _edge[i].frID ;
			cerr << " " << _modBLkNo[_edge[i].toID] << " " << _modBLkNo[_edge[i].frID] << endl;
		}
		else{
			_BLklinklist[_modBLkNo[_edge[i].toID]].push_back(i);
		}
	}
*/

	for(size_t i=0;i<_node;i++){
		_BLkmodlist[_modBLkNo[i]].push_back(i);
	}

	// block Node merge 
	_runBlkCnt=0;
	int nowsize = 0;
	for(int i=0;i<blockNo;i++){
		if ( _BLkmodlist[i].size()==0 ) { continue;}
		if (nowsize+_BLkmodlist[i].size() > _blockLimit){
			nowsize = 0;
			_runBlkCnt++;
		}
		_runUnitBLklist[_runBlkCnt].push_back(i);
		b2mb[i]=_runBlkCnt;
/*
		for(int j=0;j< _BLkmodlist[i].size();j++){
			_runUnitBLklist[_runBlkCnt].push_back(_BLkmodlist[i][j]);
		}
*/
		nowsize += _BLkmodlist[i].size();
	}
	_runBlkCnt++;

	//DEBUG
/*	for(int i=0;i<_runBlkCnt;i++){
		cerr << i << " :" ;
		for( int j=0 ;j<_runUnitBLklist[i].size() ;j++){
			cerr << " " << _runUnitBLklist[i][j] << "(" <<  getModBlkSize(_runUnitBLklist[i][j]) << ")";
		}
		cerr << endl;
	}
	*/


	// set block No (mod&link) 
	for(size_t i=0;i<_edge.size();i++){
		// runbluckされてたのはsplit
		//int to_runBLKID = ;
		//int fr_runBLKID = ;
		//	cerr << "edge id  "<< _edge[i].toID << " " << _edge[i].frID ;
		//	cerr << " " << _modBLkNo[_edge[i].toID] << " " << _modBLkNo[_edge[i].frID] ;
		//	cerr << " " << b2mb[_modBLkNo[_edge[i].toID]] << " " << b2mb[_modBLkNo[_edge[i].frID]] << endl;


		if ( b2mb[_modBLkNo[_edge[i].toID]] != b2mb[_modBLkNo[_edge[i].frID]]){
			_splitEdge.push_back(_edge[i]);
			//cerr << "no match id  "<< _edge[i].toID << " " << _edge[i].frID ;
			//cerr << " " << _modBLkNo[_edge[i].toID] << " " << _modBLkNo[_edge[i].frID] << endl;
		}
		else{//どっちに入れてもOKなはず
			_BLklinklist[_modBLkNo[_edge[i].toID]].push_back(i);
		}

		//cerr << i << " " << _modBLkNo[i] << endl;
		/*
		if( _splitNode.find() != _splitNode.end()){
			_splitEdge.push_back(_edge[i]);
			continue;
		}*/
/*
		if(_modBLkNo[_edge[i].toID]!=_modBLkNo[_edge[i].frID]){
			cerr << "no match id  "<< _edge[i].toID << " " << _edge[i].frID ;
			cerr << " " << _modBLkNo[_edge[i].toID] << " " << _modBLkNo[_edge[i].frID] << endl;
		}
		else{
			_BLklinklist[_modBLkNo[_edge[i].toID]].push_back(i);
		}
*/
	}

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
