#!/bin/bash
#for shower

#load this on fixtures to run via cron to scp daily at 12:30AM
#crontab -e   input: 30 0 * * * /home/addotson/TXData.sh

#will determine yesterdays date in the format for the logfile
FILEDATE="$(date "+%Y_%-m_%-d" -d "1 days ago")" #yesterday's date in format for uploading
#FILEDATE="$(date "+%Y_%-m_%-d")" #today's date in format for testing

#will transfer the file to the lab data server
scp /home/pi/github/AWSC_WW2/"$FILEDATE"_WWT-iFlow.txt addotson@137.229.178.173:/home/addotson/data_AWSC/raw/.

scp /home/pi/github/AWSC_WW2/"$FILEDATE"_WWT-Pressure.txt addotson@137.229.178.173:/home/addotson/data_AWSC/raw/.

scp /home/pi/github/AWSC_WW2/"$FILEDATE"_WWT-Relays.txt addotson@137.229.178.173:/home/addotson/data_AWSC/raw/.

scp /home/pi/github/AWSC_WW2/"$FILEDATE"_WWT-TandPD.txt addotson@137.229.178.173:/home/addotson/data_AWSC/raw/.

scp /home/pi/github/AWSC_WW2/"$FILEDATE"_WWT-TankLevels.txt addotson@137.229.178.173:/home/addotson/data_AWSC/raw/.

scp /home/pi/github/AWSC_WW2/"$FILEDATE"_WWT-tFlow.txt addotson@137.229.178.173:/home/addotson/data_AWSC/raw/.

scp /home/pi/github/AWSC_WW2/"$FILEDATE"_WWT-Valves1.txt addotson@137.229.178.173:/home/addotson/data_AWSC/raw/.

scp /home/pi/github/AWSC_WW2/"$FILEDATE"_WWT-Valves2.txt addotson@137.229.178.173:/home/addotson/data_AWSC/raw/.

