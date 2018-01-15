------------------------
Peptimapper Version 1.0
------------------------
From the use of MS/MS analysis data, the workflow has to : 1) confirm genes predictions ; 2) annotate new CDS; 3) correct intron-exon junctions. First, Peptide Sequence Tags (PST) are generated from MS/MS spectra analysis (MGF files) . A PST is defined as a short peptide sequence (3 to 5 amino acids) flanked by two masses corresponding to the two adjacent polypeptides. We align PSTs in the six reading frames. A hit is the location of a PST on the sequence. We clusterize hits according to parameters. Clusters obtained are evaluated and compared with the genes predictions produced by the conventional genome annotation.

The overall Peptimapper workflow is composed of 4 binaries : LX_RunPepNovo, LXRunPepMatch, LXQualify, ClustToGff.

-1-  LXRunPepNovo : takes spectra file(s) as input (dta,mgf and mzXML files) and provides a PSTs file as output.

	run PepNovo_bin with default options : -model CID_IT_TRYP and -PTMs C+57:M+16 and convert output format to be compatible with PMMatch.
	-------------------
	# usage: LXRunPepNovo [-v] [-h] [-t taglen] [-n numsol] mgf_or_pkl_files+
	# usage: -v : verbose
	# usage: -h : print this help and exit
	# usage: -t : set tag length (default 3) (only lengths 3-6 are allowed)
	# usage: -n : set max number of solutions (default 10)

-2-  LXRunPepMatch : takes PST file(s) and fasta genome sequence as input and provides a hits file and clusters file as output. 

	LXRunPepMatch run successively 3 scripts PMtrans, PMMatch, and PMClust :
	- PMTrans : will translate a DNA file (fasta format) into six-frame translations for further use by PMMatch
	- PMMatch : match PSTs found by LXRunPepNovo on the six-frame translation of DNA sequence(s). A hit correspond to the location of a PST on a translated sequence.
		with following default options : -n 1 (maximum number of modified residues), -m 0 (maximum number of miscleavage(s)), -a /EctoLine/scripts/data/aa_mono.ref (aminoacid reference file), -e /EctoLine/scripts/data/trypsine.ref (digestion enzyme reference file)
	- PMClust : clusterizes hits into clusters and projects hits position back to the original (untranslated) DNA sequence.

	-------------------
	# usage: LXRunPepMatch [-v] [-h] [-f] [-F] [-D tolerance] [-t minhit] [-T minpep] [-d dist] chromo_fasta tag_file+
	# usage: -v : verbose
	# usage: -h : print this help and exit
	# usage: -f : force recompute chromosome translation
	# usage: -F : force recompute PMMatch
	# usage: -D : set mass tolerance (default 0.5)
	# usage: -t : set min number of hit per cluster (default 3)
	# usage: -T : set min number of peptide per cluster (default 2)
	# usage: -d : set clustering distance (default 5000)

-3- LXQualify : takes clusters file(s) and GFF directory as input and provides annotated or unannotated clusters file as output

	-------------------
	#usage: LXQualify [-v] [-h] [-f] cluster_file gff_directory
	#usage: -v : verbose
	#usage: -h : print this help and exit
	#usage: -f : force recompute chromosome translation


-4- LXClust2Gff : process all cluster_files, and produce gff files for each clusters file to be integrated into a genome viewer.

	-------------------
	# usage: LXClust2Gff [-v] [-h] gff_directory output_directory cluster_file+
	# usage: -v : verbose
	# usage: -h : print this help and exit


Note that all programs take an optional -h argument, providing help on program usage



----------------------------------------------------
For any information about Peptimapper, please send an email to :
laetitia.guillot@univ-rennes1.fr
  
