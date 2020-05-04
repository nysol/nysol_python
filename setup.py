from setuptools import setup, find_packages, Extension
import re
import os
import subprocess

args = ['xml2-config','--cflags']
xmlcflg= subprocess.check_output(args).decode().rstrip().split()
args = ['xml2-config','--libs']
xmllibs= subprocess.check_output(args).decode().rstrip().split()

hedears = ['src','src/kgmod','src/mod']

def checkLibRun(cc,fname,paras):
	for para in paras:
		try:
			with open(os.devnull, 'w') as fnull:
				exit_code = subprocess.call(cc + [ fname , "-l"+para],
                                    stdout=fnull, stderr=fnull)
		except OSError :
			exit_code = 1

		if exit_code == 0:
			return para

	return ""

def check_for_boost():

	import sysconfig
	import tempfile
	import shutil
	
	# Create a temporary directory
	tmpdir = tempfile.mkdtemp()
	curdir = os.getcwd()
	os.chdir(tmpdir)

	compiler = os.environ.get('CC', sysconfig.get_config_var('CC'))

	# make sure to use just the compiler name without flags
	compiler = compiler.split()
	filename = 'test.cpp'
	with open(filename,'w') as f :
		f.write("""
			int main ()
			{
				return main ();
			  return 0;
			}
			""")

	boostFLG =[]
	boostLibLists=[
		['boost_thread-mt','boost_thread'],
		['boost_filesystem-mt','boost_filesystem'],
		['boost_regex-mt','boost_regex'],
		['boost_system-mt','boost_system'],
		['boost_date_time-mt','boost_date_time']
	]
	for boostlist in boostLibLists:

		boostflg = checkLibRun(compiler,filename, boostlist)
		if boostflg =="" : 	
			#err
			raise Exception("not find "+ " or ".join(boostlist))

		boostFLG.append(boostflg)

	# Clean up
	os.chdir(curdir)
	shutil.rmtree(tmpdir)

	return boostFLG

import sys
from distutils.util import get_platform
plat_name = get_platform()
plat_specifier = ".%s-%d.%d" % (plat_name, *sys.version_info[:2])
build_temp = os.path.join('build', 'temp' + plat_specifier)

def _get_obj_path(src_path):
	tmp_path = os.path.join(build_temp, src_path)
	filename = os.path.splitext(os.path.basename(tmp_path))[0] + '.o'
	return os.path.join(os.path.dirname(tmp_path), filename)

def exclude_no_need_compile_sources(sources):
	"""
	delete unnecessary source
	by comparing timestamp with one of object file

	return tuple which consists of 
	compiling file names and excluded file names
	"""

	compiling = []
	excluded = []

	for src in sources:
		src_path = os.path.join('src', src)
		src_last_updated_at = os.stat(src_path).st_mtime
		obj_path = _get_obj_path(src_path)

		if os.path.exists(obj_path):
			obj_last_updated_at = os.stat(obj_path).st_mtime
			if src_last_updated_at > obj_last_updated_at:
				compiling.append(src_path)
			else:
				excluded.append(obj_path)
		else:
			compiling.append(src_path)

	return compiling, excluded


nmodLibs = ['pthread']
nmodLibs.extend(check_for_boost())
umodLibs = ['pthread']
umodLibs.extend(check_for_boost())
mmodLibs = ['pthread']
mmodLibs.extend(check_for_boost())


