#!/usr/bin/env python
# -*- coding: utf-8 -*-
"""
	==================================
	Takeライブラリのコアアルゴリズム
	==================================
	C言語で開発されたTakeライブラリのコアアルゴリズムをpythonから利用できるようにラッピングしたものである。
	オリジナルのコードは http://research.nii.ac.jp/~uno/codes-j.htm より入手できる。

	:py:func:`lcm` は・・・

	:ref:`lcmへ<lcm-label>`

	:py:func:`take.extcore.lcm` は・・・


	* lcm
	* lcmseq


	xxx title
	-------------------

	extcoreパッケージが使うデータ形式を書く。
	トランザクション形式
	グラフエッジ形式
	グラフ隣接行列形式

	最終更新日: |today|

"""
from nysol.take import _grhfillib as lib_grhfil
from nysol.take import _lcmlib as lib_lcm
from nysol.take import _lcmseq_zerolib as lib_lcmseq0
from nysol.take import _lcmseqlib  as lib_lcmseq
from nysol.take import _lcmtranslib  as lib_lcmtarns
from nysol.take import _mmaketralib  as lib_mmaketra

from nysol.take import _macelib as lib_mace
from nysol.take import _medsetlib as lib_medset
from nysol.take import _simsetlib as lib_simset
from nysol.take import _sspclib as lib_sspc

def _kwd2list(args, kw_args,paraConf,extpara=None):
	def tostr(val):
		if isinstance(val,list):
			newlist=[]
			for cval in val:
				newlist.append(str(cval))
			if len(newlist) != 0 :
				return ",".join(newlist)
			else:
				return None
		else:
			return str(val)

	if len(args)>1:
		raise Exception("args zero or one ")

	if len(args)==1 and not isinstance(args[0],dict) :
		raise Exception("args is not dict")

	# dict Merge		
	if len(args)==1:
		kw_args.update(args[0])

	# 可能パラメータ設定
	enablePara = set([])
	for v in paraConf.values():
		for vv in v:
			enablePara.add(vv[0])

	# 可能パラメータCHECK
	for k in kw_args.keys():
		if not k in enablePara:
			raise Exception("Unknown parameter")

	newargs=[]
	if "saki" in paraConf:
		for paraInfo in paraConf["saki"]:
			if paraInfo[0] in kw_args:
				newargs.append(tostr(kw_args[paraInfo[0]]))
			elif paraInfo[1] == 1:
				raise Exception("not find necceary parameter")
				
	if "opt" in paraConf:
		for paraInfo in paraConf["opt"]:
			if paraInfo[0] in kw_args:
				newargs.append(tostr(paraInfo[1]))
				newargs.append(tostr(kw_args[paraInfo[0]]))

	if "opts" in paraConf:
		for paraInfo in paraConf["opts"]:

			if paraInfo[0] in kw_args:

				newargs.append(tostr(paraInfo[1]))

				if isinstance(kw_args[paraInfo[0]],list):
					if len(kw_args[paraInfo[0]])!=paraInfo[2] :
						raise Exception("not valid parameter")
					for val in kw_args[paraInfo[0]]:
						newargs.append(tostr(val))

				elif isinstance(kw_args[paraInfo[0]],str):
					optsub = kw_args[paraInfo[0]].split(",") 
					if len(optsub)!=paraInfo[2] :
						raise Exception("not valid parameter")
					for val in optsub:
						newargs.append(tostr(val))
				
				else:
					raise Exception("not valid parameter")


	if "addkw" in paraConf:
		for paraInfo in paraConf["addkw"]:

			if paraInfo[0] in kw_args:

				if isinstance(kw_args[paraInfo[0]],list):

					if len(kw_args[paraInfo[0]])==1:
						newargs.append( tostr(paraInfo[1]) )
					elif len(kw_args[paraInfo[0]])==2:
						newargs.append( tostr(paraInfo[1])+tostr(kw_args[paraInfo[0]][1]))
					else:
						raise Exception("not valid parameter")

					newargs.append(tostr(kw_args[paraInfo[0]][0]))


				elif isinstance(kw_args[paraInfo[0]],str):
					optsub = kw_args[paraInfo[0]].split(",") 
					if len(optsub)==1:
						newargs.append(tostr(paraInfo[1]) )
					elif len(optsub)==2:
						newargs.append(tostr(paraInfo[1])+tostr(optsub[1]) )
					else:
						raise Exception("not valid parameter")

					newargs.append(tostr(optsub[0]))
				
				else:
					raise Exception("not valid parameter")

	if "optB" in paraConf:
		for paraInfo in paraConf["optB"]:
			if paraInfo[0] in kw_args:
				if kw_args[paraInfo[0]]:
					 newargs.append(tostr(paraInfo[1]))


	if "optato" in paraConf:
		for paraInfo in paraConf["optato"]:
			if paraInfo[0] in kw_args:
				newargs.append(tostr(paraInfo[1]))
				newargs.append(tostr(kw_args[paraInfo[0]]))

	if "ato" in paraConf:
		for paraInfo in paraConf["ato"]:
			if paraInfo[0] in kw_args:
				newargs.append(tostr(kw_args[paraInfo[0]]))
			elif paraInfo[1] == 1:
				raise Exception("not find necceary parameter")



	if isinstance(extpara,dict) and "ext" in paraConf:
		for paraInfo in paraConf["ext"]:
			if paraInfo[0] in kw_args:
				extpara[paraInfo[1]] = tostr(kw_args[paraInfo[0]])
	
	return newargs


