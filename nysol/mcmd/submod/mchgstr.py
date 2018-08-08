# -*- coding: utf-8 -*-
import nysol._nysolshell_core as n_core
from nysol.mcmd.nysollib.core import NysolMOD_CORE
from nysol.mcmd.nysollib import nysolutil as nutil

class Nysol_Mchgstr(NysolMOD_CORE):

	_kwd ,_inkwd,_outkwd  = n_core.getparalist("mchgstr",3)

	def __init__(self,*args, **kw_args) :
		super(Nysol_Mchgstr,self).__init__("mchgstr",nutil.args2dict(args,kw_args,Nysol_Mchgstr._kwd))

def mchgstr(self,*args, **kw_args):
	return Nysol_Mchgstr(nutil.args2dict(args,kw_args,Nysol_Mchgstr._kwd)).addPre(self)

setattr(NysolMOD_CORE, "mchgstr", mchgstr)

