#!/usr/bin/env python
# -*- coding: utf-8 -*-
import sys
import nysol.util.margs as margs
import nysol.take.mbiclique as mbiclique


args=margs.Margs(sys.argv,"ei=,ef=,o=,l=,u=,-edge","ei=")
mbiclique.mclique(args).run()

