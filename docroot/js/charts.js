'use strict';

// Set theme
window.am4core.useTheme(window.am4themes_animated);

let setcharts = document.getElementById("setcharts"),
  container = document.getElementById("container");
var logData;
var currentPoint, currentSettings;
var storFiles = {};
var selects = [];

/**
 * Обработчик данных с сервера 
 * Запрос списка файлов
 */
function handlerListFiles(params) {
  let listID = {},
    arrFiles = [];
  let listLogFile = JSON.parse(params);

  // Преобразуем в массив
  for (const key in listLogFile) {
    if (listLogFile.hasOwnProperty(key)) {

      arrFiles.push(listLogFile[key]);
    }
  }

  // var time = performance.now();
  // console.log(time);

  if (arrFiles.length != 0) {

    //преобразуем из массива в обект
    for (let i = 0; i < arrFiles.length; i++) {
      let tempsStr = "" + arrFiles[i];
      let arrFind = tempsStr.match(/id=[0-9]{0,2}/i);

      if (arrFind.length != 1) {
        console.log(`incorrect file name: ${tempsStr}`);
      } else {
        let idNum = arrFind[0].match(/\d{1,2}/);
        if (idNum.length != 1) {
          console.log(`incorrect id : ${tempsStr}`);
        } else {
          // разбор массива на отдельные массивы и установка их в объект
          if (typeof listID[idNum[0]] !== "undefined") {
            //ключ есть
            listID[idNum[0]].push(tempsStr);
          } else {
            //ключа нет
            listID[idNum[0]] = [];
            listID[idNum[0]].push(tempsStr);
          }
        }
      }

    }

    var i = 0;
    //таблица
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
  return listID;
}

//скачивает список файлов с сервера
function uploadsFiles(fileNameArr = []) {
  let i = 0;
  let n = fileNameArr.length;
  let request = new XMLHttpRequest();
  request.open('GET', fileNameArr[i]);
  request.onload = function () {
    if (request.status == 200 && request.status < 300) {
      storFiles[fileNameArr[i]] = '[' + request.responseText + ']';
      if (i < n - 1) {
        i++;
        request.open('GET', fileNameArr[i]);
        request.send();
      } else {
        console.log(storFiles);
        // start drawing

        createChart(JSON.parse(storFiles[fileNameArr[1]]));
      }
    } else {
      console.log("uploads files");
    }
  };
  request.send();

}

function loadingFileLog(params) {
  let fileNameArr = [];
  for (const key in params) {
    if (params.hasOwnProperty(key)) {
      fileNameArr.push(params[key][0]);

    }
  }
  // запрос на сервер 
  uploadsFiles(fileNameArr);
}
/**
 * Запрос всех файлов логирования 
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
    .then(handlerListFiles)
    .then(loadingFileLog)
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
    valueAxis.renderer.opposite = true;
    valueAxis.renderer.grid.template.disabled = true;

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
//отрисовка графиков
function createChart(data, idElement = '', sett = {}) {
  // if(idElement === ''){
  //   console.error('IDelement is empty');
  //   return;
  // }
  // if(sett === {}){
  //   console.error('sett is empty');
  //   return;
  // }
  var chart = window.am4core.create('chartdiv', window.am4charts.XYChart);
  chart.data = data;
  //chart.dataDateFormat = "JJ:NN:SS";
  //формат разбора данных из строки
  chart.dateFormatter.inputDateFormat = "HH:mm:ss";

  var dateAxis = chart.xAxes.push(new window.am4charts.DateAxis());
  dateAxis.renderer.grid.template.location = 0;
  dateAxis.renderer.ticks.template.length = 8;
  dateAxis.renderer.ticks.template.strokeOpacity = 0.1;


  let series = setSeries(chart, "Temp", "Time", "Temp", null, null, true);
  setSeries(chart, "TempBmp", "Time", "TempBmp");
  setSeries(chart, "PressureBmp", "Time", "PressureBmp");
  setSeries(chart, "VBat", "Time", "VBat", 2, 4);

  chart.cursor = new window.am4charts.XYCursor();
  chart.cursor.xAxis = dateAxis;
  chart.cursor.behavior = "non"; // выделение области

  var scrollbarX = new window.am4charts.XYChartScrollbar();
  scrollbarX.series.push(series);
  chart.scrollbarX = scrollbarX;

  chart.legend = new window.am4charts.Legend();
  chart.legend.parent = chart.plotContainer;
  chart.legend.zIndex = 100;
  chart.legend.valueLabels.template.text = "{valueY.value.formatNumber('$#.')}";
}