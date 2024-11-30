let websocket;
let username;

// Function to append messages to the message box
function appendMessage(msg) {
    const messagesBox = document.getElementById('messages');
    messagesBox.textContent += JSON.stringify(msg, null, 2) + '\n';
    messagesBox.scrollTop = messagesBox.scrollHeight;  // Scroll to the bottom
}

// Function to connect to the WebSocket server
function connectToServer() {
    username = document.getElementById('username').value;
    const recipient = document.getElementById('recipient').value;

    if (!username || !recipient) {
        alert('Please enter both your username and recipient username');
        return;
    }

    // Connect to the server
    websocket = new WebSocket('ws://b746-2601-547-b01-309e-8082-41b2-4caf-9ae7.ngrok-free.app');
    
    websocket.onopen = () => {
        // Send username after connecting
        websocket.send(JSON.stringify({ username }));
        appendMessage({ message: `Connected as ${username}`, code: 0 });
    };

    websocket.onmessage = (event) => {
        const data = JSON.parse(event.data);
        appendMessage(data);
    };

    websocket.onerror = (error) => {
        appendMessage({ message: 'Error connecting to server', code: -1 });
    };

    websocket.onclose = () => {
        appendMessage({ message: 'Connection closed', code: -1 });
    };
}

// Function to send a message to the recipient
function sendMessage() {
    const recipient = document.getElementById('recipient').value;
    const message = document.getElementById('message').value;

    if (!message || !recipient) {
        alert('Please enter both message and recipient');
        return;
    }

    // Send message to the server
    const messageData = {
        recepient: recipient,
        message: message
    };
    websocket.send(JSON.stringify(messageData));

    // Clear message input
    document.getElementById('message').value = '';
    appendMessage(messageData)
}

// Event listeners for buttons
document.getElementById('connectBtn').addEventListener('click', connectToServer);
document.getElementById('sendBtn').addEventListener('click', sendMessage);
