var menuFunc, LogOut;

var keySettingsMessage = "settingsmessage";
var SettingsMessage,
    footer,
    tempStr,
    timerFooter;

if ((tempStr = localStorage.getItem(keySettingsMessage)) != null) {
    SettingsMessage = JSON.parse(tempStr);
} else {
    let tempSettings = {
        timerFooter: 3
    };
    localStorage.setItem(keySettingsMessage, JSON.stringify(tempSettings));
}
//вывод сообшения в футер
function setMessageFooter(message) {
    footer.innerHTML = "";
    footer.innerHTML = "" + message;
    timerFooter = setTimeout(clearMessageFooter, SettingsMessage.timerFooter * 1000);
}

//очистка футера по таймеру
function clearMessageFooter() {
    footer.innerHTML = "";
    clearTimeout(timerFooter);
}

    // обработчик событий в localSorage
    window.addEventListener('storage', function (event) {
        //если событие по ключу keySettingsTimer
        if (event.key === keySettingsMessage) {
            SettingsMessage = JSON.parse(event.newValue);
        }
    });


document.addEventListener('DOMContentLoaded', function () {
    let topMenu = document.getElementById("Topnav");

    menuFunc = function () {
        if (topMenu.className === "topnav") {
            topMenu.className += " responsive";
        } else {
            topMenu.className = "topnav";
        }
    };

    let url = document.URL,
        path = url.split('/'),
        count = path.length;

    let itemMenu = document.getElementById(path[count - 1]),
        menu = document.getElementById("menu");

    // let main = document.getElementsByClassName("main")[0];

    menu.innerHTML = itemMenu.innerHTML;
    itemMenu.classList.add("active");

    footer = document.getElementById("footdiv");
});