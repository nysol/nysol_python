# -*- coding: utf-8 -*-
import nysol._nysolshell_core as n_core
from nysol.mcmd.nysollib.core import NysolMOD_CORE
from nysol.mcmd.nysollib import nysolutil as nutil

class Nysol_Mcut(NysolMOD_CORE):

	_kwd ,_inkwd,_outkwd  = n_core.getparalist("mcut",3)

	def __init__(self,*args, **kw_args) :
		super(Nysol_Mcut,self).__init__("mcut",nutil.args2dict(args,kw_args,Nysol_Mcut._kwd))

def mcut(self,*args, **kw_args):
	return Nysol_Mcut(nutil.args2dict(args,kw_args,Nysol_Mcut._kwd)).addPre(self)

setattr(NysolMOD_CORE, "mcut", mcut)

