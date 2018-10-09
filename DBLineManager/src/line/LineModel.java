package line;

import java.io.IOException;
import java.sql.SQLException;
import java.util.HashMap;
import java.util.Map;

import conveyor.ConveyorController;
import database.SQLServer;
import line.RobotModel.RobotState;

public class LineModel {
    private static final int PICK = 8;
    private static final int DROP = 7;
    private static final int ZERO_NEEDED = 9;
    private SQLServer server;
    private int itemsPicked = 0;
    private int itemsToPick = 0;
    Map<Integer, RobotModel> robots = new HashMap<Integer, RobotModel>();
    boolean pickCommandIssued = false;
    ConveyorController conveyor;

    public LineModel(SQLServer server) {
        this.server = server;
        conveyor = new ConveyorController();
    }

    public int getItemsToPick() {
        return itemsToPick;
    }

    public void setItemsToPick(int itemsToPick) {
        this.itemsToPick = itemsToPick;
    }

    public void setItemsPicked(int itemsPicked) {
        this.itemsPicked = itemsPicked;
    }

    public Map<Integer, RobotModel> getRobots() {
        return robots;
    }

    public void updateRobotStatus() throws SQLException {
        for (RobotModel robot : robots.values()) {
            robot.updateStatus(server);
        }
    }

    public boolean readyForPick() {
        for (RobotModel robot : robots.values()) {
            if (robot.getState() != RobotState.PC_READY) {
                return false;
            }
        }
        return conveyor.isConveyorReady();
    }

    public boolean needsZero() {
        for (RobotModel robot : robots.values()) {
            if (robot.getState() == RobotState.PC_NEEDS_ZERO) {
                return true;
            }
        }
        return false;
    }

    public boolean readyForDrop() {
        for (RobotModel robot : robots.values()) {
            if (robot.getState() != RobotState.PC_AT_DROPOFF_XYZ) {
                return false;
            }
        }
        return conveyor.isConveyorReady();
    }

    public void manageLine() throws SQLException {
        updateRobotStatus();
        server.refreshLineStatus(this);
        if (itemsToPick > itemsPicked) {
            if (!pickCommandIssued) {
                if (needsZero()) {
                    issueCommand(ZERO_NEEDED);
                } else if (readyForPick()) {
                    issueCommand(PICK);
                    try {
                        conveyor.indexConveyor();
                    } catch (IOException e) {
                        System.out.println("Conveyor unavailable");
                    }
                    pickCommandIssued = true;
                } else {
                    //Waiting for states to be good
                }
            } else {
                if (readyForDrop()) {
                    issueCommand(DROP);
                    pickCommandIssued = false;
                    server.updateItemsPicked(++itemsPicked);
                }
            }
        }
    }

    public void issueCommand(int command) throws SQLException {
        server.updateLineCommand(command);
    }

}
