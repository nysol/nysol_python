# -*- coding: utf-8 -*-
import nysol._nysolshell_core as n_core
from nysol.mcmd.nysollib.core import NysolMOD_CORE
from nysol.mcmd.nysollib import nysolutil as nutil

class Nysol_Mchgnum(NysolMOD_CORE):
	_kwd ,_inkwd,_outkwd = n_core.getparalist("mchgnum",3)

	def __init__(self,*args, **kw_args) :
		super(Nysol_Mchgnum,self).__init__("mchgnum",nutil.args2dict(args,kw_args,Nysol_Mchgnum._kwd))

def mchgnum(self,*args, **kw_args):
	return Nysol_Mchgnum(nutil.args2dict(args,kw_args,Nysol_Mchgnum._kwd)).addPre(self)

setattr(NysolMOD_CORE, "mchgnum", mchgnum)

