# -*- coding: utf-8 -*-
import nysol._nysolshell_core as n_core
from nysol.mcmd.nysollib.core import NysolMOD_CORE
from nysol.mcmd.nysollib import nysolutil as nutil

class Nysol_Mnumber(NysolMOD_CORE):

	_kwd ,_inkwd,_outkwd = n_core.getparalist("mnumber",3)

	def __init__(self,*args, **kw_args) :
		super(Nysol_Mnumber,self).__init__("mnumber",nutil.args2dict(args,kw_args,Nysol_Mnumber._kwd))

def mnumber(self,*args, **kw_args):
	return Nysol_Mnumber(nutil.args2dict(args,kw_args,Nysol_Mnumber._kwd)).addPre(self)

setattr(NysolMOD_CORE, "mnumber", mnumber)
