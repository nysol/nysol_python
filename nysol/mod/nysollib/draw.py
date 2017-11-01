# -*- coding: utf-8 -*-
import sys
# これは簡易版
def dicisionPosSub(mlist,iolist,baselist,dsppos,y,counter):

	for i in baselist:
		if dsppos[i] != None:
			continue
		dsppos[i] = [counter[y],y]
		counter[y]+=1
		newlist = iolist[i][2] + iolist[i][3] 
		if len(counter)==y+1:
			counter.append(0)
		dicisionPosSub(mlist,iolist,newlist,dsppos,y+1,counter)

	
def dicisionPos(mlist,iolist):

	startpos = []
	for i , mm in enumerate(mlist):
		if len(mm[2])!=0:
			if "i" in mm[2]:
				startpos.append(i)
			if "m" in mm[2]:
				startpos.append(i)

	dsppos   = [None]*len(mlist)
	y=0
	counter =[0]
	dicisionPosSub(mlist,iolist,startpos,dsppos,y,counter)
	
	return dsppos , len(counter) ,max(counter)

	#for i in startpos:
	#	if dsppos[i] != None:
	#		continue
	#	y=0
	#	dsppos[i] = [x,y]
	#	newlist = iolist[i][2] + iolist[i][3] 
	#	dicisionPosSub(mlist,iolist,newlist,dsppos,x,y)

def chageSVG(mlist,iolist,linklist,fname=None):
	dsppos,ymax,xmax = dicisionPos(mlist,iolist)

	if fname == None:
		f=sys.stdout
	else:
		f=open(fname, 'w')
	
	f.write("<svg height='"+  str(ymax*60) + "' width='" + str(xmax*60) + "'>\n")
	f.write("<defs>\n")
	f.write("<marker id='endmrk' markerUnits='strokeWidth' markerWidth='3' markerHeight='3' viewBox='0 0 10 10' refX='5' refY='5' orient='auto'>\n")
	f.write("<polygon points='0,0 5,5 0,10 10,5 ' fill='black'/>\n")
	f.write("</marker>\n")
	f.write("</defs>\n")

	for i , mm in enumerate(dsppos):
		modobj = mlist[i]
		x,y = mm


		f.write("<g>\n")
		f.write("<title>" + modobj[0] + " " + modobj[1] + "</title>\n" )

		mstr = "<circle cx='" + str(x*60+20) + "' cy='" + str(y*60+20) + "' r='20' stroke='blue' fill='white' stroke-width='1'/>\n"
		f.write(mstr)
		mstr = "<text x='" + str(x*60) + "' y='" + str(y*60+20) + "' fill='black'>"
		mstr +=  modobj[0]
		mstr += " </text>\n"
		f.write(mstr)
		f.write("<g>\n")


	for fr , to in linklist:
		frNo = fr[1] 
		toNo = to[1] 
		frX , frY = dsppos[frNo]
		toX , toY = dsppos[toNo]
		f.write("<line x1='" + str(frX*60+20) + "' y1='" + str(frY*60+40) + "' x2='" + str(toX*60+20) + "' y2='" + str(toY*60+0) + "' stroke='black' stroke-width='5' marker-end='url(#endmrk)'/>\n")


	f.write("</svg>\n")

	f.close()



