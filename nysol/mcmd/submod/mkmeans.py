# -*- coding: utf-8 -*-
import nysol._nysolshell_core as n_core
from nysol.mcmd.nysollib.core import NysolMOD_CORE
from nysol.mcmd.nysollib import nysolutil as nutil

class Nysol_Mkmeans(NysolMOD_CORE):

	_kwd ,_inkwd,_outkwd  = n_core.getparalist("mkmeans",3)

	def __init__(self,*args, **kw_args) :
		super(Nysol_Mkmeans,self).__init__("mkmeans",nutil.args2dict(args,kw_args,Nysol_Mkmeans._kwd))

def mkmeans(self,*args, **kw_args):
	return Nysol_Mkmeans(nutil.args2dict(args,kw_args,Nysol_Mkmeans._kwd)).addPre(self)

setattr(NysolMOD_CORE, "mkmeans", mkmeans)

