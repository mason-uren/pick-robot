package line;

import java.sql.SQLException;

import database.SQLServer;

public class RobotModel {

    public enum RobotState {
        PC_VAC_ON,
        PC_VAC_OFF,
        PC_ERROR,
        PC_READY,
        PC_PICK_COMMAND_RECEIVED,
        PC_TARGET_FOUND,
        PC_AT_PICK_POSITION_XY,
        PC_MOVING_ABOVE_PICK,
        PC_AT_PICK_POSITION_XY_ABOVE_Z,
        PC_PROBING,
        PC_HAS_ITEM,
        PC_RAISING_ARM,
        PC_AT_PICK_POSITION_Z_CLEARANCE,
        PC_MOVING_TO_DROPOFF_XY,
        PC_AT_DROPOFF_XY,
        PC_MOVING_TO_DROPOFF_XYZ,
        PC_AT_DROPOFF_XYZ,
        PC_ITEM_PLACED,
        PC_AT_Z_CLEARANCE_RETURN,
        PC_WAIT_FOR_MOTION,
        PC_ZERO_RETURN,
        PC_ZERO_RETURN_WAIT,
        PC_NEEDS_ZERO;

        private RobotState() {

        }
    }

    private java.sql.Timestamp lastUpdate = new java.sql.Timestamp(System.currentTimeMillis());
    private int robotId;
    private RobotState state = RobotState.PC_ERROR;

    public RobotModel(int robotId) {
        this.robotId = robotId;
    }

    public RobotState getState() {
        return state;
    }

    public void setState(RobotState state) {
        this.state = state;
    }

    public int getRobotId() {
        return robotId;
    }

    public void updateStatus(SQLServer sqlServer) throws SQLException {
        String status = sqlServer.getRobotStatus(this);
        try {
            state = RobotState.valueOf(status);
        } catch (NullPointerException | IllegalArgumentException e) {
            state = RobotState.PC_ERROR;
        }
    }

    public java.sql.Timestamp getLastUpdate() {
        return lastUpdate;
    }

    public void setLastUpdate(java.sql.Timestamp lastUpdate) {
        this.lastUpdate = lastUpdate;
    }
}
