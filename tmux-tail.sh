#!/bin/sh

$DATE = date "+%Y_%-m_%-d"

cd /home/pi/github/AWSC_WW2

tmux new -s tail_log -d
tmux send-keys -t tail_log 'tail -f $DATE_WWT-Pressure.txt' C-m
tmux split-window -v -t tail_log
tmux send-keys -t tail_log 'tail -f $DATE_WWT-TankLevels.txt' C-m
tmux split-window -v -t tail_log
tmux send-keys -t tail_log 'tail -f $DATE_WWT-iFlow.txt' C-m
tmux split-window -v -t tail_log
tmux send-keys -t tail_log 'tail -f $DATE_WWT-Relays.txt' C-m
tmux split-window -h -t tail_log
tmux send-keys -t tail_log 'tail -f "$DATE_WWT-TandPD.txt' C-m
tmux split-window -v -t tail_log
tmux select-layout -t tail_log tiled
tmux attach -t tail_log

#to quit:  
# ctrl-b then :
# type kill-session
