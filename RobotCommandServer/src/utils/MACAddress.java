package utils;

import java.io.File;
import java.io.IOException;
import java.nio.file.Files;

public class MACAddress {

    private static String macAddress;

    private MACAddress() {
        // No construction
    }

    /**
     * Resolves the MAC address from the system pseudo-files, prioritizing the
     * modem, then wlan, then eth. If none are found, NO-MAC is returned.
     */
    private static void resolveMACAddress() {
        File[] files = new File[] { new File("/sys/class/net/enxb827eb922850/address"),
                new File("/sys/class/net/eth0/address"), new File("/sys/class/net/wlan0/address"),
                new File("/sys/class/net/wwan0/address") };

        for (File file : files) {
            try {
                if (Files.exists(file.toPath())) {
                    macAddress = extractMAC(file).trim();
                    return;
                }
            } catch (IOException e) {
                e.printStackTrace();
            }
        }
        // The above failed, just report no mac
        macAddress = "NO-MAC";
    }

    private static String extractMAC(File file) throws IOException {
        return new String(Files.readAllBytes(file.toPath()));
    }

    /**
     * If no MAC address is set, resolve it, otherwise simply return it.
     * 
     * @return the MAC address, otherwise "NO-MAC"
     */
    public static String getMACAddress() {
        if (macAddress == null) {
            resolveMACAddress();
        }
        return macAddress;
    }

}
