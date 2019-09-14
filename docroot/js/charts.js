'use strict';

// Set theme
window.am4core.useTheme(window.am4themes_animated);

var setcharts = document.getElementById("setcharts"),
  container = document.getElementById("container"),
  mainDiv = document.getElementById("main");
var logData;
var currentPoint, currentSettings;

var selects = [];


/**
 * Ищет с в строке id=nnn n-цифры и возврощает цыфру из id
 * @param {Где искать} stringForSearch
 * @param {Тип возврощаемых данных} fullReturn
 */
function idSearch(stringForSearch, fullReturn = false) {

  let arrFind = stringForSearch.match(/id=\d{1,2}/i); // looking for id
  if (arrFind.length != 1) {
    console.log(`incorrect file name: ${stringForSearch}`);
  } else {
    let idNum = arrFind[0].match(/\d{1,2}/); //looking for number
    if (idNum.length != 1) {
      console.log(`incorrect id : ${stringForSearch}`);
    } else {
      if (fullReturn === false) {
        return idNum[0];
      } else {
        return {
          "string": arrFind[0],
          "number": idNum[0]
        };
        // return idNum[0];
      }
    }
  }
}
/**
 * Обработчик кликов по таблице настроек
 */
var hendlerClickTable = function () {
  
};

/**
 * Обработчик данных с сервера 
 * Запрос списка файлов
 */
function handlerListFiles(params) {
  let listID = {};
  let arrFiles = JSON.parse(params);

  if (arrFiles.length != 0) {

    //Сортируем массив по id и укладываем массивы в объект 
    for (let i = 0; i < arrFiles.length; i++) {
      let tempsPath = "" + arrFiles[i];

      let idNum = idSearch(tempsPath);
      // разбор массива на отдельные массивы и установка их в объект
      if (typeof listID[idNum] !== "undefined") {
        //ключ есть
        listID[idNum].push(tempsPath);
      } else {
        //ключа нет
        listID[idNum] = [];
        listID[idNum].push(tempsPath);
      }
    }

    var i = 0;
    // создание таблицы с селектами и чекбоксами в нутри
    let table = document.createElement("table"),
      nameRow = document.createElement("tr");
    table.className = "tableCharts";
    nameRow.innerHTML = "<th></th><th>ID</th><th>Файлы</th>";
    // caption = document.createElement("caption");
    // caption.innerText = 
    table.appendChild(nameRow);
    for (const key in listID) {
      let row = document.createElement("tr"),
        cell1 = document.createElement("td"),
        cell2 = document.createElement("td"),
        cellID = document.createElement("td"),
        checkBox = document.createElement('input');
      checkBox.type = 'checkbox';

      if (listID.hasOwnProperty(key)) {
        let arr = listID[key];
        checkBox.id = 'id=' + key;
        let Str = "" + key;
        cellID.innerHTML = key;
        cell1.appendChild(checkBox);
        selects[i] = document.createElement("select");
        selects[i].className = "listfiles";

        // Заполнение select
        arr.forEach(element => {
          let option = document.createElement("option");
          let Find = element.match(/[0-9]{2}.[0-9]{2}.[0-9]{2}/i);
          if (Find.length > 1) {
            console.log(`error name file: ${element}`);
            option.innerHTML = "err file name";
          } else {
            option.innerHTML = Find[0];
          }

          option.value = element;
          selects[i].appendChild(option);

        });

        cell2.appendChild(selects[i]);
        row.appendChild(cell1);
        row.appendChild(cellID);
        row.appendChild(cell2);
      }
      i++;
      table.appendChild(row);
    }
    setcharts.appendChild(table);

  }
  return listID; // возврощаем обект с данными для дальнейших работы
}

//скачивает файлы из переданного списка с сервера
function uploadsFiles(fileNameArr = []) {
  let i = 0;
  let n = fileNameArr.length;
  let storFiles = {};

  return new Promise(function (resolve, reject) {
    let request = new XMLHttpRequest();
    request.open('GET', fileNameArr[i]);
    request.setRequestHeader('Content-Type', 'application/json; charset=utf-8');
    request.onreadystatechange = function () {
      if (request.readyState === 4) {
        if (request.status == 200 && request.status < 300) {
          let idStr = idSearch(fileNameArr[i], true);
          storFiles[idStr.string] = {
            "idNumber": idStr.number,
            "idElementDOM": "id" + idStr.number,
            "path": fileNameArr[i],
            "data": '[' + request.responseText + ']'
          };
          if (i < n - 1) {
            i++;
            request.open('GET', fileNameArr[i]);
            request.send();
          } else {
            // console.log(storFiles);
            // start drawing
            // createChart(JSON.parse(storFiles[fileNameArr[1]]));
            resolve(storFiles);
          }
        } else {
          reject();
        }
      }
    };
    request.send();
  });
}

/**
 * Загружает первые файлы из сортированного списка
 * @param { обект со списками файлов на сервере} params 
 */
function loadingFileLog(params) {
  let fileNameArr = [];
  for (const key in params) {
    if (params.hasOwnProperty(key)) {
      fileNameArr.push(params[key][0]);

    }
  }
  // запрос на сервер, возврощает промис
  return uploadsFiles(fileNameArr);
}
/**
 * Запрашивает описание сенсоров и возврощает обект в котором содержатся описания и файлы сенсоров
 * @param {переменная хранящая файлы данных сенсоров} storage 
 */
