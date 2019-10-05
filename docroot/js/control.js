/**
 * Переменные
 * 
 */
var descriptionJSON = {};


/**
 * Создает таблицу дескрипьора из данных в обекте и устанааливает ее в parent
 * @param {Обект описания одного сенсора} object 
 * @param {строка содержащая технический id=* } id
 * @param {элемент страници для размешения таблицы} idParent
 */


function createTable(object = {}, id = "", idParent = "") {
    if (object == {}) {
        return null;
    }
    if (idParent == "") {
        return null;
    }
    if (id == "") {
        return null;
    }

    let objectsArray = object.keys,
        lengthArray = objectsArray.length,
        nameTable = object.name,
        mainDiv = document.getElementById(idParent);

    //создаем элементы
    let skrollDiv = document.createElement('div'),
        divConteiner = document.createElement('div'),
        table = document.createElement('table'),
        caption = document.createElement('caption'),
        thead = document.createElement('thead'),
        tbody = document.createElement('tbody'),
        trHead = document.createElement('tr'),
        trAlias = document.createElement('tr'),
        hrLine = document.createElement('hr');

    hrLine.className = "line";
    caption.innerHTML = `<h> ${nameTable} </h>`;
    skrollDiv.className = "scroll-container";
    table.className = "table table-bordered description";
    table.id = id;
    trHead.id = "heads";
    trAlias.id = "alias";

    // Создание и астройка кнопки
    let divRow = document.createElement('div'),
        divCol = document.createElement('div'),
        buttonSend = document.createElement('button');

    divRow.className = "row-no-gutters";
    divCol.className = "col-sm-12 col-md-12";
    buttonSend.className = "btn btn-primary";
    buttonSend.name = id;
    buttonSend.innerHTML = "Save";
    buttonSend.addEventListener('click', handlerSendDescription);

    //Создание заголовков
    let alias = object.alias;
    for (const key in alias) {
        if (alias.hasOwnProperty(key)) {
            let th = document.createElement('th');

            //если строка пустая то подстовляем ключ иначе значение
            if (alias[key] == "") {
                th.innerHTML = key;
            } else {
                th.innerHTML = alias[key];
            }

            // если ключ это имя переменно то ставим его в начало
            if (key == 'name') {
                trHead.prepend(th);
            } else {
                trHead.append(th);
            }
        }
    }

    // помешаем настройку имен
    for (const key in alias) {
        if (alias.hasOwnProperty(key)) {
            const element = alias[key];
            let td = document.createElement('td');

            let input = document.createElement('input');
            input.type = 'text';
            input.name = key;
            if (element == "") {
                input.placeholder = "Сolumn name";
            }
            input.value = element;

            td.append(input);
            // если ключ это имя переменно то ставим его в начало
            if (key == 'name') {
                trAlias.prepend(td);
            } else {
                trAlias.append(td);
            }
        }
    }
    //Заполнение таблицы
    for (let i = 0; i < lengthArray; i++) {
        const element = objectsArray[i];
        let tr = document.createElement('tr');
        tr.id = element.name;
        for (const key in element) {
            if (element.hasOwnProperty(key)) {
                let td = document.createElement('td');

                //проверка на тип переменной
                let input = document.createElement('input');
                input.name = key;

                if (typeof (element[key]) == "string") {
                    if (key == 'color') {
                        input.type = 'color';
                    } else {
                        input.type = 'text';
                    }
                    input.value = element[key];

                } else if (typeof (element[key]) == "boolean") {
                    input.type = 'checkbox';
                    input.checked = element[key];

                } else if ((typeof (element[key]) == "number") || element[key] === null) {
                    input.type = 'number';
                    input.min = -1000000;
                    input.max = 1000000;
                    input.step = 1;
                    if (element[key] === null) {
                        // input.value = 0;
                    } else {
                        input.value = element[key];
                    }
                }
                td.append(input);
                // если ключ это имя переменно то ставим его в начало
                if (key == 'name') {
                    tr.prepend(td);
                } else {
                    tr.append(td);
                }
            }
            tbody.append(tr);
        }

        //Сборка и вставка таблицы 
        thead.append(trHead);
        table.append(caption);
        table.append(thead);
        tbody.append(trAlias);
        table.append(tbody);
        skrollDiv.append(table);
        divConteiner.append(skrollDiv);
        divConteiner.append(buttonSend);
        mainDiv.append(divConteiner);
        mainDiv.append(hrLine);

    }
}

/**
 * Создает таблицы с описанием сенсоров
 * @param {Обект должен содежать в параметрах 
 * обекты описания сенсоров} object 
 */
function createTables(object = {}) {
    for (const key in object) {
        if (object.hasOwnProperty(key)) {
            const element = object[key];
            createTable(element, key, "description");
        }
    }
}

/**
 * Получает из таблицы данные и возврошает их в JSON формате
 * @param {Переменная DOM таблицы} tableElement 
 * @returns{JSON objeckt объект в котором в качестве ключа выступает id=* 
 * в котором объект с настройками}
 */
function getDataFromTable(tableElement) {
    let id = tableElement.id;
    let trArr = $('tr', tableElement);

    let jsonSend = {},
        alias = {},
        keys = [],
        retObbj = {};

    for (let i = 0; i < trArr.length; i++) {
        const element = trArr[i];
        let tdArr = element.children,
            nodeArrKeys = {};

        if (element.id == "alias") {
            for (let i = 0; i < tdArr.length; i++) {
                const element = tdArr[i];
                alias[element.children[0].name] = element.children[0].value;
            }
        } else if (element.id != "heads") {
            for (let i = 0; i < tdArr.length; i++) {
                const element = tdArr[i].children[0];
                if (element.type == "text" || element.type == "color") {
                    nodeArrKeys[element.name] = element.value;
                } else if (element.type == "checkbox") {
                    nodeArrKeys[element.name] = element.checked;
                } else if (element.type == "number") {
                    if (element.value == "") {
                        nodeArrKeys[element.name] = null;
                    } else {
                        nodeArrKeys[element.name] = +element.value;

                    }
                }
            }
            keys.push(nodeArrKeys);
        }

    }
    retObbj.alias = alias;
    retObbj.name = tableElement.caption.textContent;
    retObbj.keys = keys;
    jsonSend[id] = retObbj;

    return jsonSend;

}

