var events = require('events');
var eventEmitter = new events.EventEmitter();

var myEventHandler = function() {
    console.log('Connection Made');
}

eventEmitter.on('scream',myEventHandler);
eventEmitter.emit('scream');