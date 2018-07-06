#!/usr/bin/env python
# -*- coding: utf-8 -*-
import sys
import nysol.util.margs as margs 
import nysol.take as nt

args=margs.Margs(sys.argv,"ni=,nf=,ei=,ef=,eo=,no=,sim=,rank=,dir=,-directed,-udout","ei=,ef=,sim=")
nt.mfriends(**(args.kvmap())).run()
