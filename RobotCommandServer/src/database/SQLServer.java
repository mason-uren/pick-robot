package database;

import java.sql.Connection;
import java.sql.DriverManager;
import java.sql.ResultSet;
import java.sql.SQLException;
import java.sql.Statement;

import com.mysql.jdbc.Driver;

import robot.RobotInterface.RobotCommands;
import robot.StationInformation;
import utils.MACAddress;
import utils.TimestampPrint;

public class SQLServer implements SQLServerInterface {
    public final static int TIMEOUT = 1000;
    private Connection sqlServer;
    boolean debug = false;

    static RobotCommands commandsMap[] = RobotCommands.values();
    static java.sql.Timestamp lastUpdateTime = new java.sql.Timestamp(System.currentTimeMillis());

    public SQLServer() {

    }

    @Override
    public void connectToDB() throws SQLException {
        try {
            if (isConnectionAlive()) {
                closeConnection();
            }
            Driver driver = new com.mysql.jdbc.Driver();
            DriverManager.registerDriver(driver);
            sqlServer = DriverManager.getConnection(
                    "jdbc:mysql://fr-systems-stg.cluster-c9cd6kcgtc1a.us-west-1.rds.amazonaws.com/robot-test",
                    "robot-test", "2yMN3LXejVxy");
        } catch (SQLException e) {
            e.printStackTrace();
            System.err.println("Database connection failed. Exiting...");
            System.exit(1);
        }

    }

    @Override
    public void closeConnection() throws SQLException {
        if (!sqlServer.isClosed()) {
            sqlServer.close();
        }
    }

    public void getRobotInfo() throws SQLException {
        Statement statement = null;
        ResultSet rs = null;
        String query = "SELECT * FROM Stations LEFT JOIN Robots ON Stations.robot_id = Robots.id WHERE mac_addr = '"
                + MACAddress.getMACAddress() + "'";
        if (debug)
            TimestampPrint.println(query);
        try {
            statement = sqlServer.createStatement();
            statement.setQueryTimeout(TIMEOUT);
            statement.execute(query);
            rs = statement.getResultSet();
            if (rs != null && rs.next()) {
                //has results
                StationInformation.setLineId(rs.getInt("line_id"));
                StationInformation.setStationId(rs.getInt("line_station_id"));
            } else {
                //no results
            }
        } finally {
            closeStatementsAndResults(statement, rs);
        }
    }

    public RobotCommands getRobotCommand() throws SQLException {
        Statement statement = null;
        ResultSet rs = null;

        String query = "SELECT * FROM `Lines` WHERE id = 1";
        if (debug)
            TimestampPrint.println(query);
        try {
            statement = sqlServer.createStatement();
            statement.setQueryTimeout(TIMEOUT);
            statement.execute(query);
            rs = statement.getResultSet();
            if (rs != null && rs.next()) {
                //has results
                java.sql.Timestamp updateTime = rs.getTimestamp("update_time");
                if (updateTime.equals(lastUpdateTime)) {
                    return RobotCommands.COMMAND_NONE;
                }
                lastUpdateTime = updateTime;
                int command = rs.getInt("current_command");
                if (command >= 0 && command < commandsMap.length) {
                    System.out
                            .println("Received command: " + command + ": " + commandsMap[command]);
                    return commandsMap[command];
                } else {
                    System.out.println("Received invalid command: " + command);
                }
            } else {
                return RobotCommands.COMMAND_NONE;
            }
        } finally {
            closeStatementsAndResults(statement, rs);
        }
        return RobotCommands.COMMAND_NONE;
    }

    @Override
    public void itemPickPushEvent(int stationID, long pickCompletionTime, long pickCount)
            throws SQLException {
        String query = "insert into pick_events (stationID, pickCount, pickCompletionTime) values (\""
                + stationID + "\", \"" + pickCount + "\", \"" + pickCompletionTime + "\")";
        if (debug)
            TimestampPrint.println(query);
        sqlServer.createStatement().execute(query);
    }

    public void updateStatus(String status) throws SQLException {
        String[] split = status.split(" ");
        String query = "UPDATE Stations SET status = '" + split[0].trim() + "', items_picked = "
                + split[1].trim() + " WHERE line_id = " + StationInformation.getLineId()
                + " AND line_station_id = " + StationInformation.getStationId();
        if (debug)
            TimestampPrint.println(query);
        Statement statement = sqlServer.createStatement();
        statement.execute(query);
        statement.close();
    }

    @Override
    public boolean isConnectionAlive() throws SQLException {
        return sqlServer != null && sqlServer.isValid(1000);
    }

    private void closeStatementsAndResults(Statement statement, ResultSet rs) {
        try {
            if (rs != null)
                rs.close();
        } catch (Exception e) {
            TimestampPrint.printStackTrace(e);
        }

        try {
            if (statement != null)
                statement.close();
        } catch (Exception e) {
            TimestampPrint.printStackTrace(e);
        }
    }
}