def lcm (*args,**kw_args):
	"""
	.. _lcm-label:

	Linear time Closed itemset Miner
	
	:type type: 文字列
	:type i: ファイル名
	:type o: ファイル名
	:type sup: 0以上の整数
	:type K: 0以上の整数
	:type l: 0以上の整数
	:type u: 0以上の整数
	:type w: ファイル名
	:type a: float(0以上1以下)
	:type A: float(0以上1以下)
	:type r: float(0以上1以下)
	:type R: float(0以上1以下)
	:type c: ファイル名
	:type C: ファイル名
	:type item: 0以上の整数
	:type so: ファイル名
	:type separator: 文字
	:param type: 列挙するアイテム集合のタイプ(F|C|M)、及び出力形式(詳細は後述)【必須】
	:param i: トランザクションファイル名【必須】
	:param o: 出力ファイル名【必須】
	:param sup: 最小サポート
	:param U: 最大サポート
	:param K: 頻出な上位のアイテム集合のみ出力する
	:param l: アイム集合のサイズの下限値
	:param u: アイム集合のサイズの上限値
	:param w: トランザクションの重みファイル名
	:param a: 相関ルールマイニングを実施しconfidenceの下限値を与える
	:param A: 相関ルールマイニングを実施しconfidenceの上限値を与える
	:param r: 相関ルールマイニングを実施しrelational confidenceの下限値を与える
	:param R: 相関ルールマイニングを実施しrelational confidenceの上限値を与える
	:param c: read item constraint file
	:param C: read item un-constraint file
	:param item: find association rule for item [num]
	:param so: 標準出力の内容を指定のファイルに出力する	
	:param separator: 出力時のアイテムの区切り文字を指定する

	f=,F=[ratio]
		output itemsets with frequency no less/greater than [ratio] times the frequency given 
		by product of the probability of each item appearance
	p=,P=[num]
		output itemset only if (frequency)/(abusolute frequency) is no less/no greater than [num]
	n=,N=[num]
		output itemset only if its negative frequency is no less/no greater than [num] 
		(negative frequency is the sum of weights of transactions having negative weights)
	opos=,Opos=[num]
		output itemset only if its positive frequency is no less/no greater than [num] 
		(positive frequency is the sum of weights of transactions having positive weights)
	m,M=[filename]
		read/write item permutation from/to file [filename]
	stop=[num]
		stop after outputting [num] solutions
	separator=[char] 
		give the separator of the numbers in the output
	Q=[filename]
		replace the output numbers according to the permutation table given by [filename]
	# the 1st letter of input-filename cannot be '-'.
		a
	
	アイテム集合のタイプとして以下の３つから一つを選ぶ【必須】。
	
	* F:頻出アイテム集合
	* C:飽和アイテム集合
	* M:極大アイテム集合
	
	出力形式として以下の組み合わせで指定する。
	
	* f:アイテム集合の後ろに出現件数を出力する。
	* Q:アイテム集合の前に出現件数を出力する。
	* I:アイテム集合の次の行に出現集合(出現するトランザクション行番号)を出力する。
	* t:トランザクションデータベースを転置する(アイテムiはi番目のトランザクションとなり、j番目のトランザクションはアイテムjとなる)。
	* A:w=でプラスとマイナスのトランザクションの重みを指定した場合、プラス/マイナスそれぞれの件数とその比を出力する。
	* P:positive-closed itemset mining
	* R:output redundant items for rule mining (usually, redundant items are removed, to be minimal, in the case of rule mining)
	* i:do not output itemset to the output file (only rules)
	* s:output confidence and item frequency by absolute values

	例1

	0〜5のアイテムについての6行のトランザクションデータベースnumTraDB1からの実行例を以下に示す。

	.. code-block:: python
		:linenos:

		>>> import os
		>>> from nysol.take.extcore import lcm 
		>>> from nysol.take.sample import Samples
		>>> smp=Samples()
		>>> iFile=smp.numTraDB1() # カレントディレクトリにサンプルのデータファイル("numTraDB1.csv")が生成される。
		>>> smp.head(iFile,top=0) # headは指定ファイルの先頭6行を表示するメソッド(top=0で全行表示)。
		file name: ./numTraDB1.csv
		2 4
		3 4 5
		0 1 3 5
		1 3 5
		0 1 3 4
		0 1 3 4 5

		>>> lcm(type="Ff_",sup=3,i=iFile,o="rsl_Ff") # 頻出アイテム集合の列挙。type=にfを指定すると出現件数を出力、_でメッセージなし
		>>> smp.head("rsl_Ff",top=0)
		file name: rsl_Ff
		(6)
		3 (5)
		1 (4)
		1 3 (4)
		4 (4)
		4 3 (3)
		5 (4)
		5 3 (4)
		5 1 (3)
		5 1 3 (3)
		0 (3)
		0 1 (3)
		0 1 3 (3)
		0 3 (3)

		>>> lcm(type="Cf_",sup=3,i=iFile,o="rsl_Cf") # 頻出飽和アイテム集合の列挙。
		>>> smp.head("rsl_Cf",top=0)
		file name: rsl_Cf
		(6)
		3 (5)
		1 3 (4)
		4 (4)
		4 3 (3)
		5 3 (4)
		5 1 3 (3)
		0 3 1 (3)

		>>> lcm(type="Mf_",sup=3,i=iFile,o="rsl_Mf") # 頻出極大アイテム集合の列挙。
		>>> smp.head("rsl_Mf",top=0)
		file name: rsl_Mf
		4 3 (3)
		5 1 3 (3)
		0 3 1 (3)

		>>> lcm(type="Ff_",sup=3,l=3,i=iFile,o="rsl_Ff_l3") # l=3でアイテム集合のサイズが3以上に限定
		>>> smp.head("rsl_Ff",top=0)
		file name: rsl_Ff_l3
		5 1 3 (3)
		0 1 3 (3)

		>>> lcm(type="FfI_",sup=3,l=3,i=iFile,o="rsl_FfI_l3") # typeにIを加えることで、アイテム集合の下に出現集合が出力される。
		>>> smp.head("rsl_FfI_l3",top=0)
		# アイテム集合{5,1,3}はトランザクションデータの2,3,5行目に出現している(先頭行は0行目と数える}。
		file name: rsl_FfI_l3
		5 1 3 (3)
		2 3 5
		0 1 3 (3)
		2 4 5

		>>> lcm(type="FftI_",sup=3,l=3,i=iFile,o="rsl_Fft_l3") # typeにtを加えることでデータベースを転置してから実行する。
		>>> smp.head("rsl_FftI_l3",top=0)
		# トランザクション{4,2,5}はアイテム{0,1,3}に出現する。
		file name: rsl_FftI_l3
		4 2 5 (3)
		0 1 3
		3 2 5 (3)
		1 3 5

		>>> lcm(type="Ff",K=4,sup=1,i=iFile,so="rsl_Ff_K") # K=4で頻出上位4番目の出現件数を出力する。メッセージの標準出力として出力されるので、typeに"_"を入れず。so=に出力ファイル名を指定する。
		trsact: ./numTraDB1.csv ,#transactions 6 ,#items 6 ,size 21 extracted database: #transactions 6 ,#items 6 ,size 21
		separated at 0

		>>> smp.head("rsl_Ff_K",top=0) # 先に実行した例 lcm(type="Ff_",sup=3,i=iFile,o="rsl_Ff") の結果から、4番目のルールは "1 3 (4)"であり、その出現件数4が出力されている。
		file name: rsl_Ff_K
		4

		>>> wFile=smp.numTraDB1w() # 重みファイルの作成
		>>> smp.head(wFile,top=0)
		# トランザクション0,2,5行目の重みが-1で、他の行の重みが1という意味。小数で指定することも可能。
		file name: ./numTraDB1w.csv
		-1
		1
		-1
		1
		1
		-1

		>>> lcm(type="FfA_",sup=1,w=wFile,i=iFile,o="rsl_FfA_w") # w=を指定すると、トランザクションの重みで件数をカウントする。typeに"A"を加えることで、マイナス重みとプラス重みの件数情報が表示される。
		>>> smp.head("rsl_Ff_w",top=0)
		# アイテム3は、1,2,3,4,5行目に含まれ、その重み合計は1(=1-1+1+1-1)となる(プラス重み件数3件、マイナス重み件数2件、プラス率は3/5=0.6)。
		# 同様にアイテム集合{4,3}は1,4,5行目に含まれ、その重み合計は1(=1+1-1)となる(プラス重み件数2件、マイナス重み件数1件、プラス率は2/3=0.6666)。
		file name: rsl_FfA_w
		3 (1) (3,2,0.6)
		4 3 (1) (2,1,0.6666)

	"""

	paraLIST={
		"saki":[("type",1)],
		"ato":[("i",1),("sup",1),("o",0)],
		"opt":[("K","-K"),("l","-l"),("u","-u"),("U","-U"),("w","-w"),("c","-c"),("C","-C"),
					("item","-i"),("a","-a"),("A","-A"),("r","-r"),("R","-R"),("f","-f"),("F","-F"),
					("p","-p"),("P","-P"),("n","-n"),("N","-N"),("opos","-o"),("Opos","-O"),
					("m","-m"),("M","-M"),("Q","-Q"),("stop","-#"),("separator","-,")],
		"ext":[("so","so")]
	}
	extpara={}
	kwd = _kwd2list(args,kw_args,paraLIST,extpara)
	if "so" in extpara:
		return lib_lcm.lcm_run(kwd,extpara["so"])
	else:
		return lib_lcm.lcm_run(kwd)
	
