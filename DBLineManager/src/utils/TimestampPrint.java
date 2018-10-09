package utils;

public class TimestampPrint {
    private TimestampPrint() {

    }

    public static void printStackTrace(Exception e) {
        String time = new java.sql.Timestamp(System.currentTimeMillis()).toString();
        System.err.print(time + " ");
        e.printStackTrace();
    }

    public static void println(String str) {
        String time = new java.sql.Timestamp(System.currentTimeMillis()).toString();
        System.out.println(time + " " + str);
    }

}
