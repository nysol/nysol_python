
#include <iostream>

#include <string>
#include <vector>
#include <queue>
#include <boost/thread.hpp>
#include <boost/thread/condition.hpp>
#include <boost/bind.hpp>
#include <csignal>
#include <fcntl.h>
#include <sys/stat.h>


#include <kgfifo.h>
#include <kgError.h>
#include <kgMethod.h>
#include <kgMessage.h>
#include <kgArgs.h>
#include <kgConfig.h>
#include <kgTempfile.h>

using namespace std;
using namespace boost;
using namespace kglib;
using namespace kgmod;

class Queue {
private:
	boost::mutex mutex_;
	condition rThreadState_;
	condition wThreadState_;

	int iFD_;
	vector<int> oFD_;
	vector<bool> oEnd_;
	int oActive_;

	bool readEnd_;

	// blkSize*queSizeがtotalサイズ
	size_t blkSize_;
	size_t queSize_;
	kgAutoPtr2<char> buf_ap_;
	char* buf_;
	vector<char*> que_;
	size_t enq_;
	size_t deq_;
	vector<size_t> readSize_;
	
	// バッファFULL時、使用
	kgTempfile      tempFile_; 
	queue<string> tmpname_;
	queue<size_t> rest_no_;

	
	size_t nextQue(size_t n){
		n++;
		return n % queSize_;
	}

	// 全データ出力
	//ロックしてから動かすこと
	void extend_fileout(){
		//全データを吐き出す
		string tname=tempFile_.create(false, "merge");    	
		int fd=::open(tname.c_str(), O_WRONLY | O_TRUNC | O_CREAT | O_APPEND, S_IRWXU);
		size_t deq_tmp = deq_;
		while(1){
			deq_tmp =nextQue(deq_tmp);
			size_t maxSize=readSize_.at(deq_tmp);
  	  char* buf=que_.at(deq_tmp);
 			size_t wsize = ::write(fd, buf , maxSize);
    	if( wsize!=maxSize ){
				ostringstream ss;
				ss << "file write error";
				throw kgError(ss.str());
			}
			if(deq_tmp==enq_){
				break;
			}
		}
		::close(fd);
		tmpname_.push(tname);
		rest_no_.push(deq_);
		enq_ = nextQue(deq_);
	}


public:
	Queue(int iFD,vector<int> oFD, size_t queSize, size_t blkSize,kgEnv* env){
		iFD_=iFD;
		for(size_t i=0; i<oFD.size(); i++){
			oFD_.push_back(oFD.at(i));
		}
		oActive_=0;
		for(size_t i=0; i<oFD_.size(); i++){
			oEnd_.push_back(false);
			oActive_++;
		}
		queSize_=queSize;
		blkSize_=blkSize;
		enq_=0; // エンキュー
		deq_=0; // デキュー
		readEnd_=false;
    readSize_.resize(queSize_,0);
		// que全体のbufferを確保
		buf_ap_.set( new char[queSize_*blkSize_] );
		char* buf_ = buf_ap_.get();
		// 読み込み単位block別にポインター登録
		for(size_t i=0; i<queSize_; i++){
			que_.push_back(buf_+blkSize_*i);
		}
		// 一時ファイルの初期化
		tempFile_.init(env);
	}
	
	// ===================================================
	// データの読み込み
	// ===================================================
	int readData() try {
		// mutexインスタンスにロックをかける
		{
			boost::mutex::scoped_lock lock(mutex_);

			// enqがdeqに追いついたら拡張
			size_t next=nextQue(enq_);
			if(next==deq_){
				extend_fileout();
			}
			else{// enqを一つ進める
				enq_=next;
			}
		}

		// readしてenq位置に格納
		size_t maxSize=blkSize_;
		size_t accSize=0;
		size_t resSize=maxSize;
    char* buf=que_.at(enq_);
		while(accSize<maxSize){
    	int rsize = ::read(iFD_, buf + accSize, resSize<blkSize_ ? resSize : blkSize_);
    	if( rsize < 0 ){
				wThreadState_.notify_one();
      	throw kgError();
    	}
    	if( rsize == 0 ) break;

			accSize+=static_cast<size_t>(rsize);
			resSize-=static_cast<size_t>(rsize);
		}
		readSize_.at(enq_)=accSize;		
		// read eof
    if( accSize == 0 ){
			{
				boost::mutex::scoped_lock lock(mutex_);
				readEnd_=true;
			}
			wThreadState_.notify_one();
			return 0;
		}

		// write側が待ち状態にあれば起こす
		wThreadState_.notify_one();
		return 1;
	}catch(...){
		return 1;
	}

