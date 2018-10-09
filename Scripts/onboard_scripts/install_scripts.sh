sudo mv robot.service /etc/systemd/system/robot.service
sudo mv robot_log_rotate /etc/logrotate.d/robot_log_rotate
sudo chmod +x robot.sh && sudo mv robot.sh /usr/local/bin/robot.sh
sudo systemctl enable robot.service
echo "Enable hourly logrotate in crontab: sudo crontab -e, enter 0 * * * * logrotate -vf /etc/logrotate.conf"
