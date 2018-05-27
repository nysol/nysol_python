
from nysol.take.traDB import TraDB


d=[
["receiptNo","item"],
["1001","牛乳"],
["1001","シリアル"],
["1001","ヨーグルト"],
["1002","牛乳"],
["1002","卵"]
]

d='''receiptNo,item
1001,牛乳
1001,シリアル
1001,ヨーグルト
1002,牛乳
1002,卵
'''

d='''id,item
"T1","C"
"T1","E"
"T2","D"
"T2","E"
"T2","F"
"T3","A"
"T3","B"
"T3","D"
"T3","A"
"T4","B"
"T4","D"
"T4","F"
"T5","A"
"T5","B"
"T5","D"
"T5","E"
"T6","A"
"T6","B"
"T6","D"
"T6","E"
"T6","F"
'''

with open("dat1.csv","w") as fpw:
	fpw.write(d)

db=TraDB(iFile="dat1.csv",idFN="id",itemFN="item")
print(db)

