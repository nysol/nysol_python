# -*- coding: utf-8 -*-
import nysol._nysolshell_core as n_core
from nysol.mcmd.nysollib.core import NysolMOD_CORE
from nysol.mcmd.nysollib import nysolutil as nutil

class Nysol_Mxml2csv(NysolMOD_CORE):

	_kwd ,_inkwd,_outkwd = n_core.getparalist("mxml2csv",3)

	def __init__(self,*args, **kw_args) :
		super(Nysol_Mxml2csv,self).__init__("mxml2csv",nutil.args2dict(args,kw_args,Nysol_Mxml2csv._kwd))

def mxml2csv(self,*args, **kw_args):
	return Nysol_Mxml2csv(nutil.args2dict(args,kw_args,Nysol_Mxml2csv._kwd)).addPre(self)

setattr(NysolMOD_CORE, "mxml2csv", mxml2csv)

