let peerConnection; // RTCPeerConnection object that manages and represents the connection state
let localStream;    // MediaStream from the user's local webcam/microphone
let remoteStream;   // MediaStream to hold the remote peer's media tracks

let servers = {
    iceServers: [   // ICE servers used for NAT traversal and peer discovery
        {
            urls: [
                'stun:stun.l.google.com:19302',  // Public STUN server for determining public IP
                'stun:stun.2.google.com:19302'  // Backup STUN server
            ]
        },
        {
            urls: 'turn:google.com:19302?transport=tcp',  // Google TURN server URL
            username: 'webrtc',                          // TURN server username
            credential: 'webrtc'                         // TURN server password
        }
    ]
}

// Triggered as soon as the user loads the page
let init = async () => {
    // Request access to the user's webcam and/or microphone
    localStream = await navigator.mediaDevices.getUserMedia({video: true, audio: false});

    // Display the local video stream on the page
    document.getElementById('user-1').srcObject = localStream;
};

let createPeerConnection = async(sdpType) => {
    peerConnection = new RTCPeerConnection(servers); // Create a new peer connection object
    
    remoteStream = new MediaStream(); // Initialize an empty MediaStream to store remote tracks
    document.getElementById('user-2').srcObject = remoteStream; // Display the remote stream in the UI

    // Add each track from localStream to the peerConnection
    localStream.getTracks().forEach((track) => {
        peerConnection.addTrack(track, localStream); 
        // Each track is associated with the localStream, which helps in grouping and signaling
    });

    // Set up the ontrack event (fired when remote tracks are received)
    // This event adds all tracks from the remote peer to the remoteStream object
    peerConnection.ontrack = async (event) => {
        event.streams[0].getTracks().forEach((track) => {
            remoteStream.addTrack(track);
        });
    };

    // Set up the onicecandidate event (fired each time an ICE candidate is generated)
    peerConnection.onicecandidate = async (event) => {
        if (event.candidate) { 
            // Update the SDP in the UI with ICE candidates added
            document.getElementById(sdpType).value = JSON.stringify(peerConnection.localDescription);
        }
    };

}

let createOffer = async () => {
    await createPeerConnection('offer-sdp');

    let offer = await peerConnection.createOffer(); // Generates an SDP offer (does NOT include ICE candidates)
    await peerConnection.setLocalDescription(offer); // Begins the process of sending localDescription to the remote peer
    
    // Initially, the SDP offer will not include ICE candidates, but they will be added incrementally by the onicecandidate event
    document.getElementById("offer-sdp").value = JSON.stringify(offer); // Display the offer SDP in the UI
};

let createAnswer = async () => {    
    await createPeerConnection('answer-sdp');

    // Get offer text from box
    let offer = document.getElementById('offer-sdp').value

    // Prompt user to fill it in if there isn't one
    if(!offer) return alert('Retrieve offer from peer first...')

    // Parse the JSON
    offer = JSON.parse(offer)

    // Set the remote description equal to the offer
    await peerConnection.setRemoteDescription(offer)

    // Generate an answer for the offer
    let answer = await peerConnection.createAnswer()

    // Set the answer to the local description
    await peerConnection.setLocalDescription(answer)

    // Display the answer to the text box
    document.getElementById('answer-sdp').value = JSON.stringify(answer)

}

let addAnswer = async () => {
    // Get the answer from the text box
    let answer = document.getElementById('answer-sdp').value

    if(!answer) return alert('Retrieve answer from peer first...')
    
    answer = JSON.parse(answer)

    // If there isn't a remote description, set it to the answer
    if(!peerConnection.currentRemoteDescription){
        peerConnection.setRemoteDescription(answer)
    }

    // We now have our local and remote descriptions ready and we can start transmitting video!
}

init();

document.getElementById('create-offer').addEventListener('click', createOffer);
document.getElementById('create-answer').addEventListener('click', createAnswer);
document.getElementById('add-answer').addEventListener('click', addAnswer);
