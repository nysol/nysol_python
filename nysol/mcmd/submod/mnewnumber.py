# -*- coding: utf-8 -*-
import nysol._nysolshell_core as n_core
from nysol.mcmd.nysollib.core import NysolMOD_CORE
from nysol.mcmd.nysollib import nysolutil as nutil

class Nysol_Mnewnumber(NysolMOD_CORE):
	_kwd = n_core.getparalist("mnewnumber")
	_outkwd = ["o="]
	def __init__(self,*args, **kw_args) :
		super(Nysol_Mnewnumber,self).__init__("mnewnumber",nutil.args2dict(args,kw_args,Nysol_Mnewnumber._kwd))

