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
		if  modobj[3] == "" :
			f.write("<title>" + modobj[0] + " " + modobj[1] + "</title>\n" )
		else:
			f.write("<title>" + modobj[0] + " " + modobj[1] + "@" + modobj[3] + "</title>\n" )
		
		mstr = "<circle cx='" + str(x*60+20) + "' cy='" + str(y*60+20) + "' r='20' stroke='blue' fill='white' stroke-width='1'/>\n"
		f.write(mstr)
		mstr = "<text x='" + str(x*60) + "' y='" + str(y*60+20) + "' fill='black'>"
		mstr +=  modobj[0]
		mstr += " </text>\n"
		f.write(mstr)
		f.write("</g>\n")


	for fr , to in linklist:
		frNo = fr[1] 
		toNo = to[1] 
		frTp = fr[0] 
		toTp = to[0] 
		 
		frX , frY = dsppos[frNo]
		toX , toY = dsppos[toNo]
		f.write("<g>\n")
		f.write("<title>" + frTp + " => " + toTp + "</title>\n" )
		f.write("<line x1='" + str(frX*60+20) + "' y1='" + str(frY*60+40) + "' x2='" + str(toX*60+20) + "' y2='" + str(toY*60+0) + "' stroke='black' stroke-width='5' marker-end='url(#endmrk)'/>\n")
		f.write("</g>\n")


	f.write("</svg>\n")

	f.close()




def chageSVG_D3(mlist,iolist,linklist,fname=None):
	dsppos,ymax,xmax = dicisionPos(mlist,iolist)

	if fname == None:
		f=sys.stdout
	else:
		f=open(fname, 'w')
	
	f.write("<html>\n")
	f.write("<head>\n")
	
	f.write("<script src='http://d3js.org/d3.v3.min.js' charset='utf-8'></script>\n")
	f.write("<script>\n")
	f.write("var NodeDATA=[")

	mlastNo = len(dsppos)
	for i , mm in enumerate(dsppos):

		modobj = mlist[i]
		x,y = mm
		if  modobj[3] == "" :
			f.write("{ title:\"%s %s\"," % (modobj[0],modobj[1]) ) 
		else:
			f.write("{ title:\"%s %s @ %s\"," % (modobj[0],modobj[1],modobj[3]) ) 

		f.write(" x:%d , y:%d , name:\"%s\"}" % (x*60+20,y*60+20, modobj[0]) ) 
		if mlastNo==i+1 :
			f.write("]\n")
		else:
			f.write(",\n")

	f.write("var EdgeDATA=[")

	elastNo = len(linklist)
	linklist_n2e=[None]*mlastNo

	for i , frto in enumerate(linklist):
		fr,to = frto
		frNo = fr[1] 
		toNo = to[1] 
		frTp = fr[0] 
		toTp = to[0] 
		 
		frX , frY = dsppos[frNo]
		toX , toY = dsppos[toNo]


		if linklist_n2e[frNo] == None:
			linklist_n2e[frNo] =[[],[]]

		linklist_n2e[frNo][0].append(str(i))

		if linklist_n2e[toNo] == None:
			linklist_n2e[toNo] =[[],[]]

		linklist_n2e[toNo][1].append(str(i))

		f.write("{ title:\"%s => %s \"," % (frTp,toTp))
		f.write(" x1:%d,y1:%d,x2:%d,y2:%d }" % (frX*60+20,frY*60+40,toX*60+20,toY*60+0))

		if elastNo==i+1 :
			f.write("]\n")
		else:
			f.write(",\n")

	n2elastNo = len(linklist_n2e)
	f.write("var LinkLIST=[")
	for i , n2elist in enumerate(linklist_n2e):
		f.write("[[%s],[%s]]"%(",".join(n2elist[0]),",".join(n2elist[1])))
		if n2elastNo ==i+1 :
			f.write("]\n")
		else:
			f.write(",\n")


	f.write("</script>\n")
	f.write("</head>")
	f.write("<body>")
	f.write("<svg id='flowDspArea' height='%d' width='%d'>\n" % (ymax*60*2 ,xmax*60*2))
	f.write("<defs>\n")
	f.write("<marker id='endmrk' markerUnits='strokeWidth' markerWidth='3' markerHeight='3' viewBox='0 0 10 10' refX='5' refY='5' orient='auto'>\n")
	f.write("<polygon points='0,0 5,5 0,10 10,5 ' fill='black'/>\n")
	f.write("</marker>\n")
	f.write("</defs>\n")
	f.write("</svg>\n")
	scp = """
	<script>
	svgGroup = d3.select('#flowDspArea');
	node_g = svgGroup.selectAll('g .node').data(NodeDATA);
	edge_g = svgGroup.selectAll('g .edge').data(EdgeDATA);
	// 移動処理用
	var drag = d3.behavior.drag()
	drag.on('drag', dragMove);
	function dragMove(d,i) {
		d.x += d3.event.dx
  	d.y += d3.event.dy
   	d3.select(this)
   		.attr('transform','translate('+d.x+','+d.y+')')
		for(var j=0 ; j<LinkLIST[i][0].length;j++){
			EdgeDATA[LinkLIST[i][0][j]].x1 += d3.event.dx
			EdgeDATA[LinkLIST[i][0][j]].y1 += d3.event.dy
	   	d3.select('#edgeP-'+LinkLIST[i][0][j])
  	 		.attr('x1' ,function (ds) {return ds.x1;})
  	 		.attr('y1' ,function (ds) {return ds.y1;})
		}
		for(var j=0 ; j<LinkLIST[i][1].length;j++){
			EdgeDATA[LinkLIST[i][1][j]].x2 += d3.event.dx
			EdgeDATA[LinkLIST[i][1][j]].y2 += d3.event.dy
	   	d3.select('#edgeP-'+LinkLIST[i][1][j])
  	 		.attr('x2' ,function (ds) {return ds.x2;})
  	 		.attr('y2' ,function (ds) {return ds.y2;})
		}
	}
 	node_g2 = node_g.enter().append('g')
		.attr('class', 'node')
		.attr('id', function (d,i) {return 'node-' + i;})
		.attr('transform',function (d) { return 'translate('+d.x+','+d.y+')'})
    .call(drag)		

	node_g2.append('title')
			.text(function(d) { return d.title})

	node_g2.append('circle')
		.attr('r',20)
		.attr('stroke','blue')
		.attr('fill','white')
		.attr('stroke-width',1)

	node_g2.append('text')
		.attr('x',function(d) { return -20})
		.attr('fill','black')
		.text(function(d) { return d.name})

 	edge_g2 = edge_g.enter().append('g')
		.attr('class', 'edge')
		.attr('id', function (d,i) {return 'edge-' + i;})

	edge_g2.append('line')
		.attr('id', function (d,i) {return 'edgeP-' + i;})
		.attr('x1',function(d) { return d.x1})
		.attr('x2',function(d) { return d.x2})
		.attr('y1',function(d) { return d.y1})
		.attr('y2',function(d) { return d.y2})
		.attr('stroke','black')
		.attr('stroke-width','5')
		.attr('marker-end','url(#endmrk)')
	</script>
	"""
	f.write(scp)
	f.write("</body>\n")
	f.write("</html>\n")
	f.close()




