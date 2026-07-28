// full-test.js
const WebSocket = require('ws');

const tests = [];
let passed = 0;
let failed = 0;

function test(name, condition) {
    if (condition) {
        console.log(`✅ ${name}`);
        passed++;
    } else {
        console.log(`❌ ${name}`);
        failed++;
    }
}

async function runFullTest() {
    console.log('🚀 Starting full end-to-end test...\n');

    const alice = new WebSocket('ws://localhost:9002');
    const bob = new WebSocket('ws://localhost:9002');

    let aliceMessages = [];
    let bobMessages = [];

    alice.on('message', (data) => {
        aliceMessages.push(JSON.parse(data));
    });

    bob.on('message', (data) => {
        bobMessages.push(JSON.parse(data));
    });

    // Wait for connections
    await new Promise(r => setTimeout(r, 500));

    // Test 1: Register Alice
    alice.send(JSON.stringify({ action: 'register', username: 'alice', password: '1234' }));
    await new Promise(r => setTimeout(r, 200));
    test('Alice registers', aliceMessages[0]?.type === 'registerResult' && aliceMessages[0]?.success);

    // Test 2: Login Alice
    alice.send(JSON.stringify({ action: 'login', username: 'alice', password: '1234' }));
    await new Promise(r => setTimeout(r, 200));
    test('Alice logs in', aliceMessages[1]?.type === 'loginResult' && aliceMessages[1]?.username === 'alice');

    // Test 3: Create Room
    alice.send(JSON.stringify({ action: 'createRoom' }));
    await new Promise(r => setTimeout(r, 200));
    test('Room created', aliceMessages[2]?.type === 'roomCreated');
    const roomId = aliceMessages[2]?.roomId;

    // Test 4: Register Bob
    bob.send(JSON.stringify({ action: 'register', username: 'bob', password: '1234' }));
    await new Promise(r => setTimeout(r, 200));
    test('Bob registers', bobMessages[0]?.type === 'registerResult' && bobMessages[0]?.success);

    // Test 5: Login Bob
    bob.send(JSON.stringify({ action: 'login', username: 'bob', password: '1234' }));
    await new Promise(r => setTimeout(r, 200));
    test('Bob logs in', bobMessages[1]?.type === 'loginResult' && bobMessages[1]?.username === 'bob');

    // Test 6: Bob joins room
    bob.send(JSON.stringify({ action: 'joinRoom', roomId }));
    await new Promise(r => setTimeout(r, 200));
    test('Bob joins room', bobMessages[2]?.type === 'gameStarting');

    // Test 7: Game starts for Alice
    test('Game starts for Alice', aliceMessages.some(m => m.type === 'gameStarting' && m.color === 'WHITE'));

    // Test 8: Game starts for Bob
    test('Game starts for Bob', bobMessages.some(m => m.type === 'gameStarting' && m.color === 'BLACK'));

    // Test 9: Alice makes move
    alice.send(JSON.stringify({ action: 'move', from: { row: 6, col: 0 }, to: { row: 5, col: 0 } }));
    await new Promise(r => setTimeout(r, 200));
    test('Move validated', aliceMessages.some(m => m.type === 'moveResult' && m.success));

    // Cleanup
    alice.close();
    bob.close();

    console.log(`\n📊 Results: ${passed} passed, ${failed} failed`);
    process.exit(failed > 0 ? 1 : 0);
}

runFullTest().catch(err => {
    console.error('Test error:', err);
    process.exit(1);
});