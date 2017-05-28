#!/usr/bin/python
import sys, os
import subprocess
import numpy as np

if len(sys.argv) != 4:
    print "please type: $script.py filepath filename stride"
    sys.exit()

S=int(sys.argv[3]) # stride

# read number of frames using ffprobe
filename = sys.argv[1] + '/' + sys.argv[2]
proc = subprocess.Popen(['ffprobe -i '+filename+' -hide_banner -show_streams'], shell=True,stdout=subprocess.PIPE)

proc2 = subprocess.Popen(['grep  nb_frames'], shell=True,stdin=proc.stdout, stdout=subprocess.PIPE, stderr=subprocess.PIPE)

proc.stdout.close()
out,err=proc2.communicate()
proc2.stdout.close()
n_frames=int(filter(str.isdigit, out))-16 #16 = snippet in the C3D model. Otherwise, error in the c3d program



# write the input file
text_file = open('MPIICooking_' + sys.argv[2] + '_input_list_video.txt', 'w')
for ii in range(0, n_frames,S):
    text_file.write(filename+' '+str(ii)+' '+ str(0) + '\n')
text_file.close()

# write the output file
text_file = open('MPIICooking_' + sys.argv[2] + '_output_list_video.txt', 'w')

prefix ='output/'+ sys.argv[2] + '/' 


for ii in range(0, n_frames,S):
    text_file.write(prefix+'%06d\n'%ii)
text_file.close()

# check the necessary directories
outfolder='../output/' + sys.argv[2]
if not os.path.isdir(outfolder):
    os.makedirs(outfolder)
else:
    print('Notice: the filder ' + sys.argv[2] + ' already exists.\n')










