# -*- coding: utf-8 -*-
import nysol._nysolshell_core as n_core
from nysol.mod.nysollib.core import NysolMOD_CORE
from nysol.mod.nysollib import nysolutil as nutil

class Nysol_Mnewstr(NysolMOD_CORE):
	kwd = n_core.getparalist("mnewstr")
	def __init__(self,*args, **kw_args) :
		super(Nysol_Mnewstr,self).__init__("mnewstr",nutil.args2dict(args,kw_args,Nysol_Mnewstr.kwd))

