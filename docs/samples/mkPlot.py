
import os
from igraph import *
from nysol.take.sample import Samples

oPath="plot"
os.system("mkdir -p plot")

def runGraph(name):
	dat=Samples(name)

	n=dat.getNodes()
	e=dat.getEdges()
	print(n)
	print(e)

	png="{}/{}.png".format(oPath,name)
	print("writing plot to {}".format(png))
	dat.plot(oFile=png)
	#dat.plot()

def runTra(name):
	dat=Samples(name)

	n=dat.getNodes()
	e=dat.getEdges()
	print(n)
	print(e)

	png="{}/{}.png".format(oPath,name)
	print("writing plot to {}".format(png))
	dat.plot(oFile=png)


runGraph("graph00")
runTra("tra00")

exit()
smp=Samples()
v,e=smp.numGraph1()

#vertices = ["ヤマダ", "Tanaka", "Suzuki", "Sato", "Ito", "Obokata"]
#edges = [(0,1),(1,3),(1,4),(1,5),(2,5),(3,5),(4,3),(0,5)]
#g = Graph(vertex_attrs={"label": vertices}, edges=edges, directed=True)

# http://igraph.org/python/doc/tutorial/tutorial.html
#vertices = list(range(5))
#edges = [(0,1),(1,3),(1,4),(1,5),(2,5),(3,5),(4,3),(0,5)]
#g = Graph(vertex_attrs={"label": vertices}, edges=edges, directed=True)

g = Graph(edges=e)
vStyle = {}
vStyle["vertex_size"] = 30
vStyle["vertex_color"] = ["white" for gender in v]
vStyle["vertex_label"] = v
vStyle["edge_width"] = [1 for i in e]
vStyle["layout"] = g.layout("kamada_kawai")
vStyle["bbox"] = (200, 200)
vStyle["margin"] = 30
plot(g,"xxa.png",**vStyle)

"""
import igraph as ig
import numpy as np

dim = 30,30
A   = np.ones(dim)
A   = A - np.diag(np.diag(A))
g   = ig.Graph.Adjacency((A+A.T).tolist(),mode=True)
#ig.plot(g, "plot.png", layout=g.layout("circle"), bbox=(800,800), vertex_label=None)
ig.plot(g, "plot.png", layout=g.layout("circle"), bbox=(800,800), vertex_label=None)

"""
