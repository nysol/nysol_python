# -*- coding: utf-8 -*-
import nysol._nysolshell_core as n_core
from nysol.mcmd.nysollib.core import NysolMOD_CORE
from nysol.mcmd.nysollib import nysolutil as nutil

class Nysol_Mselnum(NysolMOD_CORE):

	_kwd,_inkwd,_outkwd = n_core.getparalist("mselnum",3)

	def __init__(self,*args, **kw_args) :
		super(Nysol_Mselnum,self).__init__("mselnum",nutil.args2dict(args,kw_args,Nysol_Mselnum._kwd))

def mselnum(self,*args, **kw_args):
	return Nysol_Mselnum(nutil.args2dict(args,kw_args,Nysol_Mselnum._kwd)).addPre(self)

setattr(NysolMOD_CORE, "mselnum", mselnum)
