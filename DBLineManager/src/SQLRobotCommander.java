import database.SQLServer;
import line.LineModel;

public class SQLRobotCommander {
    public static void main(String argv[]) {
        int lineId = 0;
        try {
            lineId = Integer.parseInt(argv[0]);
        } catch (ArrayIndexOutOfBoundsException | NullPointerException | NumberFormatException e) {
            System.out.println("Requires line id as argument");
            System.exit(1);
        }

        SQLServer db = new SQLServer(lineId);
        LineModel lineModel = new LineModel(db);

        while (true) {
            try {
                db.connectToDB();
                db.populateRobots(lineModel.getRobots());
                while (true) {
                    try {
                        lineModel.manageLine();
                        Thread.sleep(500);
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
