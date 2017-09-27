# -*- coding: utf-8 -*-
import _nysolshell_core as n_core
from nysolmod.nysollib.nysolmodcore import NysolMOD_CORE
import nysolmod.nysollib.nysolutil as nutil

class Nysol_Mvreplace(NysolMOD_CORE):
	kwd = n_core.getparalist("mvreplace")
	def __init__(self,*args, **kw_args) :
		super(Nysol_Mvreplace,self).__init__("mvreplace",nutil.args2dict(args,kw_args,Nysol_Mvreplace.kwd))

