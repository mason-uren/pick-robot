package database;

import java.sql.Connection;
import java.sql.DriverManager;
import java.sql.ResultSet;
import java.sql.SQLException;
import java.sql.Statement;
import java.util.Map;

import com.mysql.jdbc.Driver;

import line.LineModel;
import line.RobotModel;
import utils.TimestampPrint;

public class SQLServer {
    public final static int TIMEOUT = 1000;
    private Connection sqlServer;
    boolean debug = true;

    static java.sql.Timestamp lastUpdateTime = new java.sql.Timestamp(0);
    int lineId = 0;

    public SQLServer(int lineId) {
        this.lineId = lineId;
    }

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

    public void closeConnection() throws SQLException {
        if (!sqlServer.isClosed()) {
            sqlServer.close();
        }
    }

    public void updateLineCommand(int command) throws SQLException {
        Statement statement = null;
        String query = "UPDATE `Lines` SET current_command = " + command + " WHERE id = " + lineId;
        if (debug)
            TimestampPrint.println(query);
        try {
            statement = sqlServer.createStatement();
            statement.setQueryTimeout(TIMEOUT);
            statement.execute(query);
        } finally {
            statement.close();
        }
    }

    public void updateItemsPicked(int itemsPicked) throws SQLException {
        Statement statement = null;
        String query = "UPDATE `Lines` SET items_picked = " + itemsPicked + " WHERE id = " + lineId;
        if (debug)
            TimestampPrint.println(query);
        try {
            statement = sqlServer.createStatement();
            statement.setQueryTimeout(TIMEOUT);
            statement.execute(query);
        } finally {
            statement.close();
        }
    }

    public void refreshLineStatus(LineModel line) throws SQLException {
        Statement statement = null;
        ResultSet rs = null;
        String query = "SELECT * FROM `Lines` WHERE id = " + lineId;
        if (debug)
            TimestampPrint.println(query);
        try {
            statement = sqlServer.createStatement();
            statement.setQueryTimeout(TIMEOUT);
            statement.execute(query);
            rs = statement.getResultSet();
            if (rs != null && rs.next()) {
                line.setItemsToPick(rs.getInt("items_to_pick"));
                line.setItemsPicked(rs.getInt("items_picked"));
            } else {
                System.out.println("Error - line does not exist");
                System.exit(1);
            }
        } finally {
            closeStatementsAndResults(statement, rs);
        }
    }

    public void populateRobots(Map<Integer, RobotModel> robots) throws SQLException {
        robots.clear();
        String query = "SELECT * FROM `Robots` LEFT JOIN Stations on Stations.robot_id = `Robots`.id LEFT JOIN `Lines` ON `Lines`.id = `Stations`.`line_id` WHERE `Lines`.id = "
                + lineId;
        Statement statement = null;
        ResultSet rs = null;
        if (debug)
            TimestampPrint.println(query);
        try {
            statement = sqlServer.createStatement();
            statement.setQueryTimeout(TIMEOUT);
            statement.execute(query);
            rs = statement.getResultSet();
            while (rs != null && rs.next()) {
                RobotModel robot = new RobotModel(rs.getInt("id"));
                System.out.println("Robot " + robot.getRobotId() + " added");
                robots.put(robot.getRobotId(), robot);
            }
        } finally {
            closeStatementsAndResults(statement, rs);
        }
    }

    public String getRobotStatus(RobotModel robot) throws SQLException {
        Statement statement = null;
        ResultSet rs = null;
        String query = "SELECT `Robots`.status, `Robots`.update_time FROM Robots LEFT JOIN Stations ON Stations.robot_id = Robots.id LEFT JOIN `Lines` ON `Lines`.id = Stations.line_id WHERE robot_id ="
                + robot.getRobotId() + " AND Lines.id = " + lineId;
        String retval = "";
        //        if (debug)
        //            TimestampPrint.println(query);
        try {
            statement = sqlServer.createStatement();
            statement.setQueryTimeout(TIMEOUT);
            statement.execute(query);
            rs = statement.getResultSet();
            if (rs != null && rs.next()) {
                if (!robot.getLastUpdate().equals(rs.getTimestamp("update_time"))) {
                    retval = rs.getString("status");
                    robot.setLastUpdate(rs.getTimestamp("update_time"));
                }
            } else {
                //no results
            }
        } finally {
            closeStatementsAndResults(statement, rs);
        }
        return retval;
    }

    public boolean isConnectionAlive() throws SQLException {
        return sqlServer != null && sqlServer.isValid(TIMEOUT);
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
