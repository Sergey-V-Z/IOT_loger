var xhr = new XMLHttpRequest();
let xhrUsers = new XMLHttpRequest();
let userList = document.getElementById("userlist"),
    delet = document.getElementById("delet"),
    registr = document.getElementById("registr"),
    checkBoxUpdate = document.getElementById("updatepages"),
    inputIntervalTime = document.getElementById("intervalTime"),
    inputFooterTime = document.getElementById("footerTime"),
    statusBar = document.getElementById("footDiv");
var users, listLogFile;
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
    delet.addEventListener('click', usersEvent);
    registr.addEventListener('click', usersEvent);
    checkBoxUpdate.addEventListener('click', hendlerIntervalTime);
    inputIntervalTime.addEventListener('click', hendlerIntervalTime);
    inputFooterTime.addEventListener('click', hendlerFooterTimer);
});