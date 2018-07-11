# -*- coding: utf-8 -*-
import nysol._nysolshell_core as n_core
from nysol.mcmd.nysollib.core import NysolMOD_CORE
from nysol.mcmd.nysollib import nysolutil as nutil

class Nysol_Mnormalize(NysolMOD_CORE):
	kwd = n_core.getparalist("mnormalize")
	def __init__(self,*args, **kw_args) :
		super(Nysol_Mnormalize,self).__init__("mnormalize",nutil.args2dict(args,kw_args,Nysol_Mnormalize.kwd))


def mnormalize(self,*args, **kw_args):
	return Nysol_Mnormalize(nutil.args2dict(args,kw_args,Nysol_Mnormalize.kwd)).addPre(self)

setattr(NysolMOD_CORE, "mnormalize", mnormalize)