sources_core, extra_objects_core = exclude_no_need_compile_sources([
	'mod/kgsortchk.cpp', 'mod/kg2tee.cpp', 'mod/kgfifo.cpp', 'mod/kgtrafld.cpp',
	'kgexcmd.cpp', 'nysolshell.cpp', 'mod/kg2cat.cpp',
	'kgshell.cpp', 'kgsplitblock.cpp', 'kgmsgincpysys.cpp',
	'kgload.cpp', 'kgpyfunc.cpp',
	'kgmod/kgArgFld.cpp',
	'kgmod/kgArgs.cpp', 'kgmod/kgCSV.cpp',
	'kgmod/kgCSVout.cpp', 'kgmod/kgCSVutils.cpp',
	'kgmod/kgEnv.cpp', 'kgmod/kgError.cpp',
	'kgmod/kgFldBuffer.cpp', 'kgmod/kgFldHash.cpp',
	'kgmod/kgFunction.cpp', 'kgmod/kgMessage.cpp',
	'kgmod/kgMethod.cpp', 'kgmod/kgRange.cpp',
	'kgmod/kgTempfile.cpp', 'kgmod/kgVal.cpp',
	'kgmod/kgWildcard.cpp', 'kgmod/kgmod.cpp',
	'kgmod/kgmodincludesort.cpp', 'kgmod/kgsortf.cpp',
	'mod/kgshuffle.cpp', 'mod/kgunicat.cpp',
	'mod/kgcal.cpp', 'mod/kgcut.cpp', 'mod/kgsum.cpp', 'mod/kgjoin.cpp',
	'mod/kg2cross.cpp', 'mod/kgaccum.cpp', 'mod/kgavg.cpp',
	'mod/kgbest.cpp', 'mod/kgbucket.cpp', 'mod/kgchgnum.cpp',
	'mod/kgchgstr.cpp', 'mod/kgcombi.cpp', 'mod/kgcommon.cpp',
	'mod/kgcount.cpp', 'mod/kgcross.cpp', 'mod/kgdelnull.cpp',
	'mod/kgdformat.cpp', 'mod/kgduprec.cpp', 'mod/kgfldname.cpp',
	'mod/kgfsort.cpp', 'mod/kghashavg.cpp', 'mod/kghashsum.cpp',
	'mod/kgkeybreak.cpp', 'mod/kgmbucket.cpp', 'mod/kgmvavg.cpp',
	'mod/kgmvsim.cpp', 'mod/kgmvstats.cpp', 'mod/kgnewnumber.cpp',
	'mod/kgnewrand.cpp', 'mod/kgnewstr.cpp', 'mod/kgnjoin.cpp',
	'mod/kgnormalize.cpp', 'mod/kgnrcommon.cpp', 'mod/kgnrjoin.cpp',
	'mod/kgnullto.cpp', 'mod/kgnumber.cpp', 'mod/kgpadding.cpp',
	'mod/kgpaste.cpp', 'mod/kgproduct.cpp', 'mod/kgrand.cpp',
	'mod/kgrjoin.cpp', 'mod/kgsed.cpp', 'mod/kgsel.cpp',
	'mod/kgselnum.cpp', 'mod/kgselrand.cpp', 'mod/kgselstr.cpp',
	'mod/kgsetstr.cpp', 'mod/kgshare.cpp', 'mod/kgsim.cpp',
	'mod/kgslide.cpp', 'mod/kgsplit.cpp', 'mod/kgstats.cpp',
	'mod/kgsummary.cpp', 'mod/kgtonull.cpp', 'mod/kgtra.cpp',
	'mod/kgtraflg.cpp', 'mod/kguniq.cpp', 'mod/kgcat.cpp',
	'mod/kgvcat.cpp', 'mod/kgvcommon.cpp', 'mod/kgvcount.cpp',
	'mod/kgvdelim.cpp', 'mod/kgvdelnull.cpp', 'mod/kgvjoin.cpp',
	'mod/kgvnullto.cpp', 'mod/kgvreplace.cpp', 'mod/kgvsort.cpp',
	'mod/kgvuniq.cpp', 'mod/kgwindow.cpp', 'mod/kgsep.cpp', 'mod/kgkmeans.cpp',
	'mod/kgarff2csv.cpp', 'mod/kgtab2csv.cpp', 'mod/kgxml2csv.cpp'
])
module1 = Extension('nysol/_nysolshell_core',
                    sources=sources_core,
					extra_objects=extra_objects_core,
					include_dirs=hedears,
					libraries=nmodLibs,
					extra_compile_args=xmlcflg,
					extra_link_args=xmllibs
					)


sources_lcmmod, extra_objects_lcmmod = exclude_no_need_compile_sources(
	['take/lcmrap.c']
)
lcmmod = Extension('nysol/take/_lcmlib',
                    sources=sources_lcmmod,
					extra_objects=extra_objects_lcmmod,
 					extra_compile_args=['-Wno-format-security', '-Wno-error'],
					include_dirs=['src', 'src/kgmod', 'src/mod', 'src/take/lcm']
					)


sources_seqmod, extra_objects_seqmod = exclude_no_need_compile_sources(
	['take/lcmseqrap.c']
)
seqmod = Extension('nysol/take/_lcmseqlib',
                    sources=sources_seqmod,
					extra_objects=extra_objects_seqmod,
 					extra_compile_args=['-Wno-format-security', '-Wno-error'],
					include_dirs=['src', 'src/kgmod', 'src/mod', 'src/take/lcmseq']
					)


sources_seqmodzero, extra_objects_seqmodzero = exclude_no_need_compile_sources(
	['take/lcmseq_zerorap.c']
)
seqmodzero = Extension('nysol/take/_lcmseq_zerolib',
                    sources=sources_seqmodzero,
					extra_objects=extra_objects_seqmodzero,
 					extra_compile_args=['-Wno-format-security', '-Wno-error'],
					include_dirs=['src', 'src/kgmod', 'src/mod', 'src/take/lcmseq_zero']
					)


