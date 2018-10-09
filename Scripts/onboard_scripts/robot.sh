#!/bin/bash
SERVICE_NAME=robotService
PATH_TO_ROBOT=/home/pi/pick-robot
PATH_TO_TRIGGER=/home/pi/pick-trigger-app
ROBOT_PID_PATH_NAME=/tmp/robot-pid
TRIGGER_PID_PATH_NAME=/tmp/trigger-pid
case $1 in
  start)
    echo "Starting $SERVICE_NAME ..."
      if [ ! -f $ROBOT_PID_PATH_NAME ]; then
        $PATH_TO_ROBOT >> /var/log/robot.log 2>&1&
        echo $! > $ROBOT_PID_PATH_NAME
        sleep 1
        $PATH_TO_TRIGGER >> /var/log/robot.log 2>&1&
        echo $! > $TRIGGER_PID_PATH_NAME
        echo "$SERVICE_NAME started ..."
      else
        echo "$SERVICE_NAME is already running ..."
      fi
    ;;
    stop)
         if [ -f $ROBOT_PID_PATH_NAME ]; then
            PID=$(cat $ROBOT_PID_PATH_NAME);
            echo “Robot stoping ..."
            kill $PID;
            echo “Robot stopped ..."
            rm $ROBOT_PID_PATH_NAME
        else
            echo "Robot is not running ..."
        fi
        if [ -f $TRIGGER_PID_PATH_NAME ]; then
            PID=$(cat $TRIGGER_PID_PATH_NAME);
            echo “trigger stoping ..."
            kill $PID;
            echo “trigger stopped ..."
            rm $TRIGGER_PID_PATH_NAME
        else
            echo "trigger is not running ..."
        fi
    ;;
esac
