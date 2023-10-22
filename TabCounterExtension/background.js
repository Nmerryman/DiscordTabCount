function send(text) {
    // Send text message to debug side
    const req = new XMLHttpRequest();
    req.open("GET", "http://127.0.0.1:4455/log/" + text)
    req.send();

}

function sendNum(num) {
    // Send number to main update route
    const req = new XMLHttpRequest();
    req.open("GET", "http://127.0.0.1:4455/" + num)
    req.send();

   
}

function updateItems(wIA) {
    // Count the number of tabs currently open and then send them to the python server
    let count = 0;
    items = []
    for (const wInfo of wIA) {
        count += Object.keys(wInfo.tabs).length
    }
    sendNum(count)
}

// These two are helpers
function err(a) {}

function sleep(ms) {
    return new Promise(resolve => setTimeout(resolve, ms));
} 

// Did we connect to the Python server?
send("------")

// Check for current count every 3 seconds
async function prog() {
    while (true) {
        // send(i);
        browser.windows.getAll({populate: true, windowTypes: ["normal"],}).then(updateItems, err)
        await sleep(3000);
    }
}

prog();  // Uncoment and reload to start sending updates to python server
