#!/usr/bin/env python
# -*- coding: utf-8 -*-

import os
import sys
import glob


import nysol.mcmd as nm
import nysol.shell as ns
os.system("rm -f result.csv")
os.system("rm -f xxx6.html")

iFiles=["testdata/individual/2000*.csv"]

scp=ns.script()
#scp.add(nm.readcsv(iFiles)) # => mcatに変換?
scp.add(nm.mcat(i=iFiles)) # => mcatに変換?
scp.add(nm.mcut(f="eCode,date,open:o,high:h,low:l,close:c,volume,mv")) # => 入出力が省かれているので前後に接続
scp.add(nm.mdelnull(f="*"))
scp.add(nm.mjoin(k="date",m="testdata/index.csv", f="open:to,high:th,low:tl,close:tc"),o="xxbase") # =>出力データを"xxbase"とネーミング(ファイル名ではない)
scp.add(nm.mcut(f="date,eCode,mv"),                     i="xxbase") # 上の出力xxbaseを入力とするように接続(mtee?) 
scp.add(nm.mnumber(k="date", s="mv%nr", S=1, a="rank"), o="xxmvR")
# ランクの最大値を求める
scp.add(nm.mstats(k="date", f="rank:total", c="max"),   i="xxmvR",o="xxmvMaxR")
# 上二つを結合して、相対ランク(0.0-1.0)を求める
scp.add(nm.mjoin(k="date", f="total"),                  i="xxmvR",m="xxmvMaxR")
scp.add(nm.mcal(c="${rank}/${total}", a="mvRelRank"))
scp.add(nm.mcut(f="date,eCode,mv,mvRelRank"))
scp.add(nm.mslide(k="eCode", s="date", f="date:date2"), o="xxmv")
###############
# 同じく日別return相対rankを求める (入力:xxbase, 出力xxret)
scp.add(nm.mcut(f="eCode,date,c"),                          i="xxbase")
scp.add(nm.mslide(k="eCode", s="date", f="date:date2,c:c2"))
scp.add(nm.mcal(c="${c2}/${c}-1.0", a="return"))
scp.add(nm.mcut(f="eCode,date2:date,return"))
scp.add(nm.mnumber(k="date", s="return%nr", S=1, a="rank"), o="xxretR")
# ランクの最大値を求める
scp.add(nm.mstats(k="date", f="rank:total", c="max"),       i="xxretR",o="xxretMaxR")
# 上二つを結合して、相対ランク(0.0-1.0)を求める
scp.add(nm.mjoin(k="date", f="total"),                      i="xxretR",m="xxretMaxR")
scp.add(nm.mcal(c="${rank}/${total}", a="retRelRank"))
scp.add(nm.mcut(f="date,eCode,retRelRank"))
scp.add(nm.mslide(k="eCode", s="date", f="date:date2"),      o="xxret")

# 各種変数の計算
scp.add(nm.mslide(k="eCode", s="date", f="date:date2,o:o2,h:h2,l:l2,c:c2,volume:volume2,to:to2,tc:tc2,th:th2,tl:tl2"), i="xxbase")
scp.add(nm.mcal(c='${c2}/${c}-1.0', a="return"))
scp.add(nm.mcal(c='${c2}/${c}-${tc2}/${tc}', a="aReturn"))
scp.add(nm.mcal(c='${tc2}/${tc}-1.0', a="tReturn"))
scp.add(nm.mcal(c='${o2}/${c}-1.0', a="kerb"))
scp.add(nm.mcal(c='${o2}/${c}-${to2}/${tc}', a="aKerb"))
scp.add(nm.mcal(c='${c2}/${o2}-1.0', a="barb"))
scp.add(nm.mcal(c='${c2}/${o2}-${tc2}/${to2}', a="aBarb"),o="xxa") #=>こんな風に入出力を明示することも可能
scp.add(nm.mcal(c='${kerb}-${barb}', a="kMb"),            i="xxa") # トポロジー解析後、裏では単にパイプ接続になる。
scp.add(nm.mcal(c='${aKerb}-${aBarb}', a="akMb"))
scp.add(nm.mcal(c='(${h2}-${l2})/${o2}', a="range"))
scp.add(nm.mcal(c='(${h2}-${l2})/${o2}-(${th2}-${tl2})/${to2}', a="aRange"))
scp.add(nm.mcal(c='(min(${c2},${o2})-${l2})/(${h2}-${l2})', a="shadowL"))
scp.add(nm.mcal(c='(${h2}-max(${c2},${o2}))/(${h2}-${l2})', a="shadowH"))
scp.add(nm.mcal(c='${shadowL}*${shadowH}', a="shadowLH",tag="checkpoint"))
scp.add(nm.mcut(f="eCode,date2:date,o2:open,h2:high,l2:low,c2:close,volume2:volume,return,aReturn,kerb,aKerb,barb,aBarb,kMb,akMb,range,aRange,shadowL,shadowH,shadowLH,to2:tOpen,th2:tHigh,tl2:tLow,tc2:tClose,tReturn"))

scp.add(nm.mjoin(k="eCode,date", K="eCode,date"  , f="mv,mvRelRank"               , assert_diffSize=True) , m="xxmv")
scp.add(nm.mjoin(k="eCode,date", K="eCode,date2" , f="mv:mvY,mvRelRank:mvRelRankY", assert_diffSize=True) , m="xxmv")
scp.add(nm.mjoin(k="eCode,date", K="eCode,date" , f="retRelRank"                 , assert_diffSize=True) , m="xxret")
scp.add(nm.mjoin(k="eCode,date", K="eCode,date2", f="retRelRank:retRelRankY"     , assert_diffSize=True), m="xxret", o="xxresult")
scp.add(nm.writecsv("./result.csv"), i="xxresult")
scp.drawModelD3("xxx6.html")
os.system("open xxx6.html")

scp.run()

#print(scp.modelInfo()["linklist"])

os.system("more result.csv")



