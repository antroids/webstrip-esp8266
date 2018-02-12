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
| /api/saveMode  | GET       | Save current mode              |
| /api/loadMode  | GET       | Load mode                      |
| /api/options   | GET, POST | Get or update options          |
| /api/otaUpdate | GET       | Enable OTA Update mode         |
| /api/sysInfo   | GET       | Get system status              |

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
| colorSelectionMode| no     | int(0-3)   | 0             | How to get colors from palette |
| animationMode     | no     | int(0-7)   | 0             | Animation mode |
| animationSpeed    | no     | int(0-255) | 128           | Animation Speed |
| animationProgressMode | no | int(0-3)   | 0             | Animation Progress modifier |
| animationIntensity| no     | int(0-255) | 1             | Count of animation effects per cycle |
| nextMode          | no     | mode index | 0             | Auto change to this mode |
| nextModeDelay     | no     | int(0-255) | 0             | 0: auto change disabled; >0 auto change to next mode after this delay in seconds|
| colors            | no     | string[32] | Rainbow colors| Colors palette. List of HTML color codes |

### /api/saveMode

### /api/loadMode

### /api/options

### /api/otaUpdate
