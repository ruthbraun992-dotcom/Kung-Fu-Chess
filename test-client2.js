const WebSocket = require('ws');
const ws = new WebSocket('ws://localhost:9002');
ws.on('open', () => {
    console.log('P2 connected');
    ws.send(JSON.stringify({action:'register', username:'bob', password:'1234'}));
});
ws.on('message', (data) => {
    const msg = JSON.parse(data.toString());
    console.log('P2 received:', data.toString());
    if (msg.type === 'registerResult' && msg.success)
        ws.send(JSON.stringify({action:'login', username:'bob', password:'1234'}));
    if (msg.type === 'loginResult' && msg.success)
        ws.send(JSON.stringify({action:'joinRoom', roomId:'R1'}));
});