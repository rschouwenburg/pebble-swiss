var Clay = require('pebble-clay');
var clayConfig = require('./config');
var clay = new Clay(clayConfig);

Pebble.addEventListener('ready', function() {
  console.log('PebbleKit JS ready!');
});

//Pebble.addEventListener('showConfiguration', function() {
//  var url = 'http://pebble.schouwenburg.com/pebble/swissdark-2-0.html';
//  console.log('Showing configuration page: ' + url);

//  Pebble.openURL(url);
//});

//Pebble.addEventListener('webviewclosed', function(e) {
//  var configData = JSON.parse(decodeURIComponent(e.response));
//  console.log('Configuration page returned: ' + JSON.stringify(configData));

//  var vibrate = configData['vibrate'] ? 1 : 0;
//  var second = configData['second'] ? 1 : 0;
//  var date = configData['date'] ? 1 : 0;
//  var ticks = configData['ticks'] ? 1 : 0;

//  var dict = {};
//  dict['KEY_VIBRATE'] = parseInt(vibrate);
//  dict['KEY_SECOND'] = parseInt(second);
//  dict['KEY_DATE'] = parseInt(date);
//  dict['KEY_TICKS'] = parseInt(ticks);

  // Send to watchapp
//  Pebble.sendAppMessage(dict, function() {
//    console.log('Send successful: ' + JSON.stringify(dict));
// }, function() {
//    console.log('Send failed!');
//  });
//});