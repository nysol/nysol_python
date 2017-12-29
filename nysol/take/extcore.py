#!/usr/bin/env python
# -*- coding: utf-8 -*-
"""
	TAKE LIBRARY call Function
"""
import nysol.take._grhfillib as lib_grhfil
import nysol.take._lcmlib as lib_lcm
import nysol.take._lcmseq_zerolib as lib_lcmseq0
import nysol.take._lcmseqlib  as lib_lcmseq
import nysol.take._lcmtranslib  as lib_lcmtarns
import nysol.take._macelib as lib_mace
import nysol.take._medsetlib as lib_medset
import nysol.take._simsetlib as lib_simset
import nysol.take._sspclib as lib_sspc


def kwd2list(args, kw_args,paraConf,extpara=None):
	"""
kwdをstingのkwdに変換

	"""
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
Linear time Closed itemset Miner

keyword
type=以下の組み合わせ 必須
  %:show progress
  _:no message
  +:write solutions in append mode
  F:frequent itemset mining
  C:closed frequent itemset mining
  M:maximal frequent itemset mining
  P:positive-closed itemset mining
  f:output frequency following to each output itemset
  A:output positive/negative frequency, and their ratio
  Q:output frequency and coverages preceding to itemsets
  R:output redundant items for rule mining 
    (usually, redundant items are removed, to be minimal, in the case of rule mining)
  I:output ID's of transactions including each pattern
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
  output most frequent itemsets [num]
l=,u=[num] 
  output itemsets with size at least/most [num]
U=[num]
  upper bound for support(maximum support)
w=[filename]
  read weights of transactions from the file
c=,C=[filename]
  read item constraint/un-constraint file
item=[num]
  find association rule for item [num]
a=,A=[ratio]
  find association rules of confidence at least/most [ratio]
r=,R=[ratio]
  find association rules of relational confidence at least/most [ratio]
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
so=[filename]
  standard output to [filename]
# the 1st letter of input-filename cannot be '-'.
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
	kwd = kwd2list(args,kw_args,paraLIST,extpara)
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
	kwd = kwd2list(args,kw_args,paraLIST,extpara)
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
	kwd = kwd2list(args,kw_args,paraLIST)
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

	kwd = kwd2list(args,kw_args,paraLIST)
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
	kwd = kwd2list(args,kw_args,paraLIST)
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

	kwd = kwd2list(args,kw_args,paraLIST)
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


	kwd = kwd2list(args,kw_args,paraLIST)
	return lib_simset.simset_run(kwd)


def grhfil(*args, **kw_args):
	"""
graph filtering: transform/convert/extract graph/subgraph

keyword
type=以下の組み合わせ 必須
  %:show progress
  _:no message
  +:write solutions in append mode
  d:directed graph (x->y)
  D:directed graph with reverse direction (x<-y)
  U,u:undirected graph (u:edge for both direction), 
  B:bipartite graph, 
  e,E:read/write file as edge list
  s,S:sort vertex adjacent list in increasing/decreasing order
  n,N:read/write the number of vertices and edges written in/at 1st line of the file
  v,V:node ID in read/write file starts from 1, q:non-transform mode (valid with -P option)
  0:insert vertex ID as the first entry, for each vertex (adjacency list mode)
  9:give weight 1 to each vertex ID (with 0)
  w,W:read/write edge weights in the graph file
  1:unify consecutive two same numbers into one
i=[filename] 必須
  input-file
o=[filename] 必須
  output-file
t=,T=[num]
  remove vertices with degree smaller/larger then [num]
ideg=,Ideg=[num]
  remove vertices with in-degree smaller/larger then [num]
odeg=,Odeg=[num]
  remove vertices with out-degree smaller/larger then [num]
r=,R=[num]
  remove edges with weights smaller/larger then [num]
n=[num]
  specify #nodes
X=[num1],[num2]
  multiply each weight by [num1] and trancate by [num2]
w,W=[filename]
  weight file to be read/write
d=[filename],[type]
  take difference with graph of [filename] 
  [type]はtype=の'u','U','B','D','d','e','s','S','n','w','v'を組み合わせる  
dth=[threshold]
 specify the threshold value (d=が必要)
m=,M=[filename],[type]
  take intersection/union with graph of [filename]
  [type]はtype=の'u','U','B','D','d','e','s','S','n','w','v'を組み合わせる  
p=[filename]
  permute the vertex ID to coutinuous numbering and output the permutation table to the file
separator=[char]
  give the separator of the numbers in the output
Q=[filename]
  permute the numbers in the file according to the table 
	"""

	paraLIST={
		"saki":[("type",1)],
		"ato":[("i",1),("o",1)],
		"opt":[("t","-t"),("T","-T"),("ideg","-i"),("Ideg","-I"),
					("odeg","-o"),("Odeg","-O"),("r","-r"),("r","-R"),("n","-n"),
					("w","-w"),("W","-W"),("P","-P"),("Q","-Q"),("separator","-,")],
		"opts":[("X","-X",2)],
		"addkw":[("d","-d"),("m","-m"),("M","-M")],
		"optato":[("dth","-d")]
	}
	kwd = kwd2list(args,kw_args,paraLIST)
	return lib_grhfil.grhfil_run(kwd)

def lcmtrans(inf,ptn,outf):
	"""
	lcmの結果をcsvへと変換する
	"""
	return lib_lcmtarns.lcmtrans_run(inf,ptn,outf)
	
	