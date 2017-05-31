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

# check the output folder
outfolder='/home/yzhang/workspace/ActivitySegmentation/Data_MPIICooking/' + sys.argv[2]
if not os.path.isdir(outfolder):
    os.makedirs(outfolder)
else:
    print 'Notice: the filder ' + sys.argv[2] + ' already exists.\n'

# write the output file
text_file = open('MPIICooking_' + sys.argv[2] + '_output_list_video.txt', 'w')

prefix = outfolder + '/' 


for ii in range(0, n_frames,S):
    text_file.write(prefix+'%06d\n'%ii)
text_file.close()


# check whether the model is existing
if not os.path.isfile('../conv3d_deepnetA_sport1m_iter_1900000'):
    proc = subprocess.Popen(['wget http://vlg.cs.dartmouth.edu/c3d/conv3d_deepnetA_sport1m_iter_1900000 -O ../conv3d_deepnetA_sport1m_iter_1900000'], shell=True,stdout=subprocess.PIPE)
else:
    print 'Notice: the model ../conv3d_deepnetA_sport1m_iter_1900000 exists. \n'
proc.stdout.close()

    

