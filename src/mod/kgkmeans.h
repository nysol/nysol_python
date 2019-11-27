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

#include <string>
#include <vector>
#include <set>
#include <kgConfig.h>
#include <kgEnv.h>
//#include <kgmodincludesort.h>
#include <kgmod.h>
#include <kgError.h>
#include <kgArgs.h>
#include <kgArgFld.h>
#include <kgCSV.h>
#include <kgCSVout.h>
using namespace kglib;

namespace kgmod { ////////////////////////////////////////////// start namespace

// 入力データ情報
class DataInfo {

  size_t  _rec;   // データ行数
  size_t  _fCnt;  // 項目数
  vector<double> _maxNum; // 最大値
  vector<double> _minNum; // 最小値
  vector<double> _rngNum; // 最大値-最小値
  vector<double> _sumNum; // 合計値 
  vector<int>    _cntNum; // 件数(nullを省く) 
  vector<double> _avgNum; // 平均値


	void init(size_t size){
		_rec=0;
		_fCnt = size ;
		_maxNum.clear();_maxNum.resize(size,-DBL_MAX); 
  	_minNum.clear();_minNum.resize(size,DBL_MAX); 
  	_rngNum.clear();_rngNum.resize(size,0); 
  	_sumNum.clear();_sumNum.resize(size,0); 
  	_cntNum.clear();_cntNum.resize(size,0); 
  	_avgNum.clear();_avgNum.resize(size,0); 
	}
	
	void vSet(size_t i,double v) {
		_cntNum[i]++;
		_sumNum[i]+=v;
		if ( _maxNum[i] < v ) { _maxNum[i] = v;  } 
		if ( _minNum[i] > v ) { _minNum[i] = v;  } 
	}

	// 全てNULLはNG
	bool valid(){

		for(size_t i=0;i<_cntNum.size();i++){
			if(_cntNum[i]==0) return false;
		}
		return true;
	}

	// range , avg
	void calc(void) { 

		for(size_t i=0;i<_avgNum.size();i++){
			_rngNum[i] = _maxNum[i] - _minNum[i];
			_avgNum[i] = _sumNum[i] / _cntNum[i];
		}

	}



  public:
		DataInfo():_rec(0){}
		

	size_t getDatInfo( kgCSVfld& icsv,kgArgFld& fField,kgCSVout* csvout=NULL ){

		init(fField.size());
	
		while( icsv.read()!=EOF ){

			for(size_t i=0 ; i<_fCnt ; i++){ 

				char *str = icsv.getVal(fField.num(i));
				if(*str!='\0'){ vSet(i,atof(str)); }

			}
			if(csvout){
				csvout->writeFld(icsv.fldSize(),icsv.getFld());
			}
			_rec++;
		}

		if(!valid()){ throw kgError("value not found on some fields"); }

		calc();

		return _rec;
		
	}
	
	
	size_t Rec(void){ return _rec;}
	double Avg(size_t i){return _avgNum[i];}
	double Rng(size_t i){return _rngNum[i];}
	double Min(size_t i){return _minNum[i];}


	// debug
	void print(){
		cerr << "rec " << _rec << endl;
		for (size_t i=0;i<_minNum.size();i++){
			cerr << " min:" << _minNum[i]; 
			cerr << " max:" << _maxNum[i]; 
			cerr << " rng:" << _rngNum[i];
			cerr << " sum:" << _sumNum[i];
			cerr << " cnt:" << _cntNum[i];
			cerr << " avg:" << _avgNum[i];
			cerr << endl;
		}	
	}
};

class Sample {
	vector < vector<double> > _SmpRec;
  size_t _recCnt;
  size_t _fCnt;
  
	public:

	void setSize(size_t reccnt,size_t fcnt ){
		_SmpRec.resize(reccnt);
		for(size_t i=0;i<reccnt;i++){
			_SmpRec[i].resize(fcnt);
		}
		_recCnt = reccnt;
		_fCnt = fcnt;
	}

	void   vSet(int i,int j,double v){ _SmpRec[i][j] = v;}
	double vGet(int i,int j){ return _SmpRec[i][j]; }

	int Cnt(){ return _recCnt; }
	int fCnt(){ return _fCnt; }

};

class CalNum {
  double _accum;
  size_t _cnt;
  public:
  void clear(){
  	_cnt=0;
  	_accum=0;
  }
  void add(double val){
	  _accum += val;
  	_cnt ++;
  }

	CalNum& operator+=(const double val)
	{
	  _accum += val;
  	_cnt ++;
		return *this;
	}


  size_t Cnt(void){ return _cnt;}
  double Avg(void){ return _accum/_cnt;}
  double Accm(void){ return _accum;}

};

class Cluster {

  size_t _rcnt;           // クラスタに属するレコード数
  vector<CalNum> _calNum; // 重心の計算用
  vector<kgVal>  _cenNum; // 重心

	public:

	void setSize(size_t fcnt){
		_calNum.resize(fcnt);
		_cenNum.resize(fcnt,kgVal('N'));
	}

	void   gSet(int i ,double v){ _cenNum[i].r(v);}
	double gGet(int i ){ return _cenNum[i].r();}


	void resetCnt(){ _rcnt=0;}
	void incCnt(){ _rcnt++;}

	void resetCalnum(){
		for(size_t i=0 ; i<_calNum.size();i++){
			_calNum[i].clear();
		}
	}
	
