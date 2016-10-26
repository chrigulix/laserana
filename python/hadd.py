from subprocess import Popen, PIPE, call
import os
import shutil
import argparse
import glob

import tempfile


parser = argparse.ArgumentParser()
parser.add_argument("files", help="list of files to pass to hadd",  nargs="+")
parser.add_argument("out_file", help="path to outputfile")
parser.add_argument("--chunk_size",  default=10, type=int,  help="size of chunks which are processed at a time")
args = parser.parse_args()


if len(args.files) < 2:
    raise ValueError("need at least two files to add.")
if os.path.isfile(args.out_file):
    raise ValueError("output file exists: " + str(args.out_file))

try:
    call("hadd", stdout=open(os.devnull, 'wb'))
except EnvironmentError as ex:
    raise EnvironmentError( "hadd unavailable, did you set it up via ups?")



def chunker(seq, size):
    return (seq[pos:pos+size] for pos in xrange(0, len(seq), size))


def hadd(files, outfile, tries=5):
    if tries == 0:
        raise IndexError("failed 5 times to add these files: " + str(files))
     
    command = ['hadd', outfile]
    command.extend(files)
    p = Popen(command, stdin=PIPE, stdout=PIPE, stderr=PIPE)

    print "adding to " + outfile
    out, err = p.communicate()
    
    if err:
        # lets retry
        os.remove(outfile)
        hadd(files, outfile, tries=tries-1)


files = args.files
tmp_dir = tempfile.mkdtemp()

step = 0
while len(files) > 1:
    outfiles = []
    for idx, chunk in enumerate(chunker(files, 10)):
        tmp_file = os.path.join(tmp_dir, "tmp-time-" + str(step) + "-" + str(idx) + ".root")
        outfiles.append(tmp_file)
        hadd(chunk, tmp_file)
    
    step += 1
    files = outfiles

out_file = os.path.join(args.out_file)
shutil.copyfile(files[0], out_file)
shutil.rmtree(tmp_dir)
print "finished: sum of files is in " + out_file