def lcmseq (*args, **kw_args):
	"""
LCM algorithm for enumerating all frequently appearing sequences

keyword
type=以下の組み合わせ 必須
  %:show progress
  _:no message
  +:write solutions in append mode
  F:position occurrence, C:document occurrence
  m:output extension maximal patterns only
  c:output extension closed patterns only
  f,Q:output frequency following/preceding to each output sequence
  A:output coverages for positive/negative transactions
  I(J):output ID's of transactions including each pattern, 
       if J is given, an occurrence is written in a complete stype; 
       transaction ID, starting position and ending position
  i:do not output itemset to the output file (only rules)
  s:output confidence and item frequency by absolute values
  t:transpose the input database 
    (item i will be i-th transaction, and i-th transaction will be item i)
i=[filename]　必須
  input-filename
sup=[support]　必須
  support
o=[filename] 
  output-filename
K=[num] 
  output [num] most frequent sequences
l=,u=[num] 
  output sequences with size at least/most [num]
U=[num] 
  upper bound for support(maximum support)
g=[num]
  restrict gap length of each consequtive items by [num]
G=[num]
  restrict window size of the occurrence by [num]
w=[filename]
  read weights of transactions from the file
item=[num]
  find association rule for item [num]
a=,A=[ratio]
  find association rules of confidence at least/most [ratio]
r=,R=[ratio]
  find association rules of relational confidence at least/most [ratio]
f=,F=[ratio]
  output sequences with frequency no less/greater than [ratio] times 
  the frequency given by the product of appearance probability of each item
p=,P=[num]
  output sequence only if (frequency)/(abusolute frequency) is no less/no greater than [num]
n=,N=[num]
  output sequence only if its negative frequency is no less/no greater than [num] 
  (negative frequency is the sum of weights of transactions having negative weights)
opos=,Opos=[num]
  output sequence only if its positive frequency is no less/no greater than [num] 
  (positive frequency is the sum of weights of transactions having positive weights)
s=[num]
  output itemset rule (of the form (a,b,c) => (d,e)) with confidence at least [num] 
  (only those whose frequency of the result is no less than the support)
stop=[num]
  stop after outputting [num] solutions
separator=[char]
  give the separator of the numbers in the output
Q=[filename]
  replace the output numbers according to the permutation table given by [filename]
so=[filename]
  standard output to [filename]
# the 1st letter of input-filename cannot be '-'.
	"""
	paraLIST={
		"saki":[("type",1)],
		"ato":[("i",1),("sup",1),("o",0)],
		"opt":[("K","-K"),("l","-l"),("u","-u"),("U","-U"),("g","-g"),("G","-G"),("w","-w"),
					("item","-i"),("a","-a"),("A","-A"),("r","-r"),("R","-R"),("f","-f"),("F","-F"),
					("p","-p"),("P","-P"),("n","-n"),("N","-N"),("opos","-o"),("Opos","-O"),
					("s","-s"),("Q","-Q"),("stop","-#"),("separator","-,")],
		"ext":[("so","so")]
	}

	extpara={}
	kwd = _kwd2list(args,kw_args,paraLIST,extpara)
	if "so" in extpara:
		return lib_lcmseq.lcmseq_run(kwd,extpara["so"])
	else:
		return lib_lcmseq.lcmseq_run(kwd)

