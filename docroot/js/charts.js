'use strict';

// Set theme
window.am4core.useTheme(window.am4themes_animated);

let setcharts = document.getElementById("setcharts");
var logData;
var listLogFile, currentPoint, currentSettings, requestList, request;
var arrFiles = [];
var selects = [];

/**
 * обработчик данных с сервера
 */
function handlerListFiles() {
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
          // разбор массива на отдельные массивы и установка их в объект
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
          checkBox.id = key;
          let Str = "" + key;
          let find = Str.match(/[0-9]{1,2}/i);
          if (find.length > 1) {
            console.log(`error name file: ${Str}`);
          } else {
            cellID.innerHTML = find[0];
          }

          cell1.appendChild(checkBox);
          selects[i] = document.createElement("select");
          selects[i].className = "listfiles";

          arr.forEach(element => {
            let option = document.createElement("option");
            let tempsStr = "" + arrFiles[i];
            let Find = tempsStr.match(/[0-9]{2}.[0-9]{2}.[0-9]{2}/i);
            if (Find.length > 1) {
              console.log(`error name file: ${tempsStr}`);
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
    console.log(performance.now() - time);
  }
}
/**
 * запрос всех файлов логирования 
 * */
function getFilesList() {

  requestList = new XMLHttpRequest();
  requestList.open('GET', '/vf/allFile');
  requestList.onreadystatechange = handlerListFiles;
  requestList.send();

}
/**
 * обработчик данных с сервера
 */
function handlerRequest() {
  if (request.readyState === 4 && request.status === 200) {

    let type = request.getResponseHeader('Content-Type');
    let rawText = request.responseText;
    listLogFile = JSON.parse(rawText);

    var arrFiles = [];

    for (const key in listLogFile) {
      if (listLogFile.hasOwnProperty(key)) {
        arrFiles.push(listLogFile[key]);
      }
    }

    // проверка пришедших данных будет тут
    if (arrFiles.length != 0) {
      var requestLog = new XMLHttpRequest();
      requestLog.open('GET', arrFiles[1]);
      requestLog.onreadystatechange = function () {
        if (requestLog.readyState === 4 && requestLog.status === 200) {

          let type = requestLog.getResponseHeader('Content-Type');
          var arrJSON = '[' + requestLog.responseText + ']';
          logData = JSON.parse(arrJSON);
          createChart();
        }
      };
      requestLog.send();

    }

  }
}
/**
 * Запрос данных с сервера
 */
document.addEventListener('DOMContentLoaded', function () {
  request = new XMLHttpRequest();
  request.open('GET', '/vf/listFile');
  request.onreadystatechange = handlerRequest;
  request.send();
  getFilesList();
});

/**
 * 
 */
function setSeries(pChart, nameSeries, dateX, valueY, min, max) {
  let valueAxis = pChart.yAxes.push(new window.am4charts.ValueAxis());
  valueAxis.tooltip.disabled = true;
  valueAxis.renderer.opposite = true;
  valueAxis.renderer.grid.template.disabled = true;
  if(min !== undefined){
    valueAxis.min = min;
  }
  if(max !== undefined){
    valueAxis.max = max;
  }

  let series = pChart.series.push(new window.am4charts.LineSeries());
  series.dataFields.dateX = dateX;
  series.dataFields.valueY = valueY;
  series.yAxis = valueAxis;
  series.tooltipText = "{valueY.value}";
  series.name = nameSeries;
  series.hidden = true;
  series.sequencedInterpolation = true;

  return series;
}
//отрисовка графиков
function createChart() {
  var chart = window.am4core.create("chartdiv", window.am4charts.XYChart);
  chart.data = logData;
  //chart.dataDateFormat = "JJ:NN:SS";
  //формат разбора данных из строки
  chart.dateFormatter.inputDateFormat = "HH:mm:ss";


  var dateAxis = chart.xAxes.push(new window.am4charts.DateAxis());
  //dateAxis.renderer.
  dateAxis.renderer.grid.template.location = 0;
  dateAxis.renderer.ticks.template.length = 8;
  dateAxis.renderer.ticks.template.strokeOpacity = 0.1;

  var valueAxis = chart.yAxes.push(new window.am4charts.ValueAxis());
  valueAxis.tooltip.disabled = true;

  var series = chart.series.push(new window.am4charts.LineSeries());
  series.dataFields.dateX = "Time";
  series.dataFields.valueY = "Temp";
  series.tooltipText = "{valueY.value}";
  series.name = "Temp";
  series.sequencedInterpolation = true;

  setSeries(chart, "TempBmp", "Time", "TempBmp");
  setSeries(chart, "PressureBmp", "Time", "PressureBmp");
  setSeries(chart, "VBat", "Time", "VBat", 2, 4,5);

  chart.cursor = new window.am4charts.XYCursor();
  chart.cursor.xAxis = dateAxis;
  chart.cursor.behavior = "non";

  var scrollbarX = new window.am4charts.XYChartScrollbar();
  scrollbarX.series.push(series);
  chart.scrollbarX = scrollbarX;

  chart.legend = new window.am4charts.Legend();
  chart.legend.parent = chart.plotContainer;
  chart.legend.zIndex = 100;
  chart.legend.valueLabels.template.text = "{valueY.value.formatNumber('$#.')}";
}