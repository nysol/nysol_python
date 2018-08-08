# -*- coding: utf-8 -*-
import nysol._nysolshell_core as n_core
from nysol.mcmd.nysollib.core import NysolMOD_CORE
from nysol.mcmd.nysollib import nysolutil as nutil

class Nysol_Mnrcommon(NysolMOD_CORE):

	_kwd,_inkwd,_outkwd  = n_core.getparalist("mnrcommon",3)

	def __init__(self,*args, **kw_args) :
		super(Nysol_Mnrcommon,self).__init__("mnrcommon",nutil.args2dict(args,kw_args,Nysol_Mnrcommon._kwd))

def mnrcommon(self,*args, **kw_args):
	return Nysol_Mnrcommon(nutil.args2dict(args,kw_args,Nysol_Mnrcommon._kwd)).addPre(self)

setattr(NysolMOD_CORE, "mnrcommon", mnrcommon)
