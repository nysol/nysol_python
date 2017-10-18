# -*- coding: utf-8 -*-
import nysol._nysolshell_core as n_core
from nysol.mod.nysollib.nysolmodcore import NysolMOD_CORE
from nysol.mod.nysollib import nysolutil as nutil

class Nysol_Msim(NysolMOD_CORE):
	kwd = n_core.getparalist("msim")
	def __init__(self,*args, **kw_args) :
		super(Nysol_Msim,self).__init__("msim",nutil.args2dict(args,kw_args,Nysol_Msim.kwd))

