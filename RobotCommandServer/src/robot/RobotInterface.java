package robot;

import java.io.IOException;
import java.io.InputStream;
import java.io.OutputStream;
import java.net.Socket;
import java.net.UnknownHostException;

import utils.TimestampPrint;

public class RobotInterface {
    Socket socket;
    OutputStream os;
    InputStream is;
    boolean debug = false;

    public enum RobotCommands {
        COMMAND_NONE(""),
        COMMAND_EMERGENCY_STOP("estop"),
        COMMAND_VAC_ON("vcon"),
        COMMAND_VAC_OFF("vcoff"),
        COMMAND_ZERO_RETURN("zero"),
        COMMAND_AXIS(""),
        COMMAND_LOAD_CONFIG(""),
        COMMAND_DROP_ITEM("drop"),
        COMMAND_PICK_ITEM("pick"),
        COMMAND_ZERO_NEEDED("zneeded");

        private String command;

        private RobotCommands(String command) {
            this.command = command;
        }

        public String toString() {
            return command;
        }
    }

    public RobotInterface() {

    }

    public void connect() throws UnknownHostException, IOException {
        if (socket != null && !socket.isClosed()) {
            socket.close();
        }
        socket = new Socket("127.0.0.1", 6000);
        os = socket.getOutputStream();
        is = socket.getInputStream();
    }

    public boolean isConnected() {
        return socket.isConnected() && !socket.isClosed();
    }

    public void commandRobot(RobotCommands command) throws IOException {
        if (!isConnected()) {
            connect();
        }
        System.out.println("Sending command: " + command);
        os.write((command + "\r\n").getBytes());
        os.flush();
    }

    public String getRobotStatus() throws IOException, InterruptedException {
        if (!isConnected()) {
            connect();
        }
        os.write(("status" + "\r\n").getBytes());
        os.flush();
        Thread.sleep(100);
        byte[] buffer = new byte[1024];
        if (debug) {
            TimestampPrint.println("Reading");
        }
        is.read(buffer, 0, 1024);
        String response = new String(buffer);
        if (debug) {
            TimestampPrint.println("read: " + response);
        }
        return response;
    }
}
