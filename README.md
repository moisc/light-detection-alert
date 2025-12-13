# Light Detection Alert System

A light detection alert system built for the Heltec LoRa32 v2 board that monitors ambient light levels and sends notifications when they drop below a configurable threshold.

## Features

- **Real-time Light Monitoring**: Reads analog values from a potentiometer (or photoresistor) via GPIO 36
- **Smart Notifications**: Sends Discord webhook notifications when light levels cross threshold
- **OLED Display**: Shows current readings, threshold, and status on built-in 128x64 OLED
- **Calibration Mode**: Easy calibration via serial commands
- **Debouncing**: Prevents notification spam (60-second cooldown)
- **WiFi Connectivity**: Connects to your WiFi network for notifications
- **Free to Run**: Uses Discord webhooks (no ongoing costs)

## Hardware Requirements

- **Heltec LoRa32 v2** (ESP32-based board)
- **Potentiometer** (currently) or **Photoresistor** (for final deployment)
- **10kΩ Resistor** (needed when using photoresistor)
- **USB Cable** for programming and power

## Wiring

### Current Setup (Potentiometer)
```
Potentiometer:
  - Left pin   → 3.3V
  - Middle pin → GPIO 36
  - Right pin  → GND
```

### Future Setup (Photoresistor)
```
Voltage Divider Circuit:
3.3V --- [10kΩ Resistor] --- GPIO36 --- [Photoresistor] --- GND

Behavior:
- More light = lower resistance = higher voltage = higher ADC reading
- Less light = higher resistance = lower voltage = lower ADC reading
```

## Software Setup

### 1. Install PlatformIO

**Option A: Using VSCode**
1. Install [Visual Studio Code](https://code.visualstudio.com/)
2. Install the PlatformIO IDE extension
3. Restart VSCode

**Option B: Using Command Line**
```bash
pip install platformio
```

### 2. Clone and Configure

```bash
# Clone the repository
git clone <your-repo-url>
cd light-detection-alert

# Copy and edit the config file
cp include/config.h include/config.h
```

### 3. Configure WiFi and Discord

Edit `include/config.h` with your credentials:

```cpp
#define WIFI_SSID "YourWiFiName"
#define WIFI_PASSWORD "YourWiFiPassword"
#define DISCORD_WEBHOOK_URL "https://discord.com/api/webhooks/..."
```

#### Creating a Discord Webhook:
1. Open Discord and go to your server
2. Click **Server Settings** → **Integrations** → **Webhooks**
3. Click **New Webhook** or **Create Webhook**
4. Give it a name (e.g., "Light Alert") and select a channel
5. Click **Copy Webhook URL**
6. Paste the URL into `config.h`

### 4. Build and Upload

**Using PlatformIO IDE:**
1. Open the project folder in VSCode
2. Click the PlatformIO icon in the left sidebar
3. Click **Upload** under PROJECT TASKS

**Using Command Line:**
```bash
pio run --target upload
```

### 5. Monitor Serial Output

**Using PlatformIO:**
- Click **Monitor** under PROJECT TASKS

**Using Command Line:**
```bash
pio device monitor
```

## Usage

### Serial Commands

Once uploaded, open the serial monitor (115200 baud) and use these commands:

| Command | Description |
|---------|-------------|
| `c` or `C` | Enter calibration mode (takes 10 readings over 5 seconds) |
| `t` or `T` | Manually set threshold (enter value 0-4095) |
| `s` or `S` | Show current status and settings |

### Calibration Process

1. Open serial monitor
2. Adjust the potentiometer to your desired threshold level
3. Send `c` command
4. Wait for 10 readings to complete
5. Threshold is automatically set to the average

### Normal Operation

- **OLED Display** shows:
  - Current sensor reading (0-4095)
  - Configured threshold
  - Status (Normal, Below, or ALERT!)
  - Visual bar graph
  - WiFi connection status

- **Notifications**: When sensor value drops below threshold, you'll receive a Discord message showing the current reading and threshold

## Project Structure

```
light-detection-alert/
├── src/
│   └── main.ino          # Main application code
├── include/
│   └── config.h          # WiFi and notification configuration
├── platformio.ini        # PlatformIO configuration
└── README.md            # This file
```

## Troubleshooting

### WiFi Won't Connect
- Double-check SSID and password in `config.h`
- Ensure your WiFi is 2.4GHz (ESP32 doesn't support 5GHz)
- Check serial monitor for connection status

### No Notifications Received
- Verify Discord webhook URL is correct
- Check that WiFi is connected (shown on OLED)
- Test the webhook URL using curl or a REST client
- Check serial monitor for HTTP response codes

### OLED Not Working
- The pin definitions should be correct for Heltec LoRa32 v2
- If display is blank, check OLED_RST, OLED_SDA, OLED_SCL pins
- Try power cycling the board

### Sensor Values Seem Wrong
- Check wiring to GPIO 36
- Use serial monitor to see raw values (send `s` command)
- Recalibrate using `c` command
- Values should range from 0-4095

## Libraries Used

- **U8g2** - OLED display driver
- **WiFi** - ESP32 WiFi (built-in)
- **HTTPClient** - HTTP requests (built-in)
- **Wire** - I2C communication (built-in)

## License

MIT License - Feel free to modify and use as needed!

## Future Enhancements

- [ ] Add Telegram bot support as alternative to Discord
- [ ] Add NTP time sync for timestamped alerts
- [ ] Store settings in EEPROM/Preferences
- [ ] Add web server for remote configuration
- [ ] Battery level monitoring
- [ ] Deep sleep mode for power saving
- [ ] Multiple threshold levels
- [ ] Data logging to SD card or cloud

## Contributing

Feel free to open issues or submit pull requests!
