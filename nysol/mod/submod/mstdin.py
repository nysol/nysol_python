# -*- coding: utf-8 -*-
import nysol._nysolshell_core as n_core
from nysol.mod.nysollib.core import NysolMOD_CORE
from nysol.mod.nysollib import nysolutil as nutil

class Nysol_Mstdin(NysolMOD_CORE):
	kwd = n_core.getparalist("mstdin")
	inkwd = ["i="]
	outkwd = ["o="]
	def __init__(self,*args, **kw_args) :
		super(Nysol_Mstdin,self).__init__("mstdin",nutil.args2dict(args,kw_args,Nysol_Mstdin.kwd))

def mstdin(self,*args, **kw_args):
	return Nysol_Mstdin(nutil.args2dict(args,kw_args,Nysol_Mstdin.kwd)).addPre(self)