function getDescriptions(storage) {
  return new Promise(function (resolve, reject) {
    let request = new XMLHttpRequest();
    request.open('GET', "/data/deskript");
    request.setRequestHeader('Content-Type', 'application/json; charset=utf-8');
    request.onreadystatechange = function () {
      if (request.readyState === 4) {
        if (request.status == 200 && request.status < 300) {
          let descripions = JSON.parse(request.responseText);
          if (descripions === undefined) {
            reject();
          } else {
            resolve({
              "storage": storage,
              "descripions": descripions
            });
          }
        } else {
          reject();
        }
      }
    };
    request.send();
  });
}
/** Отрисовка графиков
 *
 * dataForCharts = {
 * "storage": {
 *          "id=n" :{ "path": ,
 *                    "data": 
 *                  },
 *          "id=n" :{ "path": ,
 *                    "data": 
 *                  }
 *             }
 * "descripions": {
 *                  id=n" : {}
 *                  id=n" : {}
 *                }
 *   }
 */
function drawing(dataForCharts = {}) {
  console.log(dataForCharts);
  let charts = {};

  for (const key in dataForCharts.storage) {
    if (dataForCharts.storage.hasOwnProperty(key)) {
      const element = dataForCharts.storage[key];

      let divCont = document.createElement("div");
      divCont.className = "container";

      // создать div в контейнере
      dataForCharts.storage[key].div = document.createElement("div");
      dataForCharts.storage[key].div.className = "chart";
      dataForCharts.storage[key].div.id = dataForCharts.storage[key].idElementDOM;

      divCont.appendChild(dataForCharts.storage[key].div);
      mainDiv.appendChild(divCont);
      charts[key] = createChart(JSON.parse(dataForCharts.storage[key].data), dataForCharts.storage[key].idElementDOM, dataForCharts.descripions[key]);
    }
  }
}
/**
 * Запрос списка файлов и затем скачивание файлов логирования 
 * */
function getFilesList() {

  function postData() {
    return new Promise(function (resolve, reject) {
      let requestList = new XMLHttpRequest();
      requestList.open('GET', '/vf/allFile');
      requestList.setRequestHeader('Content-Type', 'application/json; charset=utf-8');
      requestList.onreadystatechange = function () {
        if (requestList.readyState === 4) {
          if (requestList.status == 200 && requestList.status < 300) {
            resolve(requestList.responseText);
          } else {
            reject();
          }
        }
      };
      requestList.send();
    });
  }
  postData()
    .then(handlerListFiles) // после ответа сервера обробатываем ответ и зоздаем таблицу с селектами
    .then(loadingFileLog) // загружаем самый свежий файл с данными
    .then(getDescriptions)
    .then(drawing)
    .catch();

}

/**
 * Запрос данных с сервера
 */
document.addEventListener('DOMContentLoaded', function () {
  getFilesList();
});

/**
 * создает серию и устанавливает ее 
 */
function setSeries(pChart, nameSeries, dateX, valueY, min, max, first = false) {
  let valueAxis = pChart.yAxes.push(new window.am4charts.ValueAxis());
  if (first === true) {
    valueAxis.tooltip.disabled = true;
  } else {
    valueAxis.tooltip.disabled = true;
    valueAxis.renderer.opposite = true; // расположение шкалы
    valueAxis.renderer.grid.template.disabled = true;
    // valueAxis.disabled = true; //отключение подписи осей

  }
  if (min !== undefined || min != null) {
    valueAxis.min = min;
  }
  if (max !== undefined || min != null) {
    valueAxis.max = max;
  }

  let series = pChart.series.push(new window.am4charts.LineSeries());
  series.dataFields.dateX = dateX;
  series.dataFields.valueY = valueY;
  series.yAxis = valueAxis;
  series.tooltipText = "{valueY.value}";
  series.name = nameSeries;
  series.sequencedInterpolation = true;
  if (first != true) {
    series.hidden = true;
  }
  return series;
}

/**
 * Отрисовка графика
 * @param {*} data 
 * @param {*} idElement 
 * @param {description sensor} sett 
 */
function createChart(data, idElement, sett) {
  if (idElement == undefined || idElement == '') {
    console.error('IDelement is empty');
    return;
  }
  if (sett == undefined) {
    console.error('sett is empty');
    return;
  }
  let series;
  let chart = window.am4core.create(idElement, window.am4charts.XYChart);
  chart.data = data;
  //chart.dataDateFormat = "JJ:NN:SS";
  //формат разбора данных из строки
  chart.dateFormatter.inputDateFormat = "HH:mm:ss";

  let dateAxis = chart.xAxes.push(new window.am4charts.DateAxis());
  dateAxis.renderer.grid.template.location = 0;
  dateAxis.renderer.ticks.template.length = 8;
  dateAxis.renderer.ticks.template.strokeOpacity = 0.1;



  for (let i = 0; i < sett.keys.length; i++) {
    const element = sett.keys[i];
    if (element.display == false) {
      continue;
    }
    if (element.first == true) {
      series = setSeries(chart, element.name, "Time", element.name, element.min, element.max, element.first);
    } else {
      setSeries(chart, element.name, "Time", element.name, element.min, element.max, element.first);
    }
  }

  chart.cursor = new window.am4charts.XYCursor();
  chart.cursor.xAxis = dateAxis;
  chart.cursor.behavior = "non"; // выделение области

  let scrollbarX = new window.am4charts.XYChartScrollbar();
  scrollbarX.series.push(series);
  chart.scrollbarX = scrollbarX;

  chart.legend = new window.am4charts.Legend();
  chart.legend.parent = chart.plotContainer;
  chart.legend.zIndex = 100;
  chart.legend.valueLabels.template.text = "{valueY.value.formatNumber('$#.')}";
  return chart;
}