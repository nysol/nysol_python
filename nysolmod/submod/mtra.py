# -*- coding: utf-8 -*-
import _nysolshell_core as n_core
from nysolmod.nysollib.nysolmodcore import NysolMOD_CORE
import nysolmod.nysollib.nysolutil as nutil

class Nysol_Mtra(NysolMOD_CORE):
	kwd = n_core.getparalist("mtra")
	def __init__(self,*args, **kw_args) :
		super(Nysol_Mtra,self).__init__("mtra",nutil.args2dict(args,kw_args,Nysol_Mtra.kwd))