def lcmseq_zero (*args, **kw_args):
	"""
LCM algorithm for enumerating all frequently appearing sequences
0アイテムを出力しないVersion

keyword
type=以下の組み合わせ 必須
  %:show progress
  _:no message
  +:write solutions in append mode
  F:position occurrence
  C:document occurrence
  m:output extension maximal patterns only
  c:output extension closed patterns only
  f,Q:output frequency following/preceding to each output sequence
  A:output coverages for positive/negative transactions
  I(J):output ID's of transactions including each pattern, 
  　　　if J is given, an occurrence is written in a complete stype; 
  　　　transaction ID, starting position and ending position
  i:do not output itemset to the output file (only rules)
  s:output confidence and item frequency by absolute values
  t:transpose the input database 
    (item i will be i-th transaction, and i-th transaction will be item i)
i=[filename]　必須
  input-filename
sup=[support]　必須
  support
o=[filename] 
  output-filename
K=[num]
  output [num] most frequent sequences
l=,u=[num]
  output sequences with size at least/most [num]
U=[num]
  upper bound for support(maximum support)
g=[num]
  restrict gap length of each consequtive items by [num]
G=[num]
  restrict window size of the occurrence by [num]
w=[filename]
  read weights of transactions from the file
item=[num]
  find association rule for item [num]
a=,A=[ratio]
  find association rules of confidence at least/most [ratio]
r=,R=[ratio]
  find association rules of relational confidence at least/most [ratio]
f=,F=[ratio]
  output sequences with frequency no less/greater than [ratio] times 
  the frequency given by the product of appearance probability of each item
p=,P=[num]
  output sequence only if (frequency)/(abusolute frequency) is no less/no greater than [num]
n=,N=[num]
  output sequence only if its negative frequency is no less/no greater than [num] 
  (negative frequency is the sum of weights of transactions having negative weights)
opos=,Opos=[num]
  output sequence only if its positive frequency is no less/no greater than [num] 
  (positive frequency is the sum of weights of transactions having positive weights)
s=[num]
  output itemset rule (of the form (a,b,c) => (d,e)) with confidence at least [num] 
  (only those whose frequency of the result is no less than the support)
stop=[num]
  stop after outputting [num] solutions
separator=[char]
  give the separator of the numbers in the output
Q=[filename]
  replace the output numbers according to the permutation table given by [filename]
so=[filename]
  standard output to [filename]
# the 1st letter of input-filename cannot be '-'.
	"""

	paraLIST={
		"saki":[("type",1)],
		"ato":[("i",1),("sup",1),("o",0)],
		"opt":[("K","-K"),("l","-l"),("u","-u"),("U","-U"),("g","-g"),("G","-G"),("w","-w"),
					("item","-i"),("a","-a"),("A","-A"),("r","-r"),("R","-R"),("f","-f"),("F","-F"),
					("p","-p"),("P","-P"),("n","-n"),("N","-N"),("opos","-o"),("Opos","-O"),
					("s","-s"),("Q","-Q"),("stop","-#"),("separator","-,")]
	}
	kwd = _kwd2list(args,kw_args,paraLIST)
	return lib_lcmseq0.lcmseq_zero_run(kwd)

