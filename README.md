# BluESP

*BlueESP* is an ESP32 program that uses Bluetooth Low Energy (BLE) with security through encryption and PIN authentication. The program sends nine random values every 5 seconds to connected devices, and BLE advertising is automatically repeated if no devices are connected within 60 seconds.

**Key Features:**
1. Periodic transmission of random data.
2. BLE security with PIN authentication and encryption.
3. Automatic advertising repeat if no connections are active.
4. Handling of BLE device connection status.
5. MIT App Inventor integration to display data from BLE.
6. Sending data to Google Spreadsheet.

### Google Spreadsheet Integration

1. **Create Spreadsheet**: Set up Google Sheets and create a worksheet named "Sheet1."
2. **Share Spreadsheet**: Change sharing settings to "Anyone with the link can view."
3. **Apps Script**: Add code for *GET* and *POST* data to the spreadsheet.

### Apps Script Code:
```javascript
function doGet(e) {
  const sheet = SpreadsheetApp.getActiveSpreadsheet().getSheetByName("Sheet1");
  const data = sheet.getDataRange().getValues();
  return ContentService.createTextOutput(JSON.stringify(data)).setMimeType(ContentService.MimeType.JSON);
}

function doPost(e) {
  const sheet = SpreadsheetApp.getActiveSpreadsheet().getSheetByName("Sheet1");
  try {
    const data = JSON.parse(e.postData.contents);
    sheet.appendRow(data.data);
    return ContentService.createTextOutput(JSON.stringify({status: "success"})).setMimeType(ContentService.MimeType.JSON);
  } catch (error) {
    return ContentService.createTextOutput(JSON.stringify({status: "error", message: error.toString()})).setMimeType(ContentService.MimeType.JSON);
  }
}
```

4. **Deploy**: Save and deploy as a *Web App*, and copy the generated API URL.

### Using the Request

- **GET Data**: Retrieve data from Google Sheets with a GET request.

```javascript
function getData() {
  const url = 'YOUR_API_ENDPOINT';  // Replace with your API URL
  fetch(url, { method: 'GET' })
    .then(response => response.json())
    .then(data => console.log('Data:', data))
    .catch(error => console.error('Error:', error));
}
```

- **POST Data**: Send data to Google Sheets with a POST request.

```javascript
function sendData() {
  const url = 'YOUR_API_ENDPOINT';
  const data = { data: ["Name", "Email", "Date"] };
  fetch(url, {
    method: 'POST',
    headers: { 'Content-Type': 'application/json' },
    body: JSON.stringify(data)
  })
  .then(response => response.json())
  .then(data => console.log('Success:', data))
  .catch(error => console.error('Error:', error));
}
```