	// ===================================================
	// データの書き込み
	// ===================================================
	int writeData() try {
		// mutexインスタンスにロックをかける
		bool fout=false;
		size_t deq_tmp=0;
		{
			boost::mutex::scoped_lock lock(mutex_);
			if(tmpname_.size() ){
				fout = true	;
				deq_tmp = rest_no_.front();
			}
			// deqがenqに追いついたら休眠(buffer full)
			if(tmpname_.size()==0 && deq_==enq_){
				if(readEnd_){
					rThreadState_.notify_one();
					return 0;
				}else{
					wThreadState_.wait(mutex_);
				}
			}
		}
		//ファイル出力が存在すればそっちから先に処理する
		if(fout && deq_tmp != deq_){
			kgAutoPtr2<char> buf_tmp_ap_; 
			buf_tmp_ap_.set( new char[blkSize_] );
			char* buf = buf_tmp_ap_.get();
			int tFD;
			{
				boost::mutex::scoped_lock lock(mutex_);
				tFD = ::open(tmpname_.front().c_str(), O_RDONLY);
				tmpname_.pop();
				rest_no_.pop();
			}
			while(1){
	    	int rsize = ::read(tFD, buf, blkSize_);
	    	if(rsize < 0){
					ostringstream ss;
					ss << "file read error";
					throw kgError(ss.str());
				}
				if(rsize==0) { break;} 

	 	   for(size_t i=0; i<oFD_.size(); i++){
					if(oEnd_.at(i)) continue;
					size_t maxSize=rsize;
					size_t accSize=0;
					size_t resSize=maxSize;
					// deq位置のデータをwrite
 					while(accSize<maxSize){
  	  			int wsize = ::write(oFD_.at(i), buf + accSize, resSize<maxSize ? resSize : maxSize);
  	  			if( wsize<0 ){
							rThreadState_.notify_one();
							if(32==errno){ // pipe broken
								oEnd_.at(i)=true; // 正常終了
								oActive_--;
								if(oActive_==0){
									return 0;
								}
							}else{
								ostringstream ss;
								ss << "file write error";
								throw kgError(ss.str());
							}
						}
   		 			if( wsize == 0 ){
							oEnd_.at(i)=true; // 正常終了
							oActive_--;
							if(oActive_==0){
								return 0;
							}
							break;
						}
						accSize+=static_cast<size_t>(wsize);
						resSize-=static_cast<size_t>(wsize);
					}
				}
			}
			::close(tFD);
		}
		else{
			// deq位置のデータをwrite
			for(size_t i=0; i<oFD_.size(); i++){
				if(oEnd_.at(i)){ continue; }
				size_t maxSize=readSize_.at(deq_);
				size_t accSize=0;
				size_t resSize=maxSize;
				char* buf=que_.at(deq_);
				while(accSize<maxSize){
					int wsize = ::write(oFD_.at(i), buf + accSize, resSize<maxSize ? resSize : maxSize);
					if( wsize<0 ){
						rThreadState_.notify_one();
						if(32==errno){ // pipe broken
							oEnd_.at(i)=true; // 正常終了
							oActive_--;
							if(oActive_==0){ return 0; }
						}else{
							ostringstream ss;
							ss << "file write error";
							throw kgError(ss.str());
						}
					}
					if( wsize == 0 ){
						oEnd_.at(i)=true; // 正常終了
						oActive_--;
						if(oActive_==0){ return 0; }
						break;
					}
					accSize+=static_cast<size_t>(wsize);
					resSize-=static_cast<size_t>(wsize);
				}
			}
			{
				boost::mutex::scoped_lock lock(mutex_);
				// deqを一つ進める
				if(deq_==enq_){
					if(readEnd_){
						return 0;
					}else{
						wThreadState_.wait(mutex_);
					}
				}
				deq_=nextQue(deq_);
			}
		}
		// read側が待ち状態にあれば起こす
		rThreadState_.notify_one();
		return 1;
	}catch(...){
		return 1;
	}
};

void rThread(Queue *hd) try {
	while(hd->readData()==1){}
}catch(...){
}

void wThread(Queue *hd) try{
	while(hd->writeData()==1){}
}catch(...){
}

// -----------------------------------------------------------------------------
// コンストラクタ(モジュール名，バージョン登録,パラメータ)
// -----------------------------------------------------------------------------

kgFifo::kgFifo(void){
	#ifdef ENG_FORMAT
		#include <help/en/kgfifoHelp.h>
	#else
		#include <help/jp/kgfifoHelp.h>
	#endif

	_name    = "kgfifo";
	_version = "1.0";
}