def mace(*args, **kw_args):
	"""
MAximal Clique Enumerator

keyword
type=以下の組み合わせ 必須
  %:show progress
  _:no message
  +:write solutions in append mode
  C:enumerate cliques
  M:enumerate maximal cliques
  e:edge_list format
i=[filename]　必須
  input-filename
o=[filename] 
  output-filename
l=[num]
  output cliques with size at least [num]
u=[num]
  output cliques with size at most [num]
S=[num]
  stop after outputting [num] solutions
separator=[char]
  give the separator of the numbers in the output
Q=[filename] 
  replace the output numbers according to the permutation table given by [filename]
	"""

	paraLIST={
		"saki":[("type",1)],
		"ato":[("i",1),("o",0)],
		"opt":[("l","-l"),("u","-u"),("Q","-Q"),("stop","-#"),("separator","-,")]
	}

	kwd = _kwd2list(args,kw_args,paraLIST)
	return lib_mace.mace_run(kwd)


def sspc(*args, **kw_args):
	"""
Similar Set Pair Comparison

keyword
type=以下の組み合わせ 必須
  %:show progress
  _:no message
  +:write solutions in append mode
  #:count the number of similar records for each record
  i(inclusion): find pairs [ratio] of items (weighted sum) of 
                one is included in the other (1st is included in 2nd)
  I(both-inclusion): find pairs s.t. the size (weight sum) of intersection is [ratio] of both
  S:set similarity measure to |A\cap B| / max{|A|,|B|}
  s:set similarity measure to |A\cap B| / min{|A|,|B|}
  T(intersection): find pairs having common [threshld] items
  R(resemblance): find pairs s.t. |A\capB|/|A\cupB| >= [threshld]
  P(PMI): set similarity measure to log (|A\capB|*|all| / (|A|*|B|))
          where |all| is the number of all items
  C(cosign distance): find pairs s.t. inner product of their normalized vectors >= [threshld]
  f,Q:output ratio/size of pairs following/preceding to the pairs
  D:the first entry is ID, and unify the records with the same ID
  N:normalize the ID of latter sets, in -c mode
  n:do not consider a and b in the set when comparing a and b
  Y(y):output elements of each set that contribute to no similarity (y:fast with much memory use)
  1:remove duplicated items in each transaction
  t:transpose the database so that i-th transaction will be item i
  E:input column-row representation
  w:load weight of each item in each row (with E command)
i=[filename]　必須
  input-filename
th=[ratio/threshold] 必須
  ratio/threshold
o=[filename] 
  output-filename
2=[filename]
  2nd input file name
9=[th],[filename]
  write pairs satisfies 2nd threshold [th] to file [filename]
K=[num]
  output [num] pairs of most large similarities
k=[num]
  output [num] elements of most large similarities, for each element
w=[filename]
  read item weights from [filename]
W=[filename]
  read item weights in each row from [filename]
l=,u=[num]
  ignore transactions with size (weight sum) less/more than [num]
L=,U=[num]
  ignore items appearing less/more than [num]
c=[num]
  compare transactions of IDs less than num and the others 
  (if 0 is given, automatically set to the boundary of the 1st and 2nd file)
b=[num]
  ignore pairs having no common item of at least [num]th frequency
B=[num]
  ignore pairs having no common item of frequency at most [num]
T=[num]
  ignore pairs whose intersection size is less than [num]
TT=[num]
  -T with outputting intersection size to the 1st column of each line
stop=[num]
  stop after outputting [num] solutions
separator=[char]
  give the separator of the numbers in the output
Q=[filename]
  replace the output numbers according to the permutation table given by [filename]
# the 1st letter of input-filename cannot be '-'.
	"""

	paraLIST={
		"saki":[("type",1)],
		"ato":[("i",1),("th",1),("o",0)],
		"opt":[("2","-2"),("K","-K"),("k","-k"),("w","-w"),("W","-W"),("l","-l"),("u","-u"),
					("L","-L"),("U","-U"),("c","-c"),("b","-b"),("B","-B"),("T","-T"),("TT","-TT"),
					("Q","-Q"),("stop","-#"),("separator","-,")],
		"opts":[("9","-9",2)]
	}
	kwd = _kwd2list(args,kw_args,paraLIST)
	return lib_sspc.sspc_run(kwd)
	