/**
 * Обработчик отправки одного дескриптора
 * @param {*} event 
 */
function handlerSendDescription(event) {
    console.log(event);


    //ищим все таблицы
    let tbl = document.getElementById(event.target.name);
    let json = {};

    let tempObj = getDataFromTable(tbl);
    let keys = Object.keys(tempObj);
    json[keys[0]] = tempObj[keys[0]];


    //отправка данных
    let send = JSON.stringify(json);
    $.ajax({
            url: '/data/deskript',
            type: 'POST',
            data: send,
        })
        .done(function (json) {
            console.log("ok");
        })
        .fail(function (xhr, status, errorThrown) {
            alert("Sorry, there was a problem!");
            console.log("Error: " + errorThrown);
            console.log("Status: " + status);
            console.dir(xhr);
        })
        .always(function (xhr, status) {});


}

/**
 * Обработчик отправки всех дескрипторов
 */
function handlerSendTables() {
    //ищим все таблицы
    let tbl = $('table.description');
    let json = {};

    for (let i = 0; i < tbl.length; i++) {
        const element = tbl[i];
        let tempObj = getDataFromTable(element);
        let keys = Object.keys(tempObj);
        json[keys[0]] = tempObj[keys[0]];
    }

    //отправка данных
    let send = JSON.stringify(json);
    $.ajax({
            url: '/data/deskript',
            type: 'POST',
            data: send,
        })
        .done(function (json) {
            console.log("ok");
        })
        .fail(function (xhr, status, errorThrown) {
            alert("Sorry, there was a problem!");
            console.log("Error: " + errorThrown);
            console.log("Status: " + status);
            console.dir(xhr);
        })
        .always(function (xhr, status) {});
}

/**
 * Обработчик создания нового дескриптора
 * @param {} event
 */
function addNewDeskription(event) {

    if (event.target.id == "addTable") {
        let inputs = $('input', event.currentTarget);

        let id = "id=",
            nameTable = "",
            countParam,
            alert = false,
            newDeskription = {};

        let rxId = /^id=\d{1,2}$/,
            rxName = /^[a-zA-Z]{1,20}$/,
            rxcountParam = /^\d{1,2}$/;

        for (let i = 0; i < inputs.length; i++) {
            const element = inputs[i];

            if (element.name == "countParam") {
                countParam = element.value;
                if (rxcountParam.test(countParam)) {
                    element.className = "form-control";
                } else {
                    element.className += " alert alert-danger";
                    alert = true;
                }

            } else if (element.name == "idNum") {
                id += element.value;
                if (rxId.test(id)) {
                    element.className = "form-control";
                } else {
                    element.className += " alert alert-danger";
                    alert = true;
                }

            } else if (element.name == "tableName") {
                nameTable = element.value;
                if (rxName.test(nameTable)) {
                    element.className = "form-control";
                } else {
                    element.className += " alert alert-danger";
                    alert = true;
                }

            }

        }
        if (!alert) {
            //создаем обект и вызываем создание таблицы
            let keys = Object.keys(descriptionJSON);
            let tAlias = descriptionJSON[keys[0]].alias;
            let tKeys = descriptionJSON[keys[0]].keys[0];

            /**
             * Создание шаблонов
             */
            // чистим алеасы
            for (const key in tAlias) {
                if (tAlias.hasOwnProperty(key)) {
                    tAlias[key] = "";
                }
            }

            // создаем шаблон для ключей
            for (const key in tKeys) {
                if (tKeys.hasOwnProperty(key)) {
                    const element = tKeys[key];
                    switch (typeof (element)) {
                        case 'string':
                            tKeys[key] = '';
                            break;
                        case 'boolean':
                            tKeys[key] = false;
                            break;
                        case 'object':
                            tKeys[key] = null;
                            break;
                        case 'number':
                            tKeys[key] = null;
                            break;

                        default:
                            break;
                    }
                }
            }

            // создаем ключи
            let arrKeys = [];
            for (let i = 0; i < +countParam; i++) {
                arrKeys.push(tKeys);
            }

            newDeskription = {
                alias: tAlias,
                tkeys: tKeys,
                keys: arrKeys,
                name: nameTable
            };
            createTable(newDeskription, id, "addDescription");
            $('#myModalBox').modal('hide');
        }

    } else {
        event.target.className = "form-control";
    }

}

// удаление дескриптора
function deletDeskription(id) {
    
}
// Начало начал 
document.addEventListener('DOMContentLoaded', function () {

    $.ajax({
            url: '/data/deskript',
            type: 'GET',
            dataType: 'json',

        })
        .done(function (json) {
            descriptionJSON = json;
            createTables(json);
        })
        .fail(function (xhr, status, errorThrown) {
            alert("Sorry, there was a problem!");
            console.log("Error: " + errorThrown);
            console.log("Status: " + status);
            console.dir(xhr);
        })
        .always(function (xhr, status) {});

    let saveAll = document.getElementById("saveAll");
    saveAll.addEventListener('click', handlerSendTables);

    let addNew = document.getElementById("addNew");
    addNew.addEventListener('click', function () {
        $("#myModalBox").modal('show');
    });

    document.getElementById("myModalBox").addEventListener('click', addNewDeskription);

});