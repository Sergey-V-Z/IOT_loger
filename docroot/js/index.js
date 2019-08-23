window.onload = function () {

    /**
     * Работа со строницей: создание обектов на странице, создание запросов
     * и их обработчики
     */
    var div = document.getElementById("main");

    // сосздание и заполнение таблицы
    function setTable(objPoint, domElement) {

        for (const key in objPoint) {
            // таблица для обекта
            let table = document.createElement("table"),
                tbody = document.createElement('tbody');
                table.className = "sensor";
            if (objPoint.hasOwnProperty(key)) {

                table.innerHTML = "<caption><p>" + key + "</p></caption>";
                // выносиб отдельный обект для удобства
                let sensObj = objPoint[key];
                // второй перебор обекта из предыдушего обекта
                for (const key2 in sensObj) {
                    if (sensObj.hasOwnProperty(key2)) {
                        if (key2 == "id=") {
                            continue;
                        }
                        //let rowTable = "<tr><td>" + key2 + "</td><td>" + sensObj[key2] + "</td><tr>";
                        //table.innerHTML += rowTable;
                        let rowTable = document.createElement('tr'),
                            cell1 = document.createElement('td'),
                            cell2 = document.createElement('td');
                        cell1.innerHTML = key2;
                        cell2.innerHTML = sensObj[key2];
                        rowTable.appendChild(cell1);
                        rowTable.appendChild(cell2);
                        tbody.appendChild(rowTable);
                    }
                }
                table.appendChild(tbody);
                domElement.appendChild(table);
            }
        }
    }

    // создание обекта для запроса точки и обработчик ответа сервера
    var xhr = new XMLHttpRequest();
    xhr.open("GET", "/vf/curentpoint");
    xhr.onload = function () {
        div.innerHTML = "";
        let pointObj = JSON.parse(xhr.responseText);
        setTable(pointObj, div);

    };
    xhr.send();

    // запрос точки
    function refresh() {
        xhr.open("GET", "/vf/curentpoint");
        xhr.send();
    }

    /**
     * Чтение настроек из localStorage
     * и применение их к странице,
     * а также установка обработчиков событий в localStorage
     */
    class refreshTimer {
        constructor(refresh, refreshTime) {
            this.refresh = refresh;
            this.refreshTime = refreshTime;
        }
    }

    var intervalTimer,
        settingsTimer = new refreshTimer(false, 5),
        keySettingsTimer = "settingsTimer";
    // console.log(localStorage.length);

    // Настройка таймера из localStorage
    function intervalTimerSet() {
        // разрешение на обновление данных
        let strObj = localStorage.getItem(keySettingsTimer);
        if (strObj == null) {
            localStorage.setItem(keySettingsTimer, JSON.stringify(settingsTimer));
            settingsTimer = JSON.parse(localStorage.getItem(keySettingsTimer));
        } else {
            settingsTimer = JSON.parse(strObj);
        }

        // если разрешено обновление то настраиваем таймер
        if (settingsTimer.refresh) {
            //запуск таймера
            intervalTimer = setInterval(refresh, settingsTimer.refreshTime * 1000);
        }
    }
    // обработчик событий в localSorage
    window.addEventListener('storage', function (event) {
        //если событие по ключу keySettingsTimer
        if (event.key === keySettingsTimer) {
            settingsTimer = JSON.parse(event.newValue);
            if (settingsTimer.refresh) {
                clearInterval(intervalTimer);
                intervalTimer = setInterval(refresh, settingsTimer.refreshTime * 1000);
            } else {
                clearInterval(intervalTimer);
            }
            if (settingsTimer.refresh) {
                window.setMessageFooter("Автообновление включено");
            } else {
                window.setMessageFooter("Автообновление отключено");
            }

        }
    });

    // запуск таймера
    intervalTimerSet();

    // показ состояния на странице
    if (settingsTimer.refresh) {
        window.setMessageFooter("Автообновление включено");
    } else {
        window.setMessageFooter("Автообновление отключено");
    }

};
//обработчик чекбокса
// update.addEventListener('click', function () {
//     if (!update.checked) {
//         intervalTime.disabled = false;
//         clearInterval(intervalTimer);
//     } else {
//         intervalTime.disabled = true;
//         intervalTimer = setInterval(refresh, intervalTime.value * 1000);
//     }
// });