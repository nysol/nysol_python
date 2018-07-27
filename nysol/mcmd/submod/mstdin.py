# -*- coding: utf-8 -*-
import nysol._nysolshell_core as n_core
from nysol.mcmd.nysollib.core import NysolMOD_CORE
from nysol.mcmd.nysollib import nysolutil as nutil

class Nysol_Mstdin(NysolMOD_CORE):

	_kwd ,_inkwd,_outkwd  = n_core.getparalist("mstdin",3)

	def __init__(self,*args, **kw_args) :
		super(Nysol_Mstdin,self).__init__("mstdin",nutil.args2dict(args,kw_args,Nysol_Mstdin._kwd))

def mstdin(self,*args, **kw_args):
	return Nysol_Mstdin(nutil.args2dict(args,kw_args,Nysol_Mstdin._kwd)).addPre(self)

#nm.mstdinは使えるようにする?

