# WebStrip

## API

### Resources

* Mode
* File
* Options
* OtaUpdate

|    Resource    |  Methods  | Description                    |
|----------------|-----------|--------------------------------|
| /api/mode      | GET, POST | Get or change current mode     |
| /api/file      | GET, POST | Restore or save current mode   |
| /api/options   | GET, POST | Get or update options          |
| /api/otaUpdate | GET       | Enable OTA Update mode         |

### /api/mode
#### GET
##### Return
JSON with full current mode.

#### POST
##### Parameters

| Name | Required | Format | Description |
|------|----------|--------|-------------|
| data | yes      | JSON   | Mode        |

##### Mode JSON

|       Field       |Required|    Type    |    Default    |   Description  |
|-------------------|--------|------------|---------------|----------------|
| index             | no     | int(0-32)  | 0             | File index to save current mode |
| description       | no     | string(31) | Default mode  | Mode description |
| colorSelectionMode| no     | int(0-2)   | 0             | How to get colors from palette |
| animationMode     | no     | int(0-5)   | 0             | Animation mode |
| animationSpeed    | no     | int(0-255) | 128           | Animation Speed |
| animationProgressMode | no | int(0-3)   | 0             | Animation Progress modifier |
| animationIntensity| no     | int(0-255) | 1             | Count of animation effects per cycle |
| colors            | no     | string[32] | Rainbow colors| Colors palette. List of HTML color codes |

### /api/file

### /api/options

### /api/otaUpdate