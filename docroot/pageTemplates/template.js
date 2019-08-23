let topMenu = document.getElementById("Topnav"),
    templste = document.getElementById("templste"),
    menu = document.getElementById("menu");

// let main = document.getElementsByClassName("main")[0];

menu.innerHTML = templste.innerHTML;
templste.classList.add("active");

function menuFunc() {
    if (topMenu.className === "topnav") {
        topMenu.className += " responsive";
    } else {
        topMenu.className = "topnav";
    }
}