sources_lcmtransmod, extra_objects_lcmtransmod = exclude_no_need_compile_sources(
	['take/lcmtransrun.cpp']
)
lcmtransmod = Extension('nysol/take/_lcmtranslib',
                    sources=sources_lcmtransmod,
					extra_objects=extra_objects_lcmtransmod,
 					extra_compile_args=['-Wno-format-security', '-Wno-error'],
					include_dirs=['src', 'src/kgmod', 'src/mod']
					)


sources_mmaketramod, extra_objects_mmaketramod = exclude_no_need_compile_sources([
	'take/mmaketrarun.cpp',
	'take/mmaketra/kgmaketra.cpp',
	'kgmod/kgArgFld.cpp',
	'kgmod/kgArgs.cpp', 'kgmod/kgCSV.cpp',
	'kgmod/kgCSVout.cpp', 'kgmod/kgCSVutils.cpp',
	'kgmod/kgEnv.cpp', 'kgmod/kgError.cpp',
	'kgmod/kgFldBuffer.cpp',
	'kgmod/kgMessage.cpp',
	'kgmod/kgMethod.cpp',
	'kgmod/kgTempfile.cpp', 'kgmod/kgVal.cpp',
	'kgmod/kgWildcard.cpp', 'kgmod/kgmod.cpp',
	'kgmod/kgmodincludesort.cpp', 'kgmod/kgsortf.cpp'              
])
mmaketramod = Extension('nysol/take/_mmaketralib',
						sources=sources_mmaketramod,
						extra_objects=extra_objects_mmaketramod,
						extra_compile_args=['-Wno-format-security', '-Wno-error'],
						include_dirs=['src', 'src/kgmod', 'src/mod', 'src/take/mmaketra'],
						libraries=mmodLibs
						)


sources_sspcmod, extra_objects_sspcmod = exclude_no_need_compile_sources(
	['take/sspcrap.c']
)
sspcmod = Extension('nysol/take/_sspclib',
                    sources=sources_sspcmod,
					extra_objects=extra_objects_sspcmod,
					extra_compile_args=['-Wno-format-security', '-Wno-error'],
					include_dirs=['src', 'src/kgmod', 'src/mod', 'src/take/sspc']
					)


sources_grhfilmod, extra_objects_grhfilmod = exclude_no_need_compile_sources(
	['take/grhfilrap.c']
)
grhfilmod = Extension('nysol/take/_grhfillib',
                    sources=sources_grhfilmod,
					extra_objects=extra_objects_grhfilmod,
					extra_compile_args=['-Wno-format-security', '-Wno-error'],
					include_dirs=['src', 'src/kgmod', 'src/mod', 'src/take/grhfil']
					)


sources_macemod, extra_objects_macemod = exclude_no_need_compile_sources(
	['take/macerap.c']
)
macemod = Extension('nysol/take/_macelib',
                    sources = sources_macemod,
					extra_objects=extra_objects_macemod,
					extra_compile_args=['-Wno-format-security', '-Wno-error'],
					include_dirs=['src', 'src/kgmod', 'src/mod', 'src/take/mace']
					)


sources_simsetmod, extra_objects_simsetmod = exclude_no_need_compile_sources(
	['take/simsetrap.c']
)
simsetmod = Extension('nysol/take/_simsetlib',
                    sources=sources_simsetmod,
					extra_objects=extra_objects_simsetmod,
					extra_compile_args=['-Wno-format-security', '-Wno-error'],
					include_dirs=['src', 'src/kgmod', 'src/mod', 'src/take/simset']
					)


sources_medsetmod, extra_objects_medsetmod = exclude_no_need_compile_sources(
	['take/medsetrap.c']
)
medsetmod = Extension('nysol/take/_medsetlib',
                    sources=sources_medsetmod,
					extra_objects=extra_objects_medsetmod,
					extra_compile_args=['-Wno-format-security', '-Wno-error'],
					include_dirs=['src', 'src/kgmod', 'src/mod', 'src/take/medset']
					)


sources_utilmod, extra_objects_utilmod = exclude_no_need_compile_sources([
	'util/mmethods.cpp',
	'kgmod/kgArgFld.cpp',
	'kgmod/kgArgs.cpp', 'kgmod/kgCSV.cpp',
	'kgmod/kgCSVout.cpp', 'kgmod/kgCSVutils.cpp',
	'kgmod/kgEnv.cpp', 'kgmod/kgError.cpp',
	'kgmod/kgFldBuffer.cpp',
	'kgmod/kgMessage.cpp',
	'kgmod/kgMethod.cpp',
	'kgmod/kgTempfile.cpp', 'kgmod/kgVal.cpp',
	'kgmod/kgWildcard.cpp', 'kgmod/kgmod.cpp',
	'kgmod/kgmodincludesort.cpp','kgmod/kgsortf.cpp',
	'mod/kgchkcsv.cpp'
])
utilmod = Extension('nysol/util/_utillib',
                    sources=sources_utilmod,
					extra_objects=extra_objects_utilmod,
					include_dirs=['src', 'src/kgmod', 'src/mod'],
					libraries=umodLibs
					)


