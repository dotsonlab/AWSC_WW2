#!/bin/python

import csv
import datetime as dt
import matplotlib

import matplotlib.pyplot as plt
import matplotlib.dates as md

# uncomment when on lab server
# matplotlib.use('Agg')
import matplotlib.pyplot as plt
from datetime import date, timedelta, datetime
import os

# uncomment when on lab server
os.chdir('/Users/addotson/Desktop/python/data/')

today = date.today() - timedelta(days=1)
year = today.year
month = today.month
day = today.day

# lab server working directory /home/addotson/data_AWSC/

filename1 = "/Users/addotson/Desktop/python/data/{0}_{1}_{2}_WWT-Pressure".format(year, month, day)
filename2 = "/Users/addotson/Desktop/python/data/{0}_{1}_{2}_WWT-TandPD".format(year, month, day)
filename3 = "/Users/addotson/Desktop/python/data/{0}_{1}_{2}_WWT-TankLevels".format(year, month, day)
filename4 = "/Users/addotson/Desktop/python/data/{0}_{1}_{2}_WWT-iFlow".format(year, month, day)
filename5 = "/Users/addotson/Desktop/python/data/{0}_{1}_{2}_WWT-tFlow".format(year, month, day)

####plots pressure
x,y = [],[]
x1,y = [],[]
x2,y = [],[]
x3,y = [],[]
x4,y = [],[]

csv_reader1 = csv.reader(open(filename1+".txt"), delimiter="\t")
next(csv_reader1) #ignore headers
for line in csv_reader1:
    x.append(float(line[0]))
    x1.append(float(line[1]))
    x2.append(float(line[2]))
    x3.append(float(line[3]))
    x4.append(float(line[4]))
    y.append(dt.datetime.fromtimestamp(float(line[5])))

fig = plt.figure()
ax = fig.add_subplot(111)
ax.plot(y,x,'ro',label='F',markeredgewidth=0.0)
ax.plot(y,x1,'bo',label='C1',markeredgewidth=0.0)
ax.plot(y,x2,'go',label='C2',markeredgewidth=0.0)
ax.plot(y,x3,'co',label='NFR',markeredgewidth=0.0)
ax.plot(y,x4,'mo',label='ROR',markeredgewidth=0.0)

xfmt = md.DateFormatter('%Y-%m-%d %H:%M:%S')
plt.xticks( rotation=25 )
ax.xaxis.set_major_formatter(xfmt)
#fig.autofmt_xdate()

title1 = '%i/%i/%i %s' % (month,day,year,'Pressures')
fig.suptitle(title1, fontsize=20)
plt.xlabel('Time (HH:MM:SS)', fontsize=18)
plt.ylabel('Tank Volume (gal)', fontsize=18)

# Now add the legend with some customizations.
legend = ax.legend(loc='lower left', shadow=True)
# The frame is matplotlib.patches.Rectangle instance surrounding the legend.
frame = legend.get_frame()
frame.set_facecolor('0.90')
# Set the fontsize
for label in legend.get_texts():
    label.set_fontsize('large')

for label in legend.get_lines():
    label.set_linewidth(1.5)  # the legend line width

plt.savefig(filename1+".png")
plt.show()

####plots temp
x,y = [],[]
x1,y = [],[]
x2,y = [],[]

csv_reader2 = csv.reader(open(filename2+".txt"), delimiter="\t")
next(csv_reader2) #ignore headers
for line in csv_reader2:
    x.append(float(line[0]))
    x1.append(float(line[1]))
    x2.append(float(line[2]))
    y.append(dt.datetime.fromtimestamp(float(line[5])))

fig = plt.figure()
ax = fig.add_subplot(111)
ax.plot(y,x,'ro',label='OUT', markeredgewidth=0.0)
ax.plot(y,x1,'bo',label='AC', markeredgewidth=0.0)
ax.plot(y,x2,'go',label='DC', markeredgewidth=0.0)

xfmt = md.DateFormatter('%Y-%m-%d %H:%M:%S')
plt.xticks( rotation=25 )
ax.xaxis.set_major_formatter(xfmt)
#fig.autofmt_xdate()

title1 = '%i/%i/%i %s' % (month,day,year,'Temp')
fig.suptitle(title1, fontsize=20)
plt.xlabel('Time (HH:MM:SS)', fontsize=18)
plt.ylabel('Temp (C)', fontsize=18)

# Now add the legend with some customizations.
legend = ax.legend(loc='lower left', shadow=True)
# The frame is matplotlib.patches.Rectangle instance surrounding the legend.
frame = legend.get_frame()
frame.set_facecolor('0.90')
# Set the fontsize
for label in legend.get_texts():
    label.set_fontsize('large')

for label in legend.get_lines():
    label.set_linewidth(1.5)  # the legend line width

plt.savefig(filename2+"-T.png")
plt.show()


####plots power
x3,y = [],[]
x4,y = [],[]

csv_reader2 = csv.reader(open(filename2+".txt"), delimiter="\t")
next(csv_reader2) #ignore headers
for line in csv_reader2:
    x3.append(float(line[3]))
    x4.append(float(line[4]))
    y.append(dt.datetime.fromtimestamp(float(line[5])))

fig = plt.figure()
ax = fig.add_subplot(111)
ax.plot(y,x3,'co',label='PWRA',markeredgewidth=0.0)
ax.plot(y,x4,'mo',label='PWRB',markeredgewidth=0.0)

