package conveyor;

import java.io.BufferedReader;
import java.io.IOException;
import java.io.InputStreamReader;
import java.net.HttpURLConnection;
import java.net.URL;

public class ConveyorController {
    private final String USER_AGENT = "Mozilla/5.0";
    private final static long CONVEYOR_RUN_TIME = 5000;
    private long lastIndexTime = System.currentTimeMillis() - CONVEYOR_RUN_TIME;

    public void indexConveyor() throws IOException {
        String url = "http://freshc.freshrealm.local/rr_gcode?gcode=M98P0:/macros/conveyor_index";

        URL urlObj = new URL(url);
        HttpURLConnection con = (HttpURLConnection) urlObj.openConnection();

        // optional default is GET
        con.setRequestMethod("GET");

        //add request header
        con.setRequestProperty("User-Agent", USER_AGENT);

        System.out.println("Sending 'GET' request to URL : " + url);
        con.setReadTimeout(15 * 1000);
        con.connect();
        int responseCode = con.getResponseCode();
        System.out.println("Response Code : " + responseCode);

        lastIndexTime = System.currentTimeMillis();

        BufferedReader in = new BufferedReader(new InputStreamReader(con.getInputStream()));
        String inputLine;
        StringBuffer response = new StringBuffer();

        while ((inputLine = in.readLine()) != null) {
            response.append(inputLine);
        }
        in.close();

        //print result
        System.out.println(response.toString());
    }

    public boolean isConveyorReady() {
        return System.currentTimeMillis() >= lastIndexTime + CONVEYOR_RUN_TIME;
    }
}
