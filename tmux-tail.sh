#!/bin/sh

$DATE = date "+%Y_%-m_%-d"

cd /home/pi/github/AWSC_WW2

tmux new -s tail_log -d
tmux send-keys -t tail_log 'tail -f "$DATE"_WWT-Pressure.txt' C-m
tmux split-window -v -t tail_log
tmux send-keys -t tail_log 'tail -f "$DATE"_WWT-TankLevels.txt' C-m
tmux split-window -v -t tail_log
tmux send-keys -t tail_log 'tail -f "$DATE"_WWT-iFlow.txt' C-m
tmux split-window -v -t tail_log
tmux send-keys -t tail_log 'tail -f "$DATE"_WWT-Relays.txt' C-m
tmux split-window -h -t tail_log
tmux send-keys -t tail_log 'tail -f "$DATE"_WWT-TandPD.txt' C-m
tmux split-window -v -t tail_log
tmux select-layout -t tail_log tiled
tmux attach -t tail_log

#to quit:  
# ctrl-b then :
# type kill-session
