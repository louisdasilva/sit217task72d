const mqtt = require('mqtt');
const { SerialPort } = require('serialport');
const { ReadlineParser } = require('@serialport/parser-readline');

const port = new SerialPort({
    path: 'COM6',
    baudRate: 9600
});
const parser = port.pipe(new ReadlineParser({ delimiter: '\n' }));

const client = mqtt.connect("mqtt://broker.hivemq.com:1883");
const leader = '/218541748/rover/lead'; // lead channel for rover leading swarm mode
const follower = '/218541748/rover/follow'; // follow channel for drone in swarm mode
client.on('connect', () => {
    client.subscribe(follower); 
    console.log('connected');
});

// Read the port data
port.on("open", () => {
    console.log('serial port open');
});

// Receive a message from the rover in leader mode
parser.on('data', data => {
    console.log('Rover Output:', data);
    client.publish(leader, data, () => {
        console.log(`Rover output published to ${leader}`);
    });
});

// Receive message from swarm leader for rover to follow as drone
client.on('message', (follower, message) => {
    console.log(`Received message on ${follower}: ${message}`);
});
client.on('message', (follower, message) => {
    if (follower == '/218541748/rover/follow') {
        msg = `${message}\n`;
        console.log(msg);
        port.write(msg, (err) => {
            if (err) {
                return console.log('Error on write: ', err.message);
            }
            console.log('message written');
        });
    }
});