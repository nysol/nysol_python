#!/usr/bin/env python
# -*- coding: utf-8 -*-
import sys
import nysol.util.margs as margs 
import nysol.take as nt

args=margs.Margs(sys.argv,"i=,no=,eo=,log=,tid=,item=,s=,S=,sim=,th=,-node_support,T=,-num","i=,tid=,item=,eo=")
nt.mtra2gc(**(args.kvmap())).run()