// -----------------------------------------------------------------------------
// 入出力ファイルオープン
// -----------------------------------------------------------------------------
void kgFifo::setArgs(void){
	// unknown parameter check
	_args.paramcheck("i=,o=,n=,b=");

	// 出力ファイル名の取得
	vector<kgstr_t> vs_o = _args.toStringVector("o=",false);

	// 入力ファイルオープン
	iName_ = _args.toString("i=",false);

	if(iName_.size()==0){
		iFD_=0;
	}else{
		iFD_ = ::open(iName_.c_str(), O_RDONLY);
		if(iFD_ == -1 ){
			ostringstream ss;
			ss << "file read open error: " << iName_;
			throw kgError(ss.str());
		}
	}
	if(vs_o.size()==0){
		oFD_.push_back(1);
	}else{
		for(size_t i=0; i<vs_o.size(); i++){
			int fd=::open(vs_o.at(i).c_str(), O_WRONLY | O_TRUNC | O_CREAT | O_APPEND, S_IRWXU);
			if(fd == -1 ){
				ostringstream ss;
				ss << "file write open error: " << vs_o.at(i);
				throw kgError(ss.str());
			}
			oFD_.push_back(fd);
		}
	}
	string queSize = _args.toString("n=",false);
	if(queSize.length()==0){
		queSize_=100000;
	}else{
		queSize_= atoi(queSize.c_str());
	}
	if(queSize_<=2){
		throw kgError("n= must take greater than 2.");
	}

}


// -----------------------------------------------------------------------------
// 入出力ファイルオープン
// -----------------------------------------------------------------------------
void kgFifo::setArgs(int i_p,int o_p){
	// unknown parameter check
	_args.paramcheck("i=,o=,n=,b=");

	// 出力ファイル名の取得
	vector<kgstr_t> vs_o = _args.toStringVector("o=",false);

	if(i_p>0){
		iFD_=i_p;
	}
	else{
		// 入力ファイルオープン
		iName_ = _args.toString("i=",false);

		if(iName_.size()==0){
			iFD_=0;
		}else{
			iFD_ = ::open(iName_.c_str(), O_RDONLY);
			if(iFD_ == -1 ){
				ostringstream ss;
				ss << "file read open error: " << iName_;
				throw kgError(ss.str());
			}
		}
	}
	if(o_p>0){
		oFD_.push_back(o_p);
	}
	else{
		if(vs_o.size()==0){
			oFD_.push_back(1);
		}else{
			for(size_t i=0; i<vs_o.size(); i++){
				int fd=::open(vs_o.at(i).c_str(), O_WRONLY | O_TRUNC | O_CREAT | O_APPEND, S_IRWXU);
				if(fd == -1 ){
					ostringstream ss;
					ss << "file write open error: " << vs_o.at(i);
					throw kgError(ss.str());
				}
				oFD_.push_back(fd);
			}
		}
	}
	string queSize = _args.toString("n=",false);
	if(queSize.length()==0){
		queSize_=100000;
	}else{
		queSize_= atoi(queSize.c_str());
	}
	if(queSize_<=2){
		throw kgError("n= must take greater than 2.");
	}

}

void kgFifo::oClose(void) throw(kgError) {
	//flush();
	try {
		for(size_t i=0; i<oFD_.size(); i++){
			::close(oFD_.at(i));
		}
	}catch(kgError& err){
		ostringstream ss;
		ss << "file write close error";
		throw kgError(ss.str());
	}
}

void kgFifo::iClose(void) throw(kgError) {
	try {
		::close(iFD_);
	}catch(kgError& err){
		ostringstream ss;
		ss << "file read close error: " << iName_;
		throw kgError(ss.str());
	}
}

// -----------------------------------------------------------------------------
// 実行
// -----------------------------------------------------------------------------
int kgFifo::run(void) try {

	setArgs();
	Queue que(iFD_,oFD_,queSize_,4096,_env);
	thread rThr(bind(&rThread, &que));
	thread wThr(bind(&wThread, &que));
	rThr.join();
	wThr.join();
	iClose();
	oClose();
	// 終了処理(メッセージ出力,thread pipe終了通知)
	successEnd();

// 例外catcher
}catch(kgError& err){
	iClose();
	oClose();
	errorEnd(err);
}catch(...){
	iClose();
	oClose();
	errorEnd();
}



// -----------------------------------------------------------------------------
// 実行
// -----------------------------------------------------------------------------
int kgFifo::run(int i_p,int o_p) try {

	setArgs( i_p,o_p);
	Queue que(iFD_,oFD_,queSize_,4096,_env);
	thread rThr(bind(&rThread, &que));
	thread wThr(bind(&wThread, &que));
	rThr.join();
	wThr.join();
	iClose();
	oClose();
	// 終了処理(メッセージ出力,thread pipe終了通知)
	successEnd();

// 例外catcher
}catch(kgError& err){
	iClose();
	oClose();
	errorEnd(err);
}catch(...){
	iClose();
	oClose();
	errorEnd();
}
