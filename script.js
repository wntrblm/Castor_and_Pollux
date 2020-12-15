(function(){
    "use strict";

    let nav_expand_button = document.querySelector("button.navbar-toggler");

    if(nav_expand_button === null) {
        return;
    }

    let nav_target = document.querySelector(nav_expand_button.dataset.target);
    console.log(nav_expand_button);
    console.log(nav_target);
    console.log(nav_expand_button.dataset.target);

    nav_expand_button.addEventListener("click", function() {
        nav_target.classList.toggle("show");
    });
})();