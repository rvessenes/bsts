# bsts
:exclamation: This is a read-only mirror of the CRAN R package repository.  bsts — Bayesian Structural Time Series  

Non-CRAN version that has reproducible predictions.

For package installation only, download bsts_0.7.1.1.tar.gz in the build folder.  If you already have bsts installed, replace it with this version via:

~~~~ 
remove.packages("bsts")
# assumes working directory is whre file is located
install.packages("bsts_0.7.1.1.tar.gz", repos=NULL, tyype="source")
~~~~

If you do not have bsts installed, please install it first to ensure all dependancies are there.  (This may require installing Rtools, Boom, and BoomSpikeSlab individually.)

This package only modifies the predict function from bsts, all code should work as is.