def medset(*args, **kw_args):
	"""
compute the intersection of each set of sets

keyword
ci=[filename] 必須
  cluster-filename
si=[filename] 必須
  set-filename
th=[threshold] 必須 
  threshold
o=[filename]
  output-filename
H=True
  do not use histgram, just output the items
R=True
  do not output singleton clusters
V=True
  output ratio of appearances of all items
T=True
  clustering by connected component (read edge type file)
I=True
  find an independent set, and clustering by using the vertices in it as seeds 
  (read edge type files)
i=True
  output for each item, ratio of records including the item
t=True
  transpose the input database, (transaction will be item, and vice varsa)
l=[num]
  output clusters of size at least [num]
progress=True
  show progress
nomsg=True
  no message
# the 1st letter of input-filename cannot be '-'.
# if threshold is negative, output the items whose frequencies are no more than the threshold
	"""
	paraLIST={
		"ato":[("ci",1),("si",1),("th",1),("o",1)],
		"opt":[("l","-l")],
		"optB":[("H","-H"),("R","-R"),("V","-V"),("T","-T"),
						("I","-I"),("i","-i"),("t","-t"),
						("progress","-%"),("nomsg","-_")]
	}

	kwd = _kwd2list(args,kw_args,paraLIST)
	return lib_medset.medset_run(kwd)


