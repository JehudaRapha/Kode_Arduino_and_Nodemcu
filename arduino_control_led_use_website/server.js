const { SerialPort } = require('serialport');
const { Readline, ReadlineParser } = require('@serialport/parser-readline'); // Merubah nama ReadlineParser menjadi Readline
const { Server } = require('socket.io');
const express = require('express');
const http = require('http');

const app = express();
const server = http.createServer(app);
const io = new Server(server);

app.use(express.json());
app.use(express.static('public'));

app.get('/', (req, res) => {
  res.sendFile(__dirname + '/views/index.html');
});

io.on('connection', (socket) => {
  console.log('connection...');
  socket.on('disconnect', () => {
    console.log('disconnect');
  });
});

server.listen(3000, () => {
  // Mengubah app.listen menjadi server.listen
  console.log('server on!');
});

// Konektifitas serial arduino
const port = new SerialPort({
  // Mengubah pemanggilan SerialPort
  path: 'COM7',
  baudRate: 19200,
});

// Parsing data dari arduino
const parser = port.pipe(new ReadlineParser({ delimiter: '\r\n' })); // Mengubah nama ReadlineParser menjadi Readline dan mengubah delimiter menjadi "\r\n"

// Tangkap Data dari Arduino
parser.on('data', (result) => {
  // Mengubah variabel "resurt" menjadi "result"
  console.log('data dari arduino -> ', result);
  io.emit('data', { data: result });
});

app.post('/arduinoApi', (req, res) => {
  const data = req.body.data;
  port.write(data, (err) => {
    if (err) {
      console.log('eer: ', err);
      res.status(500).json({ error: 'write data error!' });
    }
    console.log('data terkirim ->', data);
    res.status(200).json({ data: 'send command success' });
    res.end();
  });
});
