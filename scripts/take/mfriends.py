#!/usr/bin/env python
# -*- coding: utf-8 -*-
import sys
import nysol.util.margs as margs 
import nysol.take.mfriends as mfriends

args=margs.Margs(sys.argv,"ni=,nf=,ei=,ef=,eo=,no=,sim=,rank=,dir=,-directed,-udout","ei=,ef=,sim=")
mfriends.mfriends(args).run()