def simset(*args, **kw_args):
	"""

similarity set

keyword
type=以下の組み合わせ 必須
  %:show progress
  _:no message
  +:write solutions in append mode
  =:do not remove temporal files
  @:do not execute data polishing, E:read edge list file
  i:set similarity measure to the ratio of one is included in the other
  I:set similarity measure to the ratio of both are included in the other
  S:set similarity measure to |A\cap B|/max(|A|,|B|)
  s:set similarity measure to |A\cap B|/min(|A|,|B|)
  C:set similarity measure to the cosign distance, 
    the inner product of the normalized characteristic vectors
  T:set similarity measure to the intersection size, i.e., |A\cap B|
  R:(recemblance) set similarity measure to |A\cap B|/|A\cup B|
  P(PMI): set similarity measure to log 
          (|A\capB|*|all| / (|A|*|B|)) where |all| is the number of all items
  M:output intersection of each clique, instead of IDs of its members
  v (with M): output ratio of records, including each item
  m:do not remove edges in the data polishing phase
  O:repeatedly similarity clustering until convergence
  Y:take intersection of original graph and polished graph, instead of clique mining
  1:do not remove the same items in a record (with -G)
  W:load weight of each element
  t:transpose the input database, so that each line will be considered as a record
i=[filename] 必須
  similarity-graph-filename
th_s=[threshold] 必須
  similarity-threshold
th_d=[threshold] 必須
  degree-threshold
o=[filename]  必須
  output-filename
G=[similarity],[threshold]:
  use [similarity] of [threshold] in the first phase (file is regarded as a transaction database)
k=[threshold]
  find only k-best for each record in -G option
M=[num],[x]
  merge similar cliques of similarity in [num] of recemblance (changes to 'x' by giving '-Mx')
m=[num],[x]
  take independently cliques from similar cliques of similarity in [num] of recemblance, 
  and merge the neighbors of each independent clique ('recemblance' changes to 'x' by giving '-Mx')
v=[num]
  specify majority threshold (default=0.5) 
  (if negative is given, items of frequency no more than -[num] are output)
u=[num]
  ignore vertices of degree more than [num]
l=[num]
  ignore vertices of degree less than [num]
U=[num]
  ignore transactions of size more than [num] (with -G)
L=[num]
  ignore transactions of size less than [num] (with -G)
I=[num]
  ignore items of frequency more than [num] (with -G)
i=[num]
  ignore items of frequency less than [num] (with -G)
II=,ii=[ratio]
  give thresholds by the ratio of #ransactions/#items)
T=,t=[num]
  ignore pairs whose intersection size is less than [num] (T for first phase with -G option, and t for polishing)
O=[num]
  specify the number of repetitions
9=[num]
  shrink records of similarity more than [num]
X=[num]
  multiply the weight by [num] (and trancate by 1, for C command)
x=[num]
  power the weight by [num] (and normalize, for C command)
y=[num]
  at last, remove edges with weight less than [num]
w=[filename]
  load weight of elements from the file
multi=[num]
  use multicores of [num] (compile by 'make multicore')
W=[dir]
  specify the working directory (folder). 
  The last letter of the directory has to be '/' ('\')
separator=
  give the separator of the numbers in the output
Q=
  replace the output numbers according to the permutation table given by [filename]

# the 1st letter of input-filename cannot be '-'.
if similarity-threshold is 0, skip the similarity graph construction phase
	"""

	paraLIST={
		"saki":[("type",1)],
		"ato":[("i",1),("th_s",1),("th_d",1),("o",1)],

		"opt":[("k","-k"),("v","-v"),("u","-u"),("l","-l"),("U","-U"),("L","-L"),
					("Iig","-I"),("iig","-i"),("II","-II"),("ii","-ii"),("T","-T"),("t","-t"),
					("O","-O"),("9","-9"),("X","-X"),("x","-x"),("y","-y"),
					("w","-w"),("multi","-!"),("W","-W"),("Q","-Q"),("separator","-,")],
		"opts":[("G","-G",2)],
		"addkw":[("m","-m"),("M","-M")],
	}


	kwd = _kwd2list(args,kw_args,paraLIST)
	return lib_simset.simset_run(kwd)

#(:ref:`詳細<grhfilType-label>` )

