#!/bin/sh

cd /home/pi/github/AWSC_WW2

tmux new -s tail_log -d
tmux send-keys -t tail_log 'tail -f 2017_3_21_WWT-Pressure.txt' C-m
tmux split-window -v -t tail_log
tmux send-keys -t tail_log 'tail -f 2017_3_21_WWT-TankLevels.txt' C-m
tmux attach -t tail_log

