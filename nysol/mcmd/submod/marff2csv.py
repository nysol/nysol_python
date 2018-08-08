# -*- coding: utf-8 -*-
import nysol._nysolshell_core as n_core
from nysol.mcmd.nysollib.core import NysolMOD_CORE
from nysol.mcmd.nysollib import nysolutil as nutil

class Nysol_Marff2csv(NysolMOD_CORE):

	_kwd ,_inkwd,_outkwd  = n_core.getparalist("marff2csv",3)

	def __init__(self,*args, **kw_args) :
		super(Nysol_Marff2csv,self).__init__("marff2csv",nutil.args2dict(args,kw_args,Nysol_Marff2csv._kwd))

def marff2csv(self,*args, **kw_args):
	return Nysol_Marff2csv(nutil.args2dict(args,kw_args,Nysol_Marff2csv._kwd)).addPre(self)

setattr(NysolMOD_CORE, "marff2csv", marff2csv)

