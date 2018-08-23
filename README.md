# nysol_python
NYSOL (read as nee-sol) is a generic name of software tools and project activities designed for supporting big data analysis. The tools are developed from various large-scale data analysis and university research projects. NYSOL consists of two packages so far, namely MCMD and TAKE.

MCMD is a set of methods that are developed for the purpose of high-speed processing of large-scale structured tabular data in Python Lists or CSV file. Each method reads the tabular data from standard input, and carry out a very simple processing method (like sort, sum-up, join, merge, etc.) and write the results to standard output. It achieves a variety of processing functions by connecting individual methods with an inter-thread stream called "pipe", thus the output of each process feeds directly as input to the next one. It is possible to efficiently process a large scale of data with hundred millions row of records on a standard PC.

TAKE (pronunciation is "ta-ke", not common "take", stand for the name of developer) is a library for data mining consisting of a varaety of methods, like itemset mining, sequecial pattern mining, clique enumeration, graph polishing, etc. This package developed at the center led by Professor Takeaki Uno (National Institute of Informatics in Japan). TAKE wrapped his original software in Python.

NYSOL runs in UNIX environment (Linux and Mac OS X, not Windows).
