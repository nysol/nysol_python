# -*- coding: utf-8 -*-
import _nysolshell_core as n_core
from nysolmod.nysollib.nysolmodcore import NysolMOD_CORE
import nysolmod.nysollib.nysolutil as nutil

class Nysol_Msel(NysolMOD_CORE):
	kwd = n_core.getparalist("msel")
	def __init__(self,*args, **kw_args) :
		super(Nysol_Msel,self).__init__("msel",nutil.args2dict(args,kw_args,Nysol_Msel.kwd))

