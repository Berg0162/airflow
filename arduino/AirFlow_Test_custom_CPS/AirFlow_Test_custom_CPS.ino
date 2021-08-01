/*********************************************************************
 This is an example for our nRF52 based Bluefruit LE modules

 Pick one up today in the adafruit shop!

 Adafruit invests time and resources providing this open source code,
 please support Adafruit and open-source hardware by purchasing
 products from Adafruit!

 MIT license, check LICENSE for more information
 All text above, and the splash screen below must be included in
 any redistribution
*********************************************************************/

/* This sketch show how to use BLEClientService and BLEClientCharacteristic
 * to implement a custom client that is used to talk with Gatt server on
 * peripheral.
 *
 * Note: you will need another feather52 running peripheral/custom_HRM sketch
 * to test with.
 */

#include <bluefruit.h>

#define SERVICE_DEVICE_INFORMATION                  0x180A
#define CHARACTERISTIC_MANUFACTURER_NAME_STRING     0x2A29
#define CHARACTERISTIC_MODEL_NUMBER_STRING          0x2A24
#define CHARACTERISTIC_SERIAL_NUMBER_STRING         0x2A25

/* Cycling Power Service
 * CP Service: 0x1818  
 * CP Characteristic: 0x2A63 (Measurement)
 * CP Characteristic: 0x2A65 (Feature)
 * CP Characteristic: 0x2A5D (Location)
 */
BLEClientService        cps(UUID16_SVC_CYCLING_POWER);
BLEClientCharacteristic cpmc(UUID16_CHR_CYCLING_POWER_MEASUREMENT);
// BLEClientCharacteristic cpfc(UUID16_CHR_CYCLING_POWER_FEATURE);
BLEClientCharacteristic cplc(UUID16_CHR_SENSOR_LOCATION);

BLEClientService        diss(SERVICE_DEVICE_INFORMATION);

unsigned long Timeinterval = 0;

void setup()
{
  Serial.begin(115200);
  while ( !Serial ) delay(10);   // for nrf52840 with native usb

  Serial.println("Bluefruit52 Central Custom CPS Example");
  Serial.println("--------------------------------------\n");

  // Initialize Bluefruit with maximum connections as Peripheral = 0, Central = 1
  // SRAM usage required by SoftDevice will increase dramatically with number of connections
  Bluefruit.begin(0, 1);

  Bluefruit.setName("Bluefruit52 Central");

  // Initialize CPS client
  cps.begin();

  // Initialize client characteristics of CPS.
  // Note: Client Char will be added to the last service that is begin()ed.
  cplc.begin();

  // set up callback for receiving measurement
  cpmc.setNotifyCallback(cpm_notify_callback);
  cpmc.begin();

  // Initialize DISS client.
  diss.begin();

  // Increase Blink rate to different from PrPh advertising mode
  Bluefruit.setConnLedInterval(250);

  // Callbacks for Central
  Bluefruit.Central.setDisconnectCallback(disconnect_callback);
  Bluefruit.Central.setConnectCallback(connect_callback);

  /* Start Central Scanning
   * - Enable auto scan if disconnected
   * - Interval = 100 ms, window = 80 ms
   * - Don't use active scan
   * - Filter only accept HRM service
   * - Start(timeout) with timeout = 0 will scan forever (until connected)
   */
  Bluefruit.Scanner.setRxCallback(scan_callback);
  Bluefruit.Scanner.restartOnDisconnect(true);
  Bluefruit.Scanner.filterRssi(-70);   // original value of -80 , we want to scan only nearby peripherals, so get close to your device !!
  Bluefruit.Scanner.setInterval(160, 80); // in unit of 0.625 ms
  Bluefruit.Scanner.filterUuid(0x1818); // only invoke callback if one or more of these services are advertised 
  //Bluefruit.Scanner.filterUuid(UUID16_SVC_CYCLING_POWER);
  Bluefruit.Scanner.useActiveScan(true);
  Bluefruit.Scanner.start(0);                   // // 0 = Don't stop scanning after n seconds
}

void loop()
{
  // do nothing
}

/**
 * Callback invoked when scanner pick up an advertising data
 * @param report Structural advertising data
 */
void scan_callback(ble_gap_evt_adv_report_t* report)
{
  // Since we configure the scanner with filterUuid()
  // Scan callback only invoked for device with CPS service advertised
  // Connect to device with CPS service in advertising
  uint8_t buffer[32] = {0};
  Serial.println(F("Device found:"));
  // Shortened Local Name
  if(Bluefruit.Scanner.parseReportByType(report, BLE_GAP_AD_TYPE_SHORT_LOCAL_NAME, buffer, sizeof(buffer)))
  {
    Serial.printf("%14s %s | ", "SHORT NAME", buffer);
    memset(buffer, 0, sizeof(buffer));
  }
  if (Bluefruit.Scanner.checkReportForUuid(report, UUID16_SVC_CYCLING_POWER)) {
      // Complete Local Name
      if(Bluefruit.Scanner.parseReportByType(report, BLE_GAP_AD_TYPE_COMPLETE_LOCAL_NAME, buffer, sizeof(buffer)))
        {
        Serial.printf("Cycling Power Sevice delivered by: %14s %s\n", "COMPLETE NAME", buffer);
        }
  }
  Serial.println(F("Timestamp Addr              Rssi Data"));
  Serial.printf("%09d ", millis());
  // MAC is in little endian --> print reverse
  Serial.printBufferReverse(report->peer_addr.addr, 6, ':');
  Serial.print(F(" "));
  Serial.print(report->rssi);
  Serial.print(F("  "));
  Serial.printBuffer(report->data.p_data, report->data.len, '-');
  Serial.println();
  
  Bluefruit.Central.connect(report);
}

