# -*- coding: utf-8 -*-
import nysol._nysolshell_core as n_core
from nysol.mcmd.nysollib.core import NysolMOD_CORE
from nysol.mcmd.nysollib import nysolutil as nutil

class Nysol_Mnrcommon(NysolMOD_CORE):
	kwd = n_core.getparalist("mnrcommon")
	def __init__(self,*args, **kw_args) :
		super(Nysol_Mnrcommon,self).__init__("mnrcommon",nutil.args2dict(args,kw_args,Nysol_Mnrcommon.kwd))

def mnrcommon(self,*args, **kw_args):
	return Nysol_Mnrcommon(nutil.args2dict(args,kw_args,Nysol_Mnrcommon.kwd)).addPre(self)

setattr(NysolMOD_CORE, "mnrcommon", mnrcommon)