	void acum(int i ,double v){ _calNum[i] += v; }

  size_t Cnt (size_t i){ return _calNum[i].Cnt();} 
	double Avg (size_t i){ return _calNum[i].Avg();} 
	double Accm(size_t i){ return _calNum[i].Accm();}

	int getCnt(){ return _rcnt;}
	bool recEmpty(){ return (_rcnt==0);}

	double calDist(Sample& sample,size_t rno ,DataInfo * dinfo);


};

class ClustersBASE{
	protected:
	vector < Cluster > _cluster;
	size_t _fCnt;
	size_t _cCnt;

	int movCenter();

	void wrkReset(){
		for(int i=0; i<_cluster.size();i++){
	    _cluster[i].resetCnt();
  	  _cluster[i].resetCalnum();
		}
	}

	void setSmp2Cluster(int k,int s,Sample &sample)
	{
	  for(size_t i=0; i<_fCnt; i++){
		  _cluster[k].gSet(i,sample.vGet(s,i));
  	}
	}


	public:


	void cResize(int clsCnt,int fCnt){
		_cluster.clear();
		_cluster.resize(clsCnt);
		for(int i=0;i<clsCnt;i++){
	  	_cluster[i].setSize(fCnt);
	  }
	  _fCnt = fCnt;
	  _cCnt = clsCnt;
	  
	}

	bool recEmpty(size_t i) { return _cluster[i].recEmpty(); }


	void initRA_MAIN(
		Sample& fr,
		boost::variate_generator< boost::mt19937,boost::uniform_int<>  > * rmod
	);

	double gGet(int k,int i){ return _cluster[k].gGet(i);}
	void   gSet(int k,int i,double v){ _cluster[k].gSet(i,v); }

	double ttlAcum(){
		double ttl=0;
    for(size_t i=0; i< _cCnt; i++){
      for(size_t j=0; j<_fCnt; j++){
        ttl += _cluster[i].Accm(j);
      }
    }
    return ttl;
  }
	void show(){
		int i,j;

		printf("----------------- showCluster\n");
		for(i=0; i<_cluster.size(); i++){
			printf("c[%d] cnt=%d : ",i,_cluster[i].getCnt());
			for(j=0; j<_fCnt; j++){
				printf("%g",_cluster[i].gGet(j));
				printf("(");
				printf("%g",_cluster[i].Accm(j));
				printf(",%lu)",_cluster[i].Cnt(j));
				printf(")");
			}
			printf("\n");
		}
	}

};


class ClustersTmp4BFA;

class Clusters : public ClustersBASE {

	kgArgFld *_ffld;
	kgCSVfld *_icsv;
	DataInfo *_dinfo;


	void csv2sample(
		vector<Sample> & samp,int recCnt, 
		boost::variate_generator< boost::mt19937,boost::uniform_int<>  > * rmod 
	); 

	// using initKA
	int getCenInstance(Sample &sample);
	double get_dji(Sample& sample, int m, int n);
	double get_Dj (Sample& sample, int k, int j);

	public:

	void clscopy( ClustersTmp4BFA  &cls);

	size_t initKA(
		kgCSVfld *csv, kgArgFld *fld ,DataInfo *dinfo, size_t cCnt , 
		unsigned long seed, size_t cnt, size_t rcnt);

	size_t initRA( 
		kgCSVfld *csv, kgArgFld *fld ,DataInfo *dinfo , size_t cCnt , 
		unsigned long seed, size_t cnt, size_t rcnt);

	size_t initBFbyRA(
		kgCSVfld *csv, kgArgFld *fld ,DataInfo *dinfo , size_t cCnt , 
		unsigned long seed, size_t cnt, size_t rcnt);


	int  nearestCid(bool null=false);
	void convergent();

};

class ClustersTmp4BFA : public ClustersBASE{

	DataInfo *_dinfo;

	friend void Clusters::clscopy( ClustersTmp4BFA &cls);

	public:
	ClustersTmp4BFA(DataInfo *dinfo):_dinfo(dinfo){};
	
	void farthest(int k,Sample & sample,  DataInfo *dinfo);
	void convergent(Sample& sample,DataInfo *dinfo);
	int  nearestCid(Sample& sample,int rpos,DataInfo *dinfo);

};


class kgKmeans : public kgMod {

	// 引数
	kgCSVfld _iFile;  // i=
	kgCSVout _oFile;  // o=
	kgArgFld _fField; // f=

	int _cCnt; // n=
	int		_dType;	//d=
	int		_mcnt;	// m=
	unsigned long _seed; // S=
	kgstr_t		_addstr;	 //a=
	bool _null; // -null
	
	Clusters _clusters;

	// 引数セット
	void setArgs(void);
	void setArgs(int inum,int *i,int onum, int* o);

	DataInfo _dinfo;

	int runMain(void);
	void setArgsMain(void);	


  public:

	static const char * _ipara[];
	static const char * _opara[];


  // コンストラクタ
	kgKmeans(void);
	~kgKmeans(void){}

	int run(void);
	int run(int inum,int *i_p,int onum, int* o_p ,string & str);

	// コマンド固有の公開メソッド
	size_t iRecNo(void) const { return _iFile.recNo();}
	size_t oRecNo(void) const { return _oFile.recNo();}


	void runErrEnd(void){
		_iFile.close();
		_oFile.forceclose();
		
	}

};

}