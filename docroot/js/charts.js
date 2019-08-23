'use strict';

// let topMenu = document.getElementById("Topnav"),
//     charts = document.getElementById("charts"),
//     menu = document.getElementById("menu");

// menu.innerHTML = charts.innerHTML;
// charts.classList.add("active");

// if(typeof(EventSource) !== "undefined") 
// {
//   var source = new EventSource("/sse");
//   source.onmessage = function(event) 
//   {
//     console.log(event.type);
//     console.log(event.data);
//     // source.close();
//   };
// } 
// else 
// {
//   console.log("Sorry, your browser does not support server-sent events...");
// }

// Set theme
window.am4core.useTheme(window.am4themes_animated);

// var data2 = [];
// var visits = 10;
// for (var i = 1; i < 20; i++) {
//     visits += Math.round((Math.random() < 0.5 ? 1 : -1) * Math.random() * 5);
//     data2.push({ date: new Date(2018, 0, i), value: visits });
// }
var logData;
var listLogFile, currentPoint, currentSettings;


/**
 * обработчик данных с сервера
 */
function handlerRequest() {
  if (requestList.readyState === 4 && requestList.status === 200) {

    let type = requestList.getResponseHeader('Content-Type');
    let rawText = requestList.responseText;
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
var requestList = new XMLHttpRequest();
requestList.open('GET', '/vf/listFile');
requestList.onreadystatechange = handlerRequest;
requestList.send();

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
  series.name = "Series 1";
  series.sequencedInterpolation = true;

  var valueAxis2 = chart.yAxes.push(new window.am4charts.ValueAxis());
  valueAxis2.tooltip.disabled = true;
  valueAxis2.renderer.opposite = true;
  valueAxis2.renderer.grid.template.disabled = true;

  var series2 = chart.series.push(new window.am4charts.LineSeries());
  series2.dataFields.dateX = "Time";
  series2.dataFields.valueY = "PressureBmp";
  series2.yAxis = valueAxis2;
  series2.tooltipText = "{valueY.value}";
  series2.name = "Series 2";
  series2.sequencedInterpolation = true;

  var valueAxis3 = chart.yAxes.push(new window.am4charts.ValueAxis());
  valueAxis3.tooltip.disabled = true;
  valueAxis3.renderer.opposite = true;
  valueAxis3.renderer.grid.template.disabled = true;

  var series3 = chart.series.push(new window.am4charts.LineSeries());
  series3.dataFields.dateX = "Time";
  series3.dataFields.valueY = "VBat";
  series3.yAxis = valueAxis3;
  series3.tooltipText = "{valueY.value}";
  series3.name = "Series 3";
  series3.sequencedInterpolation = true;

  
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