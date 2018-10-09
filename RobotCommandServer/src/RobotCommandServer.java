import database.SQLServer;
import database.SQLServerInterface;
import robot.RobotInterface;
import robot.RobotInterface.RobotCommands;

public class RobotCommandServer {

    public static void main(String argv[]) {
        RobotInterface robot = new RobotInterface();
        SQLServerInterface db = new SQLServer();
        while (true) {
            try {
                System.out.println("Connecting to DB...");
                db.connectToDB();
                System.out.println("Connecting to Robot...");
                robot.connect();
                System.out.println("Connected.");
                db.getRobotInfo();
                while (true) {
                    try {
                        RobotCommands command = db.getRobotCommand();
                        if (command != RobotCommands.COMMAND_NONE) {
                            robot.commandRobot(command);
                        }
                        String status = robot.getRobotStatus();
                        db.updateStatus(status);
                        Thread.sleep(1000);
                    } catch (Exception e) {
                        e.printStackTrace();
                    }
                }

            } catch (Exception e) {
                //Keep alive
            }
        }
    }
}