xfmt = md.DateFormatter('%Y-%m-%d %H:%M:%S')
plt.xticks( rotation=25 )
ax.xaxis.set_major_formatter(xfmt)
#fig.autofmt_xdate()

title1 = '%i/%i/%i %s' % (month,day,year,'Power')
fig.suptitle(title1, fontsize=20)
plt.xlabel('Time (HH:MM:SS)', fontsize=18)
plt.ylabel('Power (A)', fontsize=18)

# Now add the legend with some customizations.
legend = ax.legend(loc='lower left', shadow=True)
# The frame is matplotlib.patches.Rectangle instance surrounding the legend.
frame = legend.get_frame()
frame.set_facecolor('0.90')
# Set the fontsize
for label in legend.get_texts():
    label.set_fontsize('large')

for label in legend.get_lines():
    label.set_linewidth(1.5)  # the legend line width

plt.savefig(filename2+"-P.png")
plt.show()



####plots Tank Levels
x,y = [],[]
x1,y = [],[]
x2,y = [],[]
x3,y = [],[]
x4,y = [],[]

csv_reader3 = csv.reader(open(filename3+".txt"), delimiter="\t")
next(csv_reader3) #ignore headers
for line in csv_reader3:
    x.append(float(line[0]))
    x1.append(float(line[1]))
    x2.append(float(line[2]))
    x3.append(float(line[3]))
    x4.append(float(line[4]))
    y.append(dt.datetime.fromtimestamp(float(line[5])))

fig = plt.figure()
ax = fig.add_subplot(111)
ax.plot(y,x,'ro',label='WW',markeredgewidth=0.0)
ax.plot(y,x1,'bo',label='ROF',markeredgewidth=0.0)
ax.plot(y,x2,'go',label='NFF',markeredgewidth=0.0)
ax.plot(y,x3,'co',label='GW',markeredgewidth=0.0)
ax.plot(y,x4,'mo',label='Waste',markeredgewidth=0.0)

xfmt = md.DateFormatter('%Y-%m-%d %H:%M:%S')
plt.xticks( rotation=25 )
ax.xaxis.set_major_formatter(xfmt)
#fig.autofmt_xdate()

title1 = '%i/%i/%i %s' % (month,day,year,'Tank Levels')
fig.suptitle(title1, fontsize=20)
plt.xlabel('Time (HH:MM:SS)', fontsize=18)
plt.ylabel('Tank Volume (gal)', fontsize=18)

# Now add the legend with some customizations.
legend = ax.legend(loc='lower left', shadow=True)
# The frame is matplotlib.patches.Rectangle instance surrounding the legend.
frame = legend.get_frame()
frame.set_facecolor('0.90')
# Set the fontsize
for label in legend.get_texts():
    label.set_fontsize('large')

for label in legend.get_lines():
    label.set_linewidth(1.5)  # the legend line width

plt.savefig(filename3+".png")
plt.show()

####plots instantaneous flows
x,y = [],[]
x1,y = [],[]
x2,y = [],[]
x3,y = [],[]
x4,y = [],[]

csv_reader4 = csv.reader(open(filename4+".txt"), delimiter="\t")
next(csv_reader4) #ignore headers
for line in csv_reader4:
    x.append(float(line[0]))
    x1.append(float(line[1]))
    x2.append(float(line[2]))
    x3.append(float(line[3]))
    x4.append(float(line[4]))
    y.append(dt.datetime.fromtimestamp(float(line[5])))

fig = plt.figure()
ax = fig.add_subplot(111)
ax.plot(y,x,'ro',label='C',markeredgewidth=0.0)
ax.plot(y,x1,'bo',label='NFP',markeredgewidth=0.0)
ax.plot(y,x2,'go',label='NFR',markeredgewidth=0.0)
ax.plot(y,x3,'co',label='ROP',markeredgewidth=0.0)
ax.plot(y,x4,'mo',label='ROR',markeredgewidth=0.0)

xfmt = md.DateFormatter('%Y-%m-%d %H:%M:%S')
plt.xticks( rotation=25 )
ax.xaxis.set_major_formatter(xfmt)
#fig.autofmt_xdate()

title1 = '%i/%i/%i %s' % (month,day,year,'Instantaneous Flow')
fig.suptitle(title1, fontsize=20)
plt.xlabel('Time (HH:MM:SS)', fontsize=18)
plt.ylabel('Flow Rate (gal/min)', fontsize=18)

# Now add the legend with some customizations.
legend = ax.legend(loc='lower left', shadow=True)
# The frame is matplotlib.patches.Rectangle instance surrounding the legend.
frame = legend.get_frame()
frame.set_facecolor('0.90')
# Set the fontsize
for label in legend.get_texts():
    label.set_fontsize('large')

for label in legend.get_lines():
    label.set_linewidth(1.5)  # the legend line width

plt.savefig(filename4+".png")
plt.show()

#
# #### make figure pair
# os.system("python merge.py "+filename1+".png "+filename1+"t.png")
# finalimage = '%i-%i-%i-%s' % (year,month,day,'environment')
# # lab server working directory /home/addotson/data_AWSC/
# os.system("mv out.png /home/addotson/data_AWSC/daily_visual/"+finalimage+".png")
# os.system("rm "+filename1+".png "+filename1+"t.png")
