package database;

import java.sql.SQLException;

import robot.RobotInterface.RobotCommands;

public interface SQLServerInterface {
    /**
     * This will disconnect from the current session if it is valid. This will establish a new connection
     * to the database.
     * @throws SQLException Any SQLException is passed through
     */
    public void connectToDB() throws SQLException;

    /**
     * This will disconnect from the current session if it is valid.
     * @throws SQLException Any SQLException is passed through
     */
    public void closeConnection() throws SQLException;

    /**
     * Adds to the database that an item is picked
     * @param id to set
     * @param pick completion time
     * @throws SQLException
     */
    public void itemPickPushEvent(int stationID, long pickCompletionTime, long pickCount)
            throws SQLException;

    /**
     * Check if the connection is alive and active
     * @throws SQLException 
     */
    public boolean isConnectionAlive() throws SQLException;

    /**
     * Gets the command for the robot, given that the line and station information is set.
     * @return
     * @throws SQLException
     */
    public RobotCommands getRobotCommand() throws SQLException;

    /**
     * Gets the information for the robot given the mac address
     * @throws SQLException
     */
    public void getRobotInfo() throws SQLException;

    public void updateStatus(String status) throws SQLException;
}