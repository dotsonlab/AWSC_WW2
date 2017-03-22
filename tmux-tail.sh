#!/bin/sh

TODAY=$(date "+%Y_%-m_%-d")
echo "$TODAY"

cd /home/pi/github/AWSC_WW2

tmux new -s tail_log -d
tmux send-keys -t tail_log "tail -f ${TODAY}_WWT-Pressure.txt"
tmux send-keys -t tail_log C-m
tmux split-window -v -t tail_log
tmux send-keys -t tail_log "tail -f ${TODAY}_WWT-TankLevels.txt"
tmux send-keys -t tail_log C-m
tmux split-window -v -t tail_log
tmux send-keys -t tail_log "tail -f ${TODAY}_WWT-iFlow.txt"
tmux send-keys -t tail_log C-m
tmux split-window -v -t tail_log
tmux send-keys -t tail_log "tail -f ${TODAY}_WWT-Relays.txt"
tmux send-keys -t tail_log C-m
tmux split-window -h -t tail_log
tmux send-keys -t tail_log "tail -f ${TODAY}_WWT-TandPD.txt"
tmux send-keys -t tail_log C-m
tmux split-window -v -t tail_log
tmux select-layout -t tail_log tiled
tmux attach -t tail_log

#to quit:  
# ctrl-b then :
# type kill-session
