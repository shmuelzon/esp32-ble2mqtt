const RESTART = '/restart';
const OTA_UPLOAD = '/ota/';
const BLE_DB = '/ble/bonding_db';
const BLE_DEVICES = '/ble/devices';
const CONFIG_FILE_PATH = '/fs/config.json';
const LATEST_RELEASE = 'https://github.com/shmuelzon/esp32-ble2mqtt/releases';
const STATUS = '/status';

function progress(show = true) {
    document.getElementById('progress').style.display = show ? 'flex' : 'none';
    // disable\enable all buttons
    [].concat(
        Array.from(document.getElementsByTagName('button')), //all buttons
        Array.from(document.getElementsByClassName('button'))  //all elements same button
    )
        .map(item => {
            if (show) item.setAttribute('disabled', show)
            else item.removeAttribute('disabled')
        });
}

function toaster(message) {
    document.getElementById('message').style.display = 'block';
    document.getElementById('message').innerText = message;
    setTimeout(() => document.getElementById('message').style.display = 'none', 3000);
}

function restart() {
    fetch(RESTART, {
        method: 'POST',
    })
        .then((res) => {
            window.open("/busy.html", "_self")
        })
        .catch((err) => {
            toaster("Can't restart");
            console.error(err)
        })
}

//
//API not yet implemented
//
function getStatus() {
    fetch(STATUS)
        .then(response => response.json())
        .then(json => {
            document.getElementById('software-version').innerHTML = json.version;
        });
}

function getLatestReleaseInfo() {
    fetch('https://api.github.com/repos/shmuelzon/esp32-ble2mqtt/tags')
        .then(response => response.json())
        .then(json => {
            let release = json[0];
            if (release) document.getElementById('latest-release-id').innerHTML = `(latest <a href="${LATEST_RELEASE}" target="_blank">${release.name}</a>)`;
        });
}

function otaStartUpload(type) {
    let file = document.getElementById(type + '-file').files[0];
    if (!file)
        return;
    progress(true);
    fetch(OTA_UPLOAD + type, {
        method: 'POST',
        headers: {
            'Content-Type': 'application/octet-stream'
        },
        body: file
    })
        .then((res) => {
            toaster("Upload complete");
            window.open("/busy.html", "_self")
        })
        .catch((err) => {
            toaster(`Can't upload ${type}. Please refresh page`);
            console.error(err)
        })
}

function bleClearBonding() {
    progress(true);
    fetch(BLE_DB, {
        method: 'DELETE',
    })
        .then((res) => {
            progress(false);
            toaster("Cleared");
            console.log("cleared")
        })
        .catch((err) => {
            toaster("Can't clear ble");
            console.error(err)
        })
}

function downloadConfig() {
    fetch(CONFIG_FILE_PATH, {
        method: 'GET',
        cache: 'no-store'
    })
        .then(response => response.json())
        .then(json => {
            document.getElementById('config-file').innerHTML = JSON.stringify(json, null, 2);
        })
        .catch((err) => {
            toaster("Can't download config file");
            console.error(err)
        })
}

function uploadConfigFile() {
    let file = new File([document.getElementById('config-file').value], "config.json", {
        type: "text/plain",
    });
    if (file.size === 0) {
        toaster("File is empty");
        return;
    }
    progress(true);
    fetch(CONFIG_FILE_PATH, {
        method: 'POST',
        headers: {
            'Content-Type': 'application/octet-stream'
        },
        body: file
    })
        .then(response => {
            restart();
        })
        .catch((err) => {
            toaster("Can't upload config file");
            console.error(err)
        })
}

//
//API not yet implemented
//
function bleListUpdate() {
    progress(true);
    fetch(BLE_DEVICES, {
        method: 'GET',
    })
        .then(response => {
            if (!response.ok) {
                throw new Error("HTTP status " + response.status);
            }
            return response.json();
        })
        .then(json => {
            progress(false);
            // render BLE table
            document.getElementById('ble-list').innerHTML = json.map(item => {
                return `
                <tr>
                    <td ${item.connected ? 'class="tertiary" title="connected"' : 'title="disconnected"'}>${item.name}</td>
                    <td>${item.mac}</td> 
                </tr>`;
            }).join('\n');
        })
        .catch((err) => {
            progress(false);
            toaster("Can't update list of ble");
            console.error(err)
        })
}


function loadFileManager(path) {
    path ||= document.getElementById('file-manager-path').innerText;

    fetch('/fs' + path,
        {
            method: "GET",
        })
        .then(response => {
            return response.json();
        })
        .then(data => {
            document.getElementById('file-manager-list').innerHTML =
                `<tr>
                 <td><span style="cursor: pointer; font-weight: bold" title="Up" onclick="loadFileManager('${path}')">..</span><span id="file-manager-path" style="cursor: pointer;">${path}</span></td>   
                 <td></td>   
                 <td></td>   
                </tr>` +
                data.map(entry => {
                    let name, del, download = '';
                    if (entry.type === 'directory') {
                        name = `<span style="cursor: pointer" onclick="loadFileManager('${path + '/' + entry.name}')">📁 ${entry.name}</span>`;
                    } else {
                        name = entry.name;
                        del = `<span class="icon-delete" title="delete file" style="cursor: pointer" onclick="deleteFile('${path + entry.name}')"></span>`;
                        download = `<a href="/fs${path}${entry.name}" target="_blank"><span class="icon-link" title="download file"></span></a>`;
                    }
                    return `<tr>
                            <td>${name}</td>
                            <td>${entry.type === 'file' ? entry.size : ''}</td>
                            <td>${del} 
                                ${download}
                            </td>
                            </tr>`
                }).join('\n')

        })
        .catch(err => {
                toaster(`Can't get ${path}`);
                console.error(err);
            }
        )
}

function uploadFile() {
    let file = document.getElementById('file-manager-file').files[0];
    if (!file) {
        toaster("File is empty");
        return;
    }
    let path = document.getElementById('file-manager-path').innerText;
    progress(true);
    fetch('/fs' + path + file.name, {
        method: 'POST',
        headers: {
            'Content-Type': 'application/octet-stream'
        },
        body: file
    })
        .then(response => {
            toaster("OK");
            progress(false);
            loadFileManager(path);
        })
        .catch((err) => {
            toaster("Can't upload file");
            console.error(err)
        })
}

function deleteFile(fullName) {
    if (!fullName) {
        return;
    }
    fetch('/fs' + fullName, {
        method: 'DELETE',
        headers: {
            'Content-Type': 'application/octet-stream'
        },
    })
        .then(response => {
            toaster("OK");
            progress(false);
            loadFileManager();
        })
        .catch((err) => {
            toaster("Can't delete file");
            console.error(err)
        })
}

document.addEventListener("DOMContentLoaded", event => {
    getStatus();            // device status
    getLatestReleaseInfo(); // get github version
    bleListUpdate()         // update ble table
});