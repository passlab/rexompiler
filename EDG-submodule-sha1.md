4.52988e64da30f4e595db6588174f12a7a909da29

## Wordaround for dealing with EDG issue. 
This file is used to specify a hash value for downloading EDG binary
from rose distribution at http://rosecompiler.org/edg_binaries.

If this repo is updated with the official ROSE from github at 
https://github.com/rose-compiler/rose-develop, we will need to check 
http://rosecompiler.org/edg_binaries to find the hash value of the
EDG tarball we want. Basically, we check the timestampe of the file to
find the tarball we need. 

For example, if we find the following EDG is what we need, 
`roseBinaryEDG-4-12-x86_64-pc-linux-gnu-gnu-5.4-4.26657d773d6dd73dac6875dbf76015694924aa4d.tar.gz	2018-04-11 15:22	12M`

We will copy the hash key to the first line of this file. The hash key is:
4.26657d773d6dd73dac6875dbf76015694924aa4d. 

By this, we tell the ROSE build system that the above version is needed. 

## How this works
This file is used by scripts/edg-generate-sig script to get the hash value
for downloading that roseBinaryEDG file. The ROSE build system will automatically
download the one that match your compiler version and system. 

We only need this when we need to frequently change ROSE parser related files that
may require (falsely) new version of EDG. If we do not need this feature, we need
to revert the changes made for this and the scripts/edg-generate-sig script. 