//-------------------------------------------------------
/**
 * Callback invoked when an connection is established
 * @param conn_handle
 */
void connect_callback(uint16_t conn_handle)
{
  Serial.println("Connecting");
  Serial.print("Discovering CPS Service ... ");

  // If CPS is not found, disconnect, resume scanning, and return
  if ( !cps.discover(conn_handle) )
  {
    Serial.println("Found NONE");
    // disconnect since we couldn't find CPS service
    Bluefruit.disconnect(conn_handle);
    // For Softdevice v6: after receiving a report (scan_callback), scanner will be paused (!)
    // We need to call Scanner resume() to continue scanning since we did not find a CPS Service!
    Bluefruit.Scanner.resume();
    return;
  }

  // Once CPS service is found, we continue to discover its characteristic
  Serial.println("Found it");
  
  Serial.print("Discovering Measurement characteristic ... ");
  if ( !cpmc.discover() )
  {
    // Measurement chr is mandatory, if it is not found (valid), then disconnect
    Serial.println("not found !!!");  
    Serial.println("Measurement characteristic is mandatory but not found");
    Bluefruit.disconnect(conn_handle);
    return;
  }
  Serial.println("Found it");

  Serial.print("Discovering Power Sensor Location characteristic ... ");
  if ( cplc.discover() )
  {
    Serial.println("Found it");
    
  // The Sensor Location characteristic
  // See: https://www.bluetooth.com/specifications/gatt/viewer?attributeXmlFile=org.bluetooth.characteristic.sensor_location.xml
  // Properties = Read
  // Min Len    = 1
  // Max Len    = 1
  //  B0:1      = UINT16 - Sensor Location
  //  Dec.
  //      0     = Other
  //      1     = Top of shoe
  //      2     = In shoe
  //      3     = Hip
  //      4     = Front wheel
  //      5     = Left crank
  //      6     = Right crank
  //      7     = Left pedal
  //      8     = Right pedal
  //      9     = Front hub
  //      10    = Rear dropout
  //      11    = Chainstay
  //      12    = Rear wheel
  //      13    = Rear hub
  //      14    = Chest
  //      15    = Spider
  //      16    = Chain ring
  //  17:255    = Reserved
    
    // power sensor location value is 16 bit
    const char* power_str[] = { "Other", "Top of shoe", "In shoe", "Hip", "Front wheel", "Left crank", "Right crank", "Left pedal",
    "Right pedal", "Front hub", "Rear dropout", "Chainstay", "Rear wheel", "Rear hub", "Chest", "Spider", "Chain ring"};

    // Read 16-bit cplc value from peripheral
   uint8_t loc_value = cplc.read8();
    
    Serial.print("Power Location Sensor: ");
    Serial.printf("Loc#: %d %s\n", loc_value, power_str[loc_value]);
  }else
  {
    Serial.println("Found NONE");
  }

  // Reaching here means we are ready to go, let's enable notification on measurement chr
  if ( cpmc.enableNotify() )
  {
    Serial.println("Ready to receive HRM Measurement value");
  }else
  {
    Serial.println("Couldn't enable notify for HRM Measurement. Increase DEBUG LEVEL for troubleshooting");
  }
}

/**
 * Callback invoked when a connection is dropped
 * @param conn_handle
 * @param reason is a BLE_HCI_STATUS_CODE which can be found in ble_hci.h
 */
void disconnect_callback(uint16_t conn_handle, uint8_t reason)
{
  (void) conn_handle;
  (void) reason;

  Serial.print("Disconnected, reason = 0x"); Serial.println(reason, HEX);
}


/**
 * Hooked callback that triggered when a measurement value is sent from peripheral
 * @param chr   Pointer client characteristic that even occurred,
 *              in this example it should be cpmc
 * @param data  Pointer to received data
 * @param len   Length of received data
 */
void cpm_notify_callback(BLEClientCharacteristic* chr, uint8_t* data, uint16_t len)
{

uint8_t buffer[35]= {}; /*{     (uint8_t)(cpmcDef & 0xff), (uint8_t)(cpmcDef >> 8),  // flags 
                             (uint8_t)(powerOut & 0xff), (uint8_t)(powerOut >> 8),  // inst. power 
                                                                                0,  // bal
                                                                             0, 0,  // torque
                                                                       0, 0, 0, 0,  // cum. rev
                                                                             0, 0,  // wheel time
                                                                             0, 0,  // cum. crank
                                                                             0, 0,  // crank time
                                                                             0, 0,  // max force
                                                                             0, 0,  // min force
                                                                             0, 0,  // max tor
                                                                             0, 0,  // min tor
                                                                             0, 0,  // max ang
                                                                             0, 0,  // min ang
                                                                             0, 0,  // tdc
                                                                             0, 0,  // bdc
                                                                             0, 0 }; // total energy
*/
  // Transfer first the contents of data to buffer (array of chars)
//  Serial.print("CPS Data: "); 
  for (int i = 0; i < len; i++) {
    if ( i <= sizeof(buffer)) {
      buffer[i] = *data++;
//      Serial.printf("%02X ", buffer[i], HEX);
    }
  }
//  Serial.println();
  long PowerValue = 0;
  uint8_t lsb_InstantaneousPower = buffer[2]; // Instantaneous Power LSB
  // POWER is stored in 2 bytes !!!
  uint8_t msb_InstantaneousPower = (buffer[3] & 0xFF); //  
  PowerValue = lsb_InstantaneousPower + msb_InstantaneousPower * 256;
  
  Serial.printf("TimeInterval: %d Power Value:   %4d\n", (millis()-Timeinterval), PowerValue);
  Timeinterval = millis();
}
