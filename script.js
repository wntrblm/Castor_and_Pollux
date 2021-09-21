document.addEventListener("DOMContentLoaded", () => {
    "use strict";

    /* Setup hamburger menus. */
    document.querySelectorAll(".navbar-burger").forEach((el) => {
        el.addEventListener("click", () => {
            // Get the target from the "data-target" attribute
            const target = el.dataset.target;
            const $target = document.getElementById(target);

            // Toggle the "is-active" class on both the "navbar-burger" and the "navbar-menu"
            el.classList.toggle("is-active");
            $target.classList.toggle("is-active");
        });
    });

    /* Check for copy-to-clipboard buttons */
    document.querySelectorAll("button[data-clipboard-copy-target]").forEach((el) => {
        el.addEventListener("click", () => {
            const target = document.getElementById(el.dataset.clipboardCopyTarget);
            const originalInnerHtml = el.innerHTML;

            navigator.clipboard.writeText(target.value);
            el.innerHTML = '<span class="material-icons">done</span>';
            setTimeout(() => {el.innerHTML = originalInnerHtml;}, 1000);
        });
    });
});
