# -*- coding: utf-8 -*-
import nysol._nysolshell_core as n_core
from nysol.mcmd.nysollib.core import NysolMOD_CORE
from nysol.mcmd.nysollib import nysolutil as nutil

class Nysol_Mtab2csv(NysolMOD_CORE):

	_kwd ,_inkwd,_outkwd = n_core.getparalist("mtab2csv",3)

	def __init__(self,*args, **kw_args) :
		super(Nysol_Mtab2csv,self).__init__("mtab2csv",nutil.args2dict(args,kw_args,Nysol_Mtab2csv._kwd))

def mtab2csv(self,*args, **kw_args):
	return Nysol_Mtab2csv(nutil.args2dict(args,kw_args,Nysol_Mtab2csv._kwd)).addPre(self)

setattr(NysolMOD_CORE, "mtab2csv", mtab2csv)
