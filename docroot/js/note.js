// var handlerMenu = function (e) {
//     e.preventDefault();
//     let request = new XMLHttpRequest();
//     request.open("GET", e.target.href);
//     request.setRequestHeader("Content-type", "text/html; charset=utf-8");
//     request.onreadystatechange = function () {
//         if (request.readyState === 4 && request.status === 200) {
//             main.innerHTML = request.responseText;
//             topMenu.className = "topnav";
//             menu.innerHTML = e.target.innerHTML;
//         }
//     };
//     request.send();
// };

// data.addEventListener('click', handlerMenu);
// charts.addEventListener('click', handlerMenu);
// settings.addEventListener('click', handlerMenu);
// control.addEventListener('click', handlerMenu);

// let event1 = new Event("click");
// data.dispatchEvent(event1);


// sse протокол
// if (typeof (EventSource) !== "undefined") {
//     var source = new EventSource("/sse");

//     source.addEventListener("message", function (event) {
//         div.innerHTML = "";
//         // console.log(event.type);
//         // console.log(event.data);
//         // source.close();
//         let pointObj = JSON.parse(event.data);
//         setTable(pointObj, div);

//     });
// } else {
//     console.log("Sorry, your browser does not support server-sent events...");
// }



//Отправление настроек
// var request2 = new XMLHttpRequest();
// request2.open('POST', 'sourseset');
// request2.onreadystatechange = function () {
//   if (request2.readyState === 4 && request2.status === 200) {
//     let type = request2.getResponseHeader('Content-Type');
//     var str1 = request2.responseText;
//   }
// };
// request2.send("id=2;slep=10 set?");

// //отправка пост запроса
// LogOut = function (key, val) {
//     let xhr = new XMLHttpRequest();
//     // отослать
//     let param = key + "=" + val;
//     xhr.open("POST", "/login.html",true);
//     xhr.setRequestHeader('Content-Type', 'application/x-www-form-urlencoded');
//     xhr.send(param);

// };