def grhfil(*args, **kw_args):
	"""
	*グラフフィルタ(graph filtering: transform/convert/extract graph/subgraph)*

	:type type: 文字列
	:type t: 整数
	:type T: 整数
	:type ideg: 整数
	:type Ideg: 整数
	:type odeg: 整数
	:type Odeg: 整数
	:type r: 実数
	:type R: 実数
	:type n: 整数
	:type X: 実数リスト
	:type w: 真偽
	:type W: 真偽
	:type d: ファイル名
	:type m: ファイル名
	:type M: ファイル名
	:type p: ファイル名
	:type Q: ファイル名
	:type separator: 文字
	:param type: 主パラメータ(:ref:`以下に詳述<grhfilMP-label>` )【必須】
	:param i: 入力ファイル名【必須】
	:param o: 出力ファイル名【必須】
	:param t: 次数が指定した値より小さいノードは出力しない【デフォルト=制約なし】
	:param T: 次数が指定した値より大きいノードは出力しない
	:param ideg: 有向グラフにおいて入次数が指定した値より小さいノードは出力しない
	:param Ideg: 有向グラフにおいて入次数が指定した値より大きいノードは出力しない
	:param odeg: 有向グラフにおいて出次数が指定した値より小さいノードは出力しない
	:param Odeg: 有向グラフにおいて出次数が指定した値より大きいノードは出力しない
	:param r: 重みが指定した値より小さいエッジは出力しない
	:param R: 重みが指定した値より大きいエッジは出力しない
	:param n: ノードの個数を与える【デフォルト=データ上の最大番号+1】
	:param w: 入力ファイルの3項目目を重みとして読み込む(3項目目がなければ0がセットされる)
	:param W: 出力ファイルの3項目目に重みを出力する
	:param d: 指定したグラフとの差異を出力する(gTypeとgFormatは同じでなければならない)
	:param dth: specify the threshold value (d=が必要)
	:param m: 指定したグラフとのintersectionを出力する(gTypeとgFormatは同じでなければならない)
	:param M: 指定したグラフとのunionを出力する(gTypeとgFormatは同じでなければならない)
	:param p: permute the vertex ID to coutinuous numbering and output the permutation table to the file
	:param Q: permute the numbers in the file according to the table 

	.. todo:: P,Qは未対応

	.. _grhfilMP-label:

	**主パラメータ**

	以下に示すパラメータを組み合わせて文字列として指定する(ex. "UeEs")。

	1) グラフタイプ【必須】
	グラフの種類を以下の5つから一つ選んで指定する。

	* B:二部グラフ
	* d:有向グラフ(入力隣接行列を変更しない)
	* D:逆向きの有向グラフ(隣接行列の下三角行列と上三角行列を入れ替える)
	* U:片方向無向グラフ(隣接行列の下三角行列を削除し、重複したエッジを削除する)
	* u:双方向無向グラフ(入力隣接行列の下三角行列と上三角行列を対称にする)

	2) グラフの書式【オプション】
	入出力ファイルのグラフの書式は、隣接行列形式がデフォルトであるが、
	それをノードペアのエッジ形式に変更することができる。

	* e: 入力ファイルをエッジ形式とする。
	* E: 出力ファイルをエッジ形式とする。

	3) その他の指定

	* s: 出力のグラフの隣接ノードを昇順に並べる。
	* S: 出力のグラフの隣接ノードを降順に並べる。
	* n: 入力データの一行目を"ノード数 エッジ数"とする。
	* N: 出力データの一行目に"ノード数 エッジ数"を出力する。
	* v: 入力データのノード番号は1から始まるものとする。
	* V: 出力データのノード番号は1からはじまるものとして出力する。
	* 0: 一列目にノード番号を出力する(出力が隣接行列形式の場合のみ)。
	* w: 入力ファイルの隣接ノードの次の数字を重みとする。
	* W: 出力ファイルの隣接ノードの次に重みを出力する。
	* %:show progress
	* _:no message
	* +:write solutions in append mode

	* v,V:node ID in read/write file starts from 1,
	* q:non-transform mode (valid with -P option)
	* 9:give weight 1 to each vertex ID (with 0)
	* 1:unify consecutive two same numbers into one

	例1) A-Fの6アイテムにつて、5件(T1-T6)のトランザクションをCSVファイルに保存し、
	TraDBを構築する例。

	.. code-block:: python
		:linenos:

		>>> from nysol.take.extcore import grhfil
		>>> from nysol.take.sample import Samples
		>>> smp=Samples()
		>>> fName=smp.numGraph1()
		>>> smp.head(fName)
		fName: ./numGraph1.csv
		0 1
		0 2
		0 4
		1 2
		1 4
		1 6

		>>> grhfil(type="ueE_",i=fName,o="xxa")
		>>> smp.head("xxa")
	"""

	paraLIST={
		"saki":[("type",1)],
		"ato":[("i",1),("o",1)],
		"opt":[("t","-t"),("T","-T"),("ideg","-i"),("Ideg","-I"),
					("odeg","-o"),("Odeg","-O"),("r","-r"),("r","-R"),("n","-n"),
					("w","-w"),("W","-W"),("p","-p"),("Q","-Q"),("separator","-,")],
		"opts":[("X","-X",2)],
		"addkw":[("d","-d"),("m","-m"),("M","-M")],
		"optato":[("dth","-d")]
	}

	kw_args2=kw_args

	"""
	gType=kw_args["gType"]
	giFormat=kw_args["giFormat"]
	goFormat=kw_args["goFormat"]
	if giFormat=="e":
		mainArg+="e"
	if goFormat=="e":
		mainArg+="E"
	kw_args2["type"]=mainArg
	"""

	kwd = _kwd2list(args,kw_args,paraLIST)
	return lib_grhfil.grhfil_run(kwd)

def lcmtrans(inf,ptn,outf):
	"""
	lcmの結果をcsvへと変換する
	"""
	return lib_lcmtarns.lcmtrans_run(inf,ptn,outf)


def mmaketra(**kw_args):
	"""
	csvからtraデータへと変換する
	"""
	return lib_mmaketra.mmaketra_run(**kw_args)
	
	
