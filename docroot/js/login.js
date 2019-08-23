let submit = document.getElementById("submit");
var xhr = new XMLHttpRequest();

// Обработчик нажатия кнопок
function usersEvent(e) {
    // создать объект для формы
    let formData = new FormData(e.currentTarget.form);

    let pas = formData.get("password");
    if (pas != null) {
        let hash = hex_sha256(pas);
        formData.set("password", hash);
    }

    // отослать
    xhr.open("POST", "/login.html");
    xhr.onload = function () {
        if (xhr.status === 200) {
            // getUsers();
            document.location.href = "/index.html";
        }
    };
    xhr.send(formData);

}

submit.addEventListener('click', usersEvent);