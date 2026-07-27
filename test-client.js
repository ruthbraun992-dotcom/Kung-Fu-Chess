const WebSocket = require('ws');

const ws = new WebSocket('ws://localhost:9002');

ws.on('open', () => {
    console.log('P1 connected');

    ws.send(JSON.stringify({
        action: 'register',
        username: 'alice',
        password: '1234'
    }));
});

ws.on('message', (data) => {
    const msg = JSON.parse(data.toString());

    console.log('P1 received:', msg);

    if (msg.type === 'registerResult' && msg.success) {
        ws.send(JSON.stringify({
            action: 'login',
            username: 'alice',
            password: '1234'
        }));
    }

    if (msg.type === 'loginResult' && msg.success) {
        ws.send(JSON.stringify({
            action: 'createRoom'
        }));
    }

    if (msg.type === 'gameStarting') {

        console.log("Sending first move");

        ws.send(JSON.stringify({
            action: "move",
            from: { row: 6, col: 0 },
            to: { row: 5, col: 0 }
        }));
    }
});