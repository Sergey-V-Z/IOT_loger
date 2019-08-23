var xhr = new XMLHttpRequest();
let xhrUsers = new XMLHttpRequest();
let userList = document.getElementById("userlist"),
    delet = document.getElementById("delet"),
    registr = document.getElementById("registr"),
    checkBoxUpdate = document.getElementById("updatepages"),
    inputIntervalTime = document.getElementById("intervalTime"),
    inputFooterTime = document.getElementById("footerTime"),
    statusBar = document.getElementById("footDiv"),
    setcharts = document.getElementById("setcharts");
var users, requestList, listLogFile;
var arrFiles = [];
var selects = [];
var keySettingsTimer = "settingsTimer";
/**
 * класс настроек таймера
 */
// class refreshTimer {
//     constructor(refresh, refreshTime) {
//         this.refresh = refresh;
//         this.refreshTime = refreshTime;
//     }
// }

/**
 * обработчик данных с сервера
 */
function handlerRequest() {
    if (requestList.readyState === 4 && requestList.status === 200) {

        let type = requestList.getResponseHeader('Content-Type');
        let rawText = requestList.responseText;
        listLogFile = JSON.parse(rawText);

        for (const key in listLogFile) {
            if (listLogFile.hasOwnProperty(key)) {
                arrFiles.push(listLogFile[key]);
            }
        }
        var time = performance.now();
        console.log(time);
        if (arrFiles.length != 0) {
            // тут сортировку или добавить ее на сервере
            let listID = {};
            for (let i = 0; i < arrFiles.length; i++) {
                let tempsStr = "" + arrFiles[i];
                let arrFind = tempsStr.match(/id=[0-9]{0,2}/i);
                if (arrFind.length > 1) {
                    console.log(`error name file: ${tempsStr}`);
                } else {
                    // подумать над расстлсовкой в одном цикле
                    if (typeof listID[arrFind[0]] !== "undefined") {
                        //ключ есть
                        listID[arrFind[0]].push(tempsStr);
                    } else {
                        //ключа нет
                        listID[arrFind[0]] = [];
                        listID[arrFind[0]].push(tempsStr);
                    }
                }
            }
            var i = 0;
            //таблица
            for (const key in listID) {
                let row = document.createElement("tr");
                var cell1 = document.createElement("td");
                var cell2 = document.createElement("td");
                let checkBox = document.createElement('input');
                checkBox.type = 'checkbox';

                if (listID.hasOwnProperty(key)) {
                    let arr = listID[key];
                    checkBox.id = key;
                    cell1.appendChild(checkBox);
                    selects[i] = document.createElement("select");
                    selects[i].className = "listfiles";

                    arr.forEach(element => {
                        let option = document.createElement("option");
                        option.innerHTML = element;
                        option.value = element;
                        selects[i].appendChild(option);
                    });
                    
                    cell2.appendChild(selects[i]);
                    row.appendChild(cell1);
                    row.appendChild(cell2);
                }
                i++;
                setcharts.appendChild(row);
            }

        }
        console.log(performance.now() - time);
    }
}
//обработчик input FooterTime
function hendlerFooterTimer(event) {
    let strObj = localStorage.getItem(window.keySettingsMessage);
    if (strObj != null) {
        let temp = JSON.parse(strObj);
        temp.timerFooter = +event.target.value;
        localStorage.setItem(window.keySettingsMessage, JSON.stringify(temp));
    } else {
        console.log("SettingsMessage is not defined");
    }

}
//обработчик нажатий на чекбокс
function hendlerIntervalTime(event) {
    let strObj = localStorage.getItem(keySettingsTimer);
    if (strObj != null) {
        let temp = JSON.parse(strObj);
        if (event.target == checkBoxUpdate) {
            if (event.target.checked) {
                temp.refresh = true;
            } else {
                temp.refresh = false;
            }
        } else if (event.target == inputIntervalTime) {
            temp.refreshTime = +event.target.value;
        }
        localStorage.setItem(keySettingsTimer, JSON.stringify(temp));
    } else {
        console.log("localStorage is empty");
    }
}
//чтение localStorage и выставление параметров на странице
function getSetSettings() {
    let strObj = localStorage.getItem(keySettingsTimer);
    if (strObj != null) {
        let temp = JSON.parse(strObj);
        checkBoxUpdate.checked = temp.refresh;
        inputIntervalTime.value = temp.refreshTime;
    } else {
        console.log("localStorage is empty");
        window.footer.innerText = "localStorage is empty";
    }

    strObj = localStorage.getItem(window.keySettingsMessage);
    if (strObj != null) {
        let temp = JSON.parse(strObj);
        inputFooterTime.value = temp.timerFooter;
    } else {
        console.log("localStorage is empty");
        window.footer.innerText = "localStorage is empty";
    }

}
// Обработчик нажатия кнопок регистрации и удаления
function usersEvent(e) {
    // создать объект для формы
    let formData = new FormData(e.currentTarget.form);

    // добавить к пересылке ещё пару ключ - значение
    formData.append("setuser", e.target.id);

    let pas = formData.get("password");
    if (pas != null) {
        let hash = window.hex_sha256(pas);
        formData.set("password", hash);
    }

    // отослать
    xhr.open("POST", "/login.html");
    xhr.onload = function () {
        let temp = JSON.parse(xhr.responseText);
        let keys = Object.keys(temp);
        if (keys[0] == "respons") {
            // console.log(temp.respons);
            if (xhr.status === 200) {
                getUsers();
            }
        } else {
            // console.log(temp);
        }

    };
    xhr.send(formData);

}
// запрос всех файлов логирования
function getFilesList() {
    /**
     * Запрос данных с сервера
     */
    requestList = new XMLHttpRequest();
    requestList.open('GET', '/vf/allFile');
    requestList.onreadystatechange = handlerRequest;
    requestList.send();

}
// Запрос списка ползователе и занесение их в список после ответа
function getUsers() {
    xhrUsers.open("POST", "/login.html");
    xhrUsers.setRequestHeader('Content-Type', 'application/x-www-form-urlencoded');
    xhrUsers.addEventListener('load', function () {
        users = JSON.parse(xhrUsers.responseText);
        userList.innerHTML = "";
        for (const key in users) {
            if (users.hasOwnProperty(key)) {
                let element = document.createElement("option");
                element.innerHTML = key;
                element.value = key;
                userList.appendChild(element);
            }
        }
        // xhrUsers = null;
    });
    xhrUsers.send("setuser=users");

}
document.addEventListener('DOMContentLoaded', function () {
    getUsers();
    getSetSettings();
    getFilesList();
    delet.addEventListener('click', usersEvent);
    registr.addEventListener('click', usersEvent);
    checkBoxUpdate.addEventListener('click', hendlerIntervalTime);
    inputIntervalTime.addEventListener('click', hendlerIntervalTime);
    inputFooterTime.addEventListener('click', hendlerFooterTimer);
});