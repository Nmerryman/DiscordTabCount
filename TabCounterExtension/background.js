function send(text) {
    const req = new XMLHttpRequest();
    req.open("GET", "http://127.0.0.1:4455/log/" + text)
    req.send();

}

function sendNum(num) {
    const req = new XMLHttpRequest();
    req.open("GET", "http://127.0.0.1:4455/" + num)
    req.send();

   
}

function updateItems(wIA) {
    let count = 0;
    items = []
    for (const wInfo of wIA) {
        count += Object.keys(wInfo.tabs).length
    }
    sendNum(count)
}

function err(a) {}

function sleep(ms) {
    return new Promise(resolve => setTimeout(resolve, ms));
} 

send("------")

async function prog() {
    while (true) {
        // send(i);
        browser.windows.getAll({populate: true, windowTypes: ["normal"],}).then(updateItems, err)
        await sleep(2000);
    }
}

prog();  // Uncoment and reload to start sending updates to python server
