package robot;

public class StationInformation {
    private static int stationId = -1;
    private static int lineId = -1;

    public static int getStationId() {
        return stationId;
    }

    public static void setStationId(int stationId) {
        StationInformation.stationId = stationId;
    }

    public static int getLineId() {
        return lineId;
    }

    public static void setLineId(int lineId) {
        StationInformation.lineId = lineId;
    }
}