sources_vsopmod, extra_objects_vsopmod = exclude_no_need_compile_sources([
	'vsop/vsop.cpp',
	'vsop/SAPPOROBDD/src/BDDc/bddc.c',
	'vsop/SAPPOROBDD/src/BDD+/BDD.cc',
	'vsop/SAPPOROBDD/src/BDD+/CtoI.cc',
	'vsop/SAPPOROBDD/src/BDD+/ZBDD.cc',
	'vsop/SAPPOROBDD/src/BDD+/ZBDDDG.cc',
	'vsop/SAPPOROBDD/src/BDDLCM/lcm-vsop.cc',
	'vsop/SAPPOROBDD/app/VSOP/table.cc',
	'vsop/SAPPOROBDD/app/VSOP/print.cc'
])
vsopmod = Extension('nysol/vsop/_vsoplib',
                    sources=sources_vsopmod,
					extra_objects=extra_objects_vsopmod,
					define_macros=[('B_64', None), ('_NO_MAIN_', None)],
					extra_compile_args=['-Wno-format-security', '-Wno-error', '-Wno-narrowing'],
					include_dirs=[
						'src/vsop/SAPPOROBDD/include',
						'src/vsop/SAPPOROBDD/src/BDDLCM/',
						'src/vsop/SAPPOROBDD/app/VSOP'
					]
					)

setup(
	name='nysol',
	packages=[
		'nysol',
		'nysol/util',
		'nysol/mcmd',
		'nysol/mcmd/submod',
		'nysol/mcmd/nysollib',
		'nysol/take',
		'nysol/vsop',
		'nysol/take/lib',
		'nysol/take/lib/base'
	],
	version='0.3.9',
	description='This is nysol tools',
	long_description="""\
NYSOL (read as nee-sol) is a generic name of software tools and project activities designed for supporting big data analysis. The tools are developed from various large-scale data analysis and university research projects. NYSOL consists of two packages so far, namely MCMD and TAKE.

MCMD is a set of methods that are developed for the purpose of high-speed processing of large-scale structured tabular data in Python Lists or CSV file. Each method reads the tabular data from standard input, and carry out a very simple processing method (like sort, sum-up, join, merge, etc.) and write the results to standard output. It achieves a variety of processing functions by connecting individual methods with an inter-thread stream called "pipe", thus the output of each process feeds directly as input to the next one. It is possible to efficiently process a large scale of data with hundred millions row of records on a standard PC.

TAKE (pronunciation is "ta-ke", not common "take", stand for the name of developer) is a library for data mining consisting of a varaety of methods, like itemset mining, sequecial pattern mining, clique enumeration, graph polishing, etc. This package developed at the center led by Professor Takeaki Uno (National Institute of Informatics in Japan). TAKE wrapped his original software in Python.

NYSOL runs in UNIX environment (Linux and Mac OS X, not Windows).
""",
	author='nysol',
	author_email='info@nysol.jp',
	license='AGPL3',
	url='http://www.nysol.jp/',
	classifiers=[ 
		'Development Status :: 4 - Beta',
		'Intended Audience :: Developers',
		'License :: OSI Approved :: GNU Affero General Public License v3',
		'Operating System :: POSIX :: Linux',
		'Operating System :: MacOS :: MacOS X',
		'Programming Language :: C',
		'Programming Language :: C++',
		'Programming Language :: Python :: 3',
		'Topic :: Software Development :: Libraries :: Python Modules',
		'Topic :: Scientific/Engineering :: Information Analysis',
		'Topic :: Scientific/Engineering :: Mathematics',
	],
	install_requires=["psutil"],
	scripts=[
		'scripts/take/mfriends.py', 'scripts/take/mitemset.py',
		'scripts/take/mpolishing.py', 'scripts/take/msequence.py','scripts/take/mccomp.py',
		'scripts/take/mtra2gc.py', 'scripts/take/mpal.py','scripts/take/mgdiff.py',
		'scripts/take/mclique.py', "scripts/take/mbipolish.py", "scripts/take/mbiclique.py"
	],
	ext_modules=[
		module1, lcmmod, sspcmod, grhfilmod, macemod, seqmod,
		seqmodzero, lcmtransmod, macemod, simsetmod, medsetmod, vsopmod,
		utilmod, mmaketramod
	]
)
       
