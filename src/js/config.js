module.exports =

[
  {
    "type": "heading",
    "id": "main-heading",
    "defaultValue": "SWISS CONFIGURATION",
    "size": 1
  },
  {
  "type": "section",
  "items": [
    {
      "type": "heading",
      "defaultValue": "Theme"
    },
  
    {
      "type": "color",
      "messageKey": "KEY_COLOR",
      "defaultValue": "000000",
      "label": "Background Color",
      "sunlight": false,
      "layout": "BLACK_WHITE"
    } ]
  },
  
  
  {
  "type": "section",
  "items": [
    {
      "type": "heading",
      "defaultValue": "Phone bluetooth connection"
    },
    {
      "type": "toggle",
      "messageKey": "KEY_VIBRATE",
      "label": "Vibrate",
      "defaultValue": true,
      "description": "Turn on or off the vibration on your Pebble when there is a change in the bluetooth connection."
    }
  ]
  },
  {
  "type": "section",
  "items": [
    {
      "type": "heading",
      "defaultValue": "Features"
    },
    {
      "type": "select",
      "messageKey": "KEY_BLUETOOTH",
      "defaultValue": "0",
      "label": "Bluetooth status",
      "options": [
        { 
          "label": "Show when lost", 
          "value": "0"
        },
        { 
          "label": "Always show", 
          "value": "1"
        },
        { 
          "label": "Never show", 
          "value": "2"
        },
      ]
    },
    {
      "type": "select",
      "messageKey": "KEY_BATTERY",
      "defaultValue": "0",
      "label": "Battery status",
      "options": [
        { 
          "label": "Show when low", 
          "value": "0"
        },
        { 
          "label": "Always show", 
          "value": "1"
        },
        { 
          "label": "Never show", 
          "value": "2"
        },
      ]
    },
    {
      "type": "toggle",
      "messageKey": "KEY_DATE",
      "label": "Show date",
    },
    {
      "type": "toggle",
      "messageKey": "KEY_TICKS",
      "defaultValue": true,
      "label": "Show ticks",
    }
  ]
  },
  {
  "type": "section",
  "items": [
    {
      "type": "heading",
      "defaultValue": "Power saving"
    },
    {
      "type": "toggle",
      "messageKey": "KEY_SECOND",
      "label": "Show second hand",
      "defaultValue": true,
      "description": "By turning off the second hand, you can conserve up to 25% of power consumption adding a couple of days of battery life to your Pebble depending on your model."
    }
  ]
  },
  {
    "type": "submit",
    "defaultValue": "Save"
  }
